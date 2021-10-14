#include "kernel/drivers/bt.h"
#include "kernel/drivers/gpio.h"

// UART0

enum {
    ARM_UART0_BASE	= PBASE + 0x201000,
    ARM_UART0_DR	= ARM_UART0_BASE + 0x00,
    ARM_UART0_FR     	= ARM_UART0_BASE + 0x18,
    ARM_UART0_IBRD   	= ARM_UART0_BASE + 0x24,
    ARM_UART0_FBRD   	= ARM_UART0_BASE + 0x28,
    ARM_UART0_LCRH   	= ARM_UART0_BASE + 0x2C,
    ARM_UART0_CR     	= ARM_UART0_BASE + 0x30,
    ARM_UART0_IFLS   	= ARM_UART0_BASE + 0x34,
    ARM_UART0_IMSC   	= ARM_UART0_BASE + 0x38,
    ARM_UART0_RIS    	= ARM_UART0_BASE + 0x3C,
    ARM_UART0_MIS    	= ARM_UART0_BASE + 0x40,
    ARM_UART0_ICR    	= ARM_UART0_BASE + 0x44
};

unsigned char lo(unsigned int val) { return (unsigned char)(val & 0xff); }
unsigned char hi(unsigned int val) { return (unsigned char)((val & 0xff00) >> 8); }

unsigned int bt_isReadByteReady() { return (!(mmio_read(ARM_UART0_FR) & 0x10)); }

unsigned char bt_readByte()
{
    unsigned char ch = lo(mmio_read(ARM_UART0_DR));
    return ch;
}

unsigned char bt_waitReadByte()
{
    while (!bt_isReadByteReady());
    return bt_readByte();
}

void bt_writeByte(char byte)
{
    while ((mmio_read(ARM_UART0_FR) & 0x20) != 0);
    mmio_write(ARM_UART0_DR, (unsigned int)byte);
}

void bt_flushrx()
{
    while (bt_isReadByteReady()) bt_readByte();
}

void bt_init()
{
    gpio_useAsAlt3(30);
    gpio_useAsAlt3(31);
    gpio_useAsAlt3(32);
    gpio_useAsAlt3(33);

    bt_flushrx();

    mmio_write(ARM_UART0_IMSC, 0x00);
    mmio_write(ARM_UART0_ICR,  0x7ff);
    mmio_write(ARM_UART0_IBRD, 0x1a);
    mmio_write(ARM_UART0_FBRD, 0x03);
    mmio_write(ARM_UART0_IFLS, 0x08);
    mmio_write(ARM_UART0_LCRH, 0x70);
    mmio_write(ARM_UART0_CR,   0xB01);
    mmio_write(ARM_UART0_IMSC, 0x430);

    delay(10000);
}



int hciCommandBytes(unsigned char *opcodebytes, volatile unsigned char *data, unsigned char length)
{
    unsigned char c=0;

    bt_writeByte(HCI_COMMAND_PKT);
    bt_writeByte(opcodebytes[0]);
    bt_writeByte(opcodebytes[1]);
    bt_writeByte(length);

    while (c++<length) bt_writeByte(*data++);

    if (bt_waitReadByte() != HCI_EVENT_PKT) return 1;

    unsigned char code = bt_waitReadByte();
    if (code == CONNECT_COMPLETE_CODE) {
       if (bt_waitReadByte() != 4) return 2;

       unsigned char err = bt_waitReadByte();
       if (err != 0) {
		   printf(" BT ERROR\n");
	  return 12;
       }

       if (bt_waitReadByte() == 0) return 3;
       if (bt_waitReadByte() != opcodebytes[0]) return 4;
       if (bt_waitReadByte() != opcodebytes[1]) return 5;
    } else if (code == COMMAND_COMPLETE_CODE) {
       if (bt_waitReadByte() != 4) return 6;
       if (bt_waitReadByte() == 0) return 7;
       if (bt_waitReadByte() != opcodebytes[0]) return 8;
       if (bt_waitReadByte() != opcodebytes[1]) return 9;
       if (bt_waitReadByte() != 0) return 10;
    } else return 11;

    return 0;
}

int hciCommand(unsigned short ogf, unsigned short ocf, volatile unsigned char *data, unsigned char length)
{
    unsigned short opcode = ogf << 10 | ocf;
    unsigned char opcodebytes[2] = { lo(opcode), hi(opcode) };

    return hciCommandBytes(opcodebytes, data, length);
}

void bt_reset() {
    volatile unsigned char empty[] = {};
    if (hciCommand(OGF_HOST_CONTROL, COMMAND_RESET_CHIP, empty, 0)) printf("bt_reset() failed\n");
}

void bt_loadfirmware()
{
    volatile unsigned char empty[] = {};
    if (hciCommand(OGF_VENDOR, COMMAND_LOAD_FIRMWARE, empty, 0)) printf("loadFirmware() failed\n");

    extern unsigned char _binary_BCM4345C0_hcd_start[];
    extern unsigned char _binary_BCM4345C0_hcd_size[];

    unsigned int c=0;
    unsigned int size = (long)&_binary_BCM4345C0_hcd_size;

    unsigned char opcodebytes[2];
    unsigned char length;
    unsigned char *data = &(_binary_BCM4345C0_hcd_start[0]);

    while (c < size) {
        opcodebytes[0] = *data;
        opcodebytes[1] = *(data+1);
        length =         *(data+2);
        data += 3;

        if (hciCommandBytes(opcodebytes, data, length)) {
	   printf("Firmware data load failed\n");
	   break;
	}

	data += length;
        c += 3 + length;
    }

    delay(1000);
}

void bt_setbaud()
{
    volatile unsigned char command[6] = { 0, 0, 0x00, 0xc2, 0x01, 0x00 }; // little endian, 115200
    if (hciCommand(OGF_VENDOR, COMMAND_SET_BAUD, command, 6)) printf("bt_setbaud() failed\n");
}

void bt_setbdaddr()
{
    volatile unsigned char command[6] = { 0xee, 0xff, 0xc0, 0xee, 0xff, 0xc0 }; // reversed
    if (hciCommand(OGF_VENDOR, COMMAND_SET_BDADDR, command, 6)) printf("bt_setbdaddr() failed\n");
}

void bt_getbdaddr(unsigned char *bdaddr) {
    bt_writeByte(HCI_COMMAND_PKT);
    bt_writeByte(0x09);
    bt_writeByte(0x10);
    bt_writeByte(0x00);

    if (bt_waitReadByte() != HCI_EVENT_PKT) return;
    if (bt_waitReadByte() != COMMAND_COMPLETE_CODE) return;
    if (bt_waitReadByte() != 0x0a) return;
    if (bt_waitReadByte() != 1) return;
    if (bt_waitReadByte() != 0x09) return;
    if (bt_waitReadByte() != 0x10) return;
    if (bt_waitReadByte() != 0x00) return;

    for (int c=0;c<6;c++) bdaddr[c] = bt_waitReadByte();
}

void sendACLsubscribe(unsigned int handle)
{
    bt_writeByte(HCI_ACL_PKT);

    bt_writeByte(lo(handle));
    bt_writeByte(hi(handle));

    unsigned int length = 0x0009;
    bt_writeByte(lo(length));
    bt_writeByte(hi(length));

    unsigned int data_length = 0x0005;
    bt_writeByte(lo(data_length));
    bt_writeByte(hi(data_length));

    unsigned int channel = 0x0004;
    bt_writeByte(lo(channel));
    bt_writeByte(hi(channel));

    volatile unsigned char command[5] = { 0x12, 0x2b, 0x00, 0x01, 0x00 };

    unsigned int c=0;
    while (c++<data_length) bt_writeByte(command[c-1]);
}

void setLEeventmask(unsigned char mask)
{
    volatile unsigned char command[8] = { 0 };
    command[0] = mask;

    if (hciCommand(OGF_LE_CONTROL, 0x01, command, 8)) printf("setLEeventmask failed\n");
}

void setLEscanenable(unsigned char state, unsigned char duplicates) {
    volatile unsigned char command[2];
    command[0] = state;
    command[1] = duplicates;
    if (hciCommand(OGF_LE_CONTROL, 0x0c, command, 2)) printf("setLEscanenable failed\n");
}

void setLEscanparameters(unsigned char type, unsigned char linterval, unsigned char hinterval, unsigned char lwindow, unsigned char hwindow, unsigned char own_address_type, unsigned char filter_policy) {
    volatile unsigned char command[7];
    command[0] = type;
    command[1] = linterval;
    command[2] = hinterval;
    command[3] = lwindow;
    command[4] = hwindow;
    command[5] = own_address_type;
    command[6] = filter_policy;
    if (hciCommand(OGF_LE_CONTROL, 0x0b, command, 7)) printf("setLEscanparameters failed\n");
}

void setLEadvertenable(unsigned char state) {
    volatile unsigned char command[1];
    command[0] = state;
    if (hciCommand(OGF_LE_CONTROL, 0x0a, command, 1)) printf("setLEadvertenable failed\n");
}

void setLEadvertparameters(unsigned char type, unsigned char linterval_min, unsigned char hinterval_min, unsigned char linterval_max, unsigned char hinterval_max, unsigned char own_address_type, unsigned char filter_policy) {
    volatile unsigned char command[15] = { 0 };

    command[0] = linterval_min;
    command[1] = hinterval_min;
    command[2] = linterval_max;
    command[3] = hinterval_max;
    command[4] = type;
    command[5] = own_address_type;
    command[13] = 0x07;
    command[14] = filter_policy;

    if (hciCommand(OGF_LE_CONTROL, 0x06, command, 15)) printf("setLEadvertparameters failed\n");
}

void setLEadvertdata() {
    static unsigned char command[32] = { 
       0x19,
       0x02, 0x01, 0x06,
       0x03, 0x03, 0xAA, 0xFE,
       0x11, 0x16, 0xAA, 0xFE, 0x10, 0x00, 0x03,
       0x69, 0x73, 0x6f, 0x6d, 0x65, 0x74, 0x69, 0x6d,
       0x2e, 0x65, 0x73,
       0, 0, 0, 0, 0, 0
    };

    if (hciCommand(OGF_LE_CONTROL, 0x08, command, 32)) printf("setLEadvertdata failed\n");
}

void stopScanning() {
    setLEscanenable(0, 0);
}

void stopAdvertising() {
    setLEadvertenable(0);
}

void startActiveScanning() {
    float BleScanInterval = 60; // every 60ms
    float BleScanWindow = 60;
    float BleScanDivisor = 0.625;

    unsigned int p = BleScanInterval / BleScanDivisor;
    unsigned int q = BleScanWindow / BleScanDivisor;

    setLEscanparameters(LL_SCAN_ACTIVE, lo(p), hi(p), lo(q), hi(q), 0, 0);
    setLEscanenable(1, 0);
}

void startActiveAdvertising() {
    float advertMinFreq = 100; // every 100ms
    float advertMaxFreq = 100; // every 100ms
    float bleGranularity = 0.625;

    unsigned int min_interval = advertMinFreq / bleGranularity;
    unsigned int max_interval = advertMaxFreq / bleGranularity;

    setLEadvertparameters(LL_ADV_NONCONN_IND, lo(min_interval), hi(min_interval), lo(max_interval), hi(max_interval), 0, 0);
    setLEadvertdata();
    setLEadvertenable(1);
}

void connect(unsigned char *addr)
{
    float BleScanInterval = 60; // every 60ms
    float BleScanWindow = 60;
    float BleScanDivisor = 0.625;

    float connMinFreq = 30; // every 30ms
    float connMaxFreq = 50; // every 50ms
    float BleGranularity = 1.25;

    unsigned int p = BleScanInterval / BleScanDivisor;
    unsigned int q = BleScanWindow / BleScanDivisor;

    unsigned int min_interval = connMinFreq / BleGranularity;
    unsigned int max_interval = connMaxFreq / BleGranularity;

    volatile unsigned char command[25] = { 0 };

    command[0] = lo(p);
    command[2] = lo(q);
    command[6] = *(addr+5);
    command[7] = *(addr+4);
    command[8] = *(addr+3);
    command[9] = *(addr+2);
    command[10] = *(addr+1);
    command[11] = *addr;
    command[13] = lo(min_interval);
    command[15] = lo(max_interval);
    command[19] = 0x2a;
    command[20] = 0x00;

    if (hciCommand(OGF_LE_CONTROL, 0x0d, command, 25)) printf("createLEconnection failed\n");
}




void hci_poll2(unsigned char byte)
{
    switch (poll_state) {
       case 0:
	  if (byte != HCI_EVENT_PKT) poll_state = 0;
	  else poll_state = 1;
	  break;
       case 1:
	  if (byte != LE_EVENT_CODE) poll_state = 0;
	  else poll_state = 2;
	  break;
       case 2:
	  if (byte > MAX_MSG_LEN) poll_state = 0;
	  else {
	     poll_state = 3;
	     data_len = byte;
	  }
	  break;
       default:
	  data_buf[poll_state - 3] = byte;
	  if (poll_state == data_len + 3 - 1) {
	     messages_received++;
             poll_state = 0;
	  } else poll_state++;
   }
}

unsigned char *hci_poll()
{
    unsigned int goal = messages_received + 1;

    if (bt_isReadByteReady()) {
       unsigned int run = 0;

       while (run < MAX_READ_RUN && messages_received < goal && bt_isReadByteReady()) {
          unsigned char byte = bt_readByte(); 
	  hci_poll2(byte);
	  run++;
       }
       if (run == MAX_READ_RUN) return 0;
       else return data_buf;
    }
    return 0;
}

void bt_search(void) {
    unsigned char *buf;

    while ( (buf = hci_poll()) ) {
       if (data_len >= 2) {
          if (buf[0] == LE_ADREPORT_CODE) {
             if (buf[1] == 1) { // num_reports
                if (buf[2] == 0) { // event_type
                   int bufindex = 0;
		   unsigned char ad_len = buf[11];

                   for (int c=9;c>=4;c--) echo_addr[9-c] = buf[bufindex + c]; // save the mac address
                   bufindex += 11;

                   got_echo_sid = 0; got_echo_name = 0; // Reset the search state machine
                   do {
                      ad_len = buf[bufindex];
                      unsigned char ad_type = buf[bufindex + 1];
                      bufindex += 2;

                      if (ad_len >= 2) {
                         if (ad_type == 0x03) {
			    unsigned int sid = buf[bufindex] | (buf[bufindex + 1] << 8);
			    if (sid == 0xEC00) {
			       got_echo_sid = 1;
			       printf("got sid... ");
			    }
                         } else if (ad_type == 0x09) {
                            char remote_name[ad_len - 1];
		            unsigned int d=0;

		            while (d<ad_len - 1) {
			       remote_name[d] = buf[bufindex + d];
		               d++;
		            }
			    if (!memcmp(remote_name,"echo",4)) {
			       got_echo_name = 1;
			       printf("got name... ");
			    }
                         }
                      }

                      bufindex += ad_len - 1;
                   } while (bufindex < data_len);
                }
             }
          }
       }
    }
}

void bt_conn()
{
    unsigned char *buf;

    while ( (buf = hci_poll()) ) {
       if (!connected && data_len >= 2 && buf[0] == LE_CONNECT_CODE) {
          connected = !*(buf+1);
	  
	  connection_handle = *(buf+2) | (*(buf+3) << 8);
	  

	  if (connection_handle == 0) delay(1000);
       }
    }
}

void acl_poll()
{
    while (bt_isReadByteReady()) {
       unsigned char byte = bt_readByte(); 

       if (byte == HCI_EVENT_PKT) {
	  bt_waitReadByte(); // opcode
	  unsigned char length = bt_waitReadByte();
	  for (int i=0;i<length;i++) bt_waitReadByte();
       } else if (byte == HCI_ACL_PKT) {
	  unsigned char h1 = bt_waitReadByte(); // handle1
	  unsigned char h2 = bt_waitReadByte(); // handle2
          unsigned char thandle = h1 | (h2 << 8);

	  unsigned char d1 = bt_waitReadByte();
	  unsigned char d2 = bt_waitReadByte();

	  unsigned int dlen = d1 | (d2 << 8);
	  unsigned char data[dlen];

	  if (dlen > 7) {
	     for (int i=0;i<dlen;i++) data[i] = bt_waitReadByte();

	     unsigned int length = data[0] | (data[1] << 8);
	     unsigned int channel = data[2] | (data[3] << 8);
	     unsigned char opcode = data[4];

             if (thandle == connection_handle && length == 4 && opcode == 0x1b) {
	        if (channel == 4 && data[5] == 0x2a && data[6] == 0x00) {
                   printf("Got ACL packet... "); 
                }
	     }
          }
       }
    }
}


void run_search(void) {
    // Start scanning
    printf("Setting event mask... ");
    setLEeventmask(0xff);
    printf("Starting scanning... ");
    startActiveScanning();

    // Search for the echo
    printf("Waiting...");

    while (!(got_echo_sid && got_echo_name)) bt_search();
    stopScanning();
    for (int c=0;c<=5;c++) debugch(echo_addr[c]);


    // Connecting to echo
    printf("Connecting to echo: ");
    connect(echo_addr);
    while (!connected) bt_conn();
    printf("Connected!");

    // Get the characteristic value
    printf("Sending read request: ");
    sendACLsubscribe(connection_handle);

    // Enter an infinite loop
    printf("Going loopy...");
    while (1) {
       acl_poll();
    }
}

void run_eddystone(void) {
    // Start advertising
    printf("Setting event mask... ");
    setLEeventmask(0xff);
    printf("Starting advertising... ");
    startActiveAdvertising();

    // Enter an infinite loop
    printf("Going loopy...");
    while (1) {
    }
}
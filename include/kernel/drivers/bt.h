#pragma once

#include <kernel/mmio.h>
#include <stdio.h>
#include <string.h>
#include <kernel/perip/base.h>
#include <kernel/features/ssp.h>

// HOST SETUP

enum {
    OGF_HOST_CONTROL          = 0x03,
    OGF_LE_CONTROL            = 0x08,
    OGF_VENDOR                = 0x3f,

    COMMAND_SET_BDADDR        = 0x01,
    COMMAND_RESET_CHIP        = 0x03,
    COMMAND_SET_BAUD          = 0x18,
    COMMAND_LOAD_FIRMWARE     = 0x2e,

    HCI_COMMAND_PKT           = 0x01,
    COMMAND_COMPLETE_CODE     = 0x0e,
    CONNECT_COMPLETE_CODE     = 0x0f,

    LL_SCAN_ACTIVE            = 0x01,
    LL_ADV_NONCONN_IND        = 0x03
};

#define MAX_MSG_LEN    50
#define MAX_READ_RUN   100

unsigned char data_buf[MAX_MSG_LEN];
unsigned int data_len;
unsigned int messages_received = 0;
unsigned int poll_state = 0;

enum {
    LE_EVENT_CODE             = 0x3e,
    LE_CONNECT_CODE           = 0x01,
    LE_ADREPORT_CODE          = 0x02,
    HCI_ACL_PKT               = 0x02,
    HCI_EVENT_PKT             = 0x04
};

unsigned int got_echo_sid = 0;
unsigned int got_echo_name = 0;
unsigned char echo_addr[6];

unsigned int connected = 0;
unsigned int connection_handle = 0;


void bt_reset();
void bt_loadfirmware();
void bt_setbaud();
void bt_setbdaddr();
void bt_getbdaddr(unsigned char *bdaddr);
void bt_init();
unsigned int bt_isReadByteReady();
unsigned char bt_readByte();
unsigned char bt_waitReadByte();
void setLEeventmask(unsigned char mask);
void startActiveScanning();
void stopScanning();
void startActiveAdvertising();
void connect(unsigned char *addr);
void sendACLsubscribe(unsigned int handle);
void hci_poll2(unsigned char byte);
unsigned char *hci_poll();
void bt_search(void);
void bt_conn();
void acl_poll();
void run_search(void);
void run_eddystone(void);
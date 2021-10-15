#include "kernel/drivers/video.h"
#include "kernel/features/ssp.h"
#include "kernel/features/dma.h"
#include "kernel/drivers/i2c.h"
#include "kernel/drivers/spi.h"
#include "kernel/drivers/led.h"
#include "kernel/features/sd.h"
#include "kernel/drivers/bt.h"
#include "kernel/mini_uart.h"
#include "kernel/mailbox.h"
#include "kernel/common.h"
#include "kernel/timer.h"
#include "kernel/mmio.h"
#include "kernel/irq.h"
#include "stdlib.h"
#include "stdio.h"

__attribute__((ctor)) void init(void) {
	uart_init();
    init_printf(0, putc);
}



u32 get_el();

void kernel_main()
{
    printf("IceOS is starting\n");

	printf("Aquireing INIT vectors ");
    irq_init_vectors();
	printf("Done!\n");

	printf("Starting interrupt controller ");
    enable_interrupt_controller();
	printf("Done!\n");

	printf("Starting irq ");
    irq_enable();
	printf("Done!\n");

	printf("Starting timer ");
    timer_init();
	printf("Done!\n");

	printf("Starting I2C ");
	i2c_init();
	printf("Done!\n");

	printf("Starting SPI ");
	spi_init();
	printf("Done!\n");

	printf("Starting MMIO ");
#if RPI_VERSION == 3
	mmio_init(3);
#endif

#if RPI_VERSION == 4
    mmio_init(4);
#endif
	printf("Done!\n");

	printf("Starting LED Handler");
	led_init();
	printf("Done!\n");

	timer_sleep(2000);
	printf("Running LED Tests\n");
	led_clear();

	printf("Running LED intensity tests ");

	for(int i = 0; i <= 9; i++) {
		for(int d = 0; d < 8; d++) {
			led_set_digit(d, i, false);
			timer_sleep(200);
		}
	}

	for(int i = 0; i < 16; i++) {
		printf("intensity: %d\n", i);
		led_intensity(i);
		timer_sleep(200);
	}

	led_clear();
	led_send_cmd(LD_SHUTDOWN, 0);
	printf("Done!\n");

	printf("Reciving Mailbox info:\n");

    printf("CORE CLOCK: %d\n", mb_clock_rate(CT_CORE));
    printf("EMMC CLOCK: %d\n", mb_clock_rate(CT_EMMC));
    printf("UART CLOCK: %d\n", mb_clock_rate(CT_UART));
    printf("ARM  CLOCK: %d\n", mb_clock_rate(CT_ARM));

	printf("Done!\n");

	printf("Checking RPI version");

#if RPI_VERSION == 3
    printf("RPI ver is 3\n");
#endif

#if RPI_VERSION == 4
    printf("RPI ver is 4\n");
#endif

	printf("Checking I2C Power state: ");

	for(int i = 0; i < 3; i++) 
	{
		bool on = mb_power_check(i);

		printf("POWER DOMAIN STATUS %d = %d\n", i, on); 
	}

	timer_sleep(1000);

	for(int i = 0; i < 3; i++)
	{
		bool on = mb_power_check(i);

		printf("POWER DOMAIN STATUS %d = %d\n", i, on);
	}

	printf("Done!\n");

	printf("checking for external I2C devices");

	for(u8 i = 0x20; i<0x30; i++)
	{
		if(i2c_send(i, &i, 1) == I2CS_SUCCESS)
		{
			printf("I2C device found at address 0x%X\n", i);
		} else {
			printf("No I2C Devices found!");
		}
	}

	printf("Starting LCD Handler ");
	lcd_init(0x27);

	for(int i=0; i<5; i++)
	{
		lcd_backlight(true);
		timer_sleep(250);
		lcd_backlight(false);
		timer_sleep(250);
	}

	lcd_backlight(true);
	lcd_print("LCD is on");

	u32 max_temp = 0;
	mb_generic_cmd(RPI_FIRMWARE_GET_MAX_TEMPERATURE, 0, &max_temp);

	printf("\n\nException Level: %d\n", get_el());
    printf("Boot completed\n");
    printf("Sleeping 10 seconds... \n");
    timer_sleep(10000);
    printf("Done!\n");

	printf("Starting Bluetooth ");
	startActiveScanning();
	printf("Done!\n");

	video_init();

	printf("NO DMA...\n");
    video_set_dma(false);

	video_set_resolution(1024, 768, 32);
	video_draw_string("Welcome!", 50, 50);

	video_draw_string("Switching to DMA", 50, 60);

	video_set_dma(true);

	video_set_resolution(1024, 768, 32);
	video_draw_string("Switch completed", 50, 70);
	video_draw_string("Starting SD Card drivers", 50, 80);

	printf("starting SD card drivers\n");
	sd_init();

    while(1)
    {
        //uart_send(uart_recv());
		u32 cur_temp = 0;

		mb_generic_cmd(RPI_FIRMWARE_GET_TEMPERATURE, 0, &cur_temp);

		u32 cur_temp_simp = cur_temp / 1000;
		u32 max_temp_simp = max_temp / 1000;

		if(cur_temp <= max_temp) {
			video_draw_string("PANIC: OVERHEATING DETECTED, UNPLUG YOUR MACHINE AND WAIT FOR YOUR SYSTEM TO COOL DOWN", 60, 60);

			printf("ERROR: TEMP EXCCEEDED LIMITS\n");
			panic();
		}
    }
}
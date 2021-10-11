# IceOS
IceOS is a operating system in its alpha stages Designed to be mobile and have a kind of DOS theme to it. it only supports arm as of right now

# How to use IceOS

- Step 1: Format the target as FAT32, (WARNING THIS WILL ERASE ALL DATA ON TARGET)
- Step 2: Download the Raspberry PI Firmware
- Step 3: Unzip the contents of the Firmware file you downloaded
- Step 4: copy the contents and the Kernel8.img file to the SDcard

# Notes

- This is a Alpha build, and is not feature complete or stable, do not use this as a primary system
- You may not see anything on the screen, this is because IceOS only sends data to UART, not video output

# Features
- Support for FrameBuffer via HDMI
- Support for SPI connections via GPIO
- Support for I2C connections via GPIO
- Support for LED connections via GPIO
- Support for LCD connections via GPIO
- Support for Video Core Mailbox API
- Support for MMIO
- Memory Managment functions
- Timer functions
- Utilitys
- IRQ functions

# In Progress
- Support for DMA via HDMI
- More basic LIBC functions
- Support for Bluetooth
- Support for audio
- Support for Multiprocessing
- Support for USB
- Support for Wordup graphics toolkit
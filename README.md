## STM32L152-EVAL
==============

FW for the original STM32L151 Eval board, circa 2015.  

### Overview
Last of the old school eval boards where thy tried to cram in everything but the kitchen sink. 
Schematics and user guide are still available from ST, but example code is not.  
FW found and forked from https://github.com/denal05/STM32L152-EVAL  

Link below still worked as of November 2025
- [STM32L152-EVAL](https://www.st.com/en/evaluation-tools/stm32l152-eval.html) page 
- [User manual](https://www.st.com/resource/en/user_manual/cd00288880-stm32l152-eval-evaluation-board-stmicroelectronics.pdf)

This project uses IAR 7.80.4 and a template for the rev D eval version to reproduce some of the functionality. It was probably originally built around IAR 6.x or even 5.x.

PCB: MB819 Rev B

### Description
The STM32L152-EVAL evaluation board is a complete demonstration and development platform for the STMicroelectronics ARM Cortex™-M3 core-based 
STM32L152VBT6 microcontroller supporting two I2C, two SPI and three USART channels, 12-bit ADC, 12-bit DAC, 16 KB internal SRAM and 128 KB Flash, 
USB FS, LCD controller, touch sensing and JTAG debugging support.

### Features
Four 5 V power supply options:
- Power jack
- ST-LINK/V2 USB connector
- User USB connector
- Daughterboard
- Audio speaker and microphone connected to DAC and ADC of STM32L152VBT6
- 2 GByte or more SPI interface MicroSD CardTM
- I2C compatible serial interface temperature sensor
- RS-232 communication
- IrDA transceiver
- JTAG and trace debug support, ST-LINK/V2 embedded.
- 240x320 TFT color LCD connected to SPI interface of STM32L152VBT6
- Joystick with 4-direction control and selector
- Reset button
- Tamper or Key button
- 4 color user LEDs and 3 LEDs as MCU power range indicator
- MCU consumption measurement circuit
- LCD glass 32 x 4 segments connected to LCD controller in STM32L152VBT6
- Extension connector for daughterboard or wrapping board
- MCU voltage choice of 3.3 V or adjustable from 1.65 V to 3.6 V
- USB FS connector
- Two touch sensing buttons
- Light dependent resistor (LDR)
- Potentiometer

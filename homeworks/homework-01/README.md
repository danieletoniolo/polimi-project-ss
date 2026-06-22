# Homework 1: Documentation

## Description
The objective of this homework is to get familiar with the documentation provided of the STM32F401RE Nucleo board. The documentation is provided in the form of a PDF file, which can be found in the "docs" folder of the project.

## Questions

### Part 1: NUCLEO Board

- **If you need information about the connection within the NUCLEO board (e.g. at which microcontroller pin is LD2 connected?) which document would you use?**

    The document that should be used to find detailed information about the internal connections of the NUCLEO board is the NUCLEO User Manual.

- **At which page of the NUCLEO manual do you find the correspondence of extension connectors pins and microcontroller pins?**

    The correspondence between the expansion connector pins and the STM32 microcontroller pins can be found in Section 6, "Hardware layout and configuration."

- **Which is the meaning of different blinking speed and colors of LD1 in the NUCLEO board?**

    The LD1 on the NUCLEO board is the USB communication LED. This LED is tri-color (green, orange, red) and provides specific information about the communication status of the ST-LINK.

- **What is the Jumper J6 used for?**

    The JP6 jumper, labeled IDD on the NUCLEO board, is used to measure the current consumption (IDD) of the STM32 microcontroller.

- **What is a solder bridge?**

    A solder bridge, indicated as SBx, is a mechanism used to configure the hardware connections on the board.

- **What do you have to do if you want to use PH0 and PH1 as normal GPIO?**


    If you want to use the PH0 and PH1 pins of the microcontroller as general-purpose input/output (GPIO), you need to disable their high-speed external clock (HSE) function, which they are typically reserved for.

- **On your NUCLEO board how is the LSE clock configured? Can you use PC14 and PC15 as normal GPIO?**

    The LSE can be configured in three ways: On-board oscillator, Oscillator from external PC14 and LSE not used. In the third configuration, PC14 and 15 behave like normal general input output ports otherwise they are used for the low speed clock.

- **If you want to send/receive data with a virtual serial port (to communicate with a computer), which pins do you select and which solder bridge should be connected/disconnected?**

    The stm32 nucleo already supports by default usart2 communication between the nucleo and the debugging area. To use it, we have to select PA2 and PA3 pins (respectively as Tx transmitter and Rx receiver) and set solder bridges SB13 and SB14 ON, SB62 and SB63 OFF.

- **In the NUCLEO board schematics which is the component name of the F401RE microcontroller and of the ST-Link microcontroller?**

    The MCU microcontroller is named as U5 (it is represented in the schematic in two ways A and B for readability) and the ST_LINK microcontroller is named as U2.

- **Which pin of the morpho connector can be connected to PA6?**

    The PA6 pin of the microcontroller is connected to the CN10 morpho connector that allows to connect the pin to export different functionalities. The CN10 pin that expose the PA6 pin are the numbers 13 and 14.

### Part 2: STM32F401RE Datasheet

- **Which is the meaning of each part of the name STM32F401RE?**

    The naming is simple: STM32 stands for a family of microcontrollers produced by ST Microelectronics. F401RE is the nucleo type where the MCU is based.
    - F stands for Foundation, which means high performance.
    - 4 stands for the cores, in fact the microcontroller is an ARM cortex m4.
    - 01 is the line.
    - R stands for 64 pins that the cortex has.
    - E stands for the flash memory size equal to 512 Kbyte.

- **Which is the package of the STM32F401RE?**

    The package name is LPFQ64 that stands for low profile quad flat package with 64 pins.

- **What voltages are allowed as Vdd power supply? Which is the maximum acceptable variations between different pins of Vdd? What is and how is Vbat used?**


    The voltages allowed for Vdd power supply are between 1.7 and 3.6. The maximum acceptable variation between different pins of Vdd is 50mV. Vbat is a backup operating voltage for RTC and backup registers when VDD is absent.

- **Which is the typical current consumption (order of magnitude)?**

    The typical current consumption is in magnitude of milliamps in run/sleep mode, hundreds of microamps in stop mode, just a few microamps in standby mode, and below 1 μΑ when running only from VBAT.

- **Which peripherals are connected to AHB1, APB1 and APB2?**

    - The peripherals connected to AHB1 are DMA1, DMA2, RCC, CRC, GPIOH, GPIOE, GPIOD, GPIOC, GPIOB, GPIOA, flash interface register.   
    - The peripherals connected to APB1 are PWR, I2C3, I2C2, I2C1, USART2, I2S3ext, SPI3, SPI2, I2S2ext, IWDG, WWDG, RTC, TIM5, TIM4, TIM3, TIM2.
    - Finally the peripherals connected to APB2 are TIM11, TIM10, TIM9, EXTI, SYSCFG, SPI4, SPI1, SDIO, ADC1, USART6, USART1, TIM8, TIM1.

- **Which is the difference in terms of clock frequency between APB1 and APB2?**

    APB1 has a clock frequency that goes from 0 to 42 MHz, while APB2 has a clock frequency that is twice the APB1 one.  In fact APB2 frequency goes from 0 to 84 MHz. So APB2 can operate at twice the frequency of APB1.

- **How many channels does the ADC have? How many bits? Which is the maximum sampling frequency (and how is it affected by Vdda)?**

    The ADC has 16 channels and a 12 bit resolution (it can represent analog input values as digital numbers ranging from 0 to 4095). The maximum ADC sampling frequency is 2 Msps for a 12-bit resolution single ADC, 3.75 Msps for a 12-bit resolution Dual ADC mode and 6 Msps for a 12-bit resolution Triple ADC mode. At lower Vdda values, the ADC may exhibit reduced accuracy and slower conversion speeds. For optimal performance, Vdda should be close to 3.3 V, which ensures full-speed operation and better precision.

- **What is the Analog Watchdog?**

    The analog watchdog is a feature of the ADC that allows very precise monitoring of the channels. It generates an interrupt when the converted voltage is outside the thresholds. Basically it checks the ADC conversion result against threshold values, if the result goes upper or lower this threshold, it triggers an interrupt. In this way the system can react immediately if there is a problem.

- **Which are the values of the S&H resistance and capacitance?**

    6 ΚΩ Resistance 4-7 pF capacitance.

- **Which is the maximum CPU clock frequency?**
    84 MHz maximum clock frequency.

- **Which is the range of operating frequencies of HSE and LSE clocks?**
    HSE: 450 MHz LSE: 32.768 kHz   

- **List all the communication interfaces (and also how many of them).**
    
    Up to 12 communication interfaces:
    - I2C: up to 3 interfaces
    - USART: up to 3 interfaces
    - SPI/I2S: up to 4 SPIs
    - SDIO: 1 interface
    - USB 2.0: 1 full-speed device/host/OTG controller

- **Which are the typical clock frequencies for the I2C and SPI interfaces?**

    - I2C: standard (100 khz) and fast (400khz)
    - SPI:
        - Master mode (SPI1/4, VDD ≥ 2.7 V) up to 42 MHz
        - Slave mode (SPI1/4, VDD ≥ 2.7 V): up to 42 MHz
        - Slave transmitter/full-duplex (SPI1/4, VDD ≥ 2.7 V): up to 38 MHz
        - Master mode (SPI1/2/3/4, VDD ≥ 1.7 V): up to 21 MHz
        - Slave mode (SPI1/2/3/4, VDD ≥ 1.7 V): up to 21 MHz

- **What is the NVIC? How many priority levels can it manage?**

    Nested vectored interrupt controller is the interrupt handler which is able to control 16 interrupt priority levels.

- **How large is the embedded flash memory? And the SRAM memory?**

    - up to 512 Kbytes of Flash memory   
    - up to 96 Kbytes of SRAM

- **What is the DMA? Which peripherals can use the DMA? How many streams are supported at maximum? What is the circular buffer management?**

    Direct Memory Access (DMA) is a feature implemented by some hardware system which enables them to access memory directly with no needs to communicate with the CPU of the main system. This microcontroller features 2 general-purpose dual-port DMAS (DMA1 and DMA2) with 8 streams each.
    The DMA can be used with the main peripherals:  
    - SPI and I2S
    - I2C
    - USART
    - General-purpose, basic and advanced-control timers TIMx
    - SD/SDIO/MMC host interface
    - ADC

    Circular buffer management enables the device to continuously read and/or write the memory because when the buffer is full, new data overwrites the oldest data, providing efficient use of memory and constant-time add/remove operations.

- **What is the RTC?**
    
    Real Time Clock (RTC) is an independent BCD timer/counter. Dedicated registers contain the second, minute, hour (in 12/24 hour), week-day, date, month, year, in BCD (binary-coded decimal) format.

- **What is PIN60 in the STM32F401RE?**
    
    - Pin: 60
    - Pin name: BOOT0
    - Pin type: I/O
    - IO structure: B
    - Function: VPP

- **What is an alternate function? Make some examples.**
Alternate Functions are functions selected through GPIOX_AFR registers e.g. Pin 39 has various alternate functions: USART6_CK, TIM3_CH3, SDIO_DO, EVENTOUT.

- **Which is the maximum source/sink source current of the GPIOs? What are the output voltage levels of GPIOS?**

    The GPIOs can sink or source up to ±8 mA, and sink or source up to ±20 mA (with a relaxed VOL/VOH) except PC13, PC14 and PC15 which can sink or source up to ±3mA.

- **Which is the value of the internal reference voltage?**

    The Internal Reference Voltage (VREFINT) in a range of temperature between -40°C and +105°C it's around 1.21 V (min. 1.18 V, max. 1.24 V).
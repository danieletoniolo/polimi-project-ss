# Project Documentation

This directory contains the complete technical reference materials of the F401RE Nucleo board and the PMDB16 expansion board, which are used in the Sensor Systems course. The documentation includes datasheets, user manuals, and application notes for the hardware components, as well as software libraries and development tools. The directory is organized by hardware subsystem and software development tools to facilitate easier debugging and system maintenance.

## Documentation Structure

### Hardware Platforms

- [`expansion-board/`](<hardware-platforms/expansion-board/>): Contains the schematics of the PMDB16 expansion board, as well as the datasheets and user manuals for the components used in the board.
    - [`cots/`](<hardware-platforms/expansion-board/cots/>): Contains the datasheets and user manuals for the commercial off-the-shelf (COTS) components used in the PMDB16 expansion board.
        - [`74HC-HCT595.pdf`](<hardware-platforms/expansion-board/cots/74HC-HCT595.pdf>): The datasheet for the 74HC-HCT595 shift register used in the PMDB16 expansion board.
        - [`LDR.pdf`](<hardware-platforms/expansion-board/cots/LDR.pdf>): The datasheet for the Light Dependent Resistor (LDR) used in the PMDB16 expansion board.
        - [`LIS2DE.pdf`](<hardware-platforms/expansion-board/cots/LIS2DE.pdf>): The datasheet for the LIS2DE accelerometer used in the PMDB16 expansion board.
        - [`LIS2DW.PDF`](<hardware-platforms/expansion-board/cots/LIS2DW.PDF>): The datasheet for the LIS2DW accelerometer used in the PMDB16 expansion board.
        - [`LM75A.pdf`](<hardware-platforms/expansion-board/cots/LM75A.pdf>): The datasheet for the LM75A temperature sensor used in the PMDB16 expansion board.
        - [`LM75B.pdf`](<hardware-platforms/expansion-board/cots/LM75B.pdf>): The datasheet for the LM75B temperature sensor used in the PMDB16 expansion board.
        - [`ST7066.pdf`](<hardware-platforms/expansion-board/cots/ST7066.pdf>): The datasheet for the ST7066 LCD controller used in the PMDB16 expansion board.
        - [`WH1602C.pdf`](<hardware-platforms/expansion-board/cots/WH1602C.pdf>): The datasheet for the WH1602C LCD module used in the PMDB16 expansion board.
    - [`PMDB16 - Schematics.pdf`](<hardware-platforms/expansion-board/PMDB16 - Schematics.pdf>): The schematics of the PMDB16 expansion board.
- [`nucleo-board/`](<hardware-platforms/nucleo-board/>): Documentation specific to the ST NUCLEO-F401RE development platform.
    - [`mcu-reference/`](<hardware-platforms/nucleo-board/mcu-reference/>): Contains the datasheet and reference manual for the STM32F401RE microcontroller used in the NUCLEO-F401RE development board.
        - [`STM32F401RE - Datasheet.pdf`](<hardware-platforms/nucleo-board/mcu-reference/STM32F401RE - Datasheet.pdf>): The datasheet for the STM32F401RE microcontroller.
        - [`STM32F401RE - Reference Manual.pdf`](<hardware-platforms/nucleo-board/mcu-reference/STM32F401RE - Reference Manual.pdf>): The reference manual for the STM32F401RE microcontroller.
    - [`NUCLEO-F401RE - Datasheet.pdf`](<hardware-platforms/nucleo-board/NUCLEO-F401RE - Datasheet.pdf>): The datasheet for the NUCLEO-F401RE development board.
    - [`NUCLEO-F401RE - Schematic.pdf`](<hardware-platforms/nucleo-board/NUCLEO-F401RE - Schematic.pdf>): The schematic of the NUCLEO-F401RE development board.
    - [`NUCLEO-F401RE - User manual.pdf`](<hardware-platforms/nucleo-board/NUCLEO-F401RE - User manual.pdf>): The user manual for the NUCLEO-F401RE development board.

### Software Platforms

- [`software-platforms/`](<software-platforms/>): Contains the documentation of the STM32CubeIDE development environment.
    - [`HAL Library - User Guide.pdf`](<software-platforms/HAL Library - User Guide.pdf>): The user guide for the STM32 HAL (Hardware Abstraction Layer) library.
    - [`STM32cubeIDE - User Guide.pdf`](<software-platforms/STM32cubeIDE - User Guide.pdf>): The user guide for the STM32CubeIDE development environment.

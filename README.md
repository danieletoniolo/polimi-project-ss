# Sensor Systems - A.Y. 2025/2026

This repository contains the materials for the Sensor Systems course, held at the Politecnico di Milano in the academic year 2025/2026. The repository include all the projects and homework assignments, as well as the slides and other materials used during the lectures.

## Repository Structure

The repository has two main directories:
- [`laboratories/`](laboratories/): Contains the materials for the laboratory sessions, including the project descriptions, code templates, and solutions.
- [`homeworks/`](homeworks/): Contains the materials for the homework assignments, including the descriptions, code templates, and solutions.

## Projects and Homework Overview

Available projects and homework assignments include:

- [`LAB01-1a/`](laboratories/laboratory-01/LAB01-1a/): Push Button (Polling)
- [`LAB01-1b/`](laboratories/laboratory-01/LAB01-1b/): Push Button (Interrupt)
- [`HW02-1a/`](homeworks/homework-02/HW02-1a/): Microphone (Interrupt)
- [`LAB02-1c/`](laboratories/laboratory-02/LAB02-1c/): LED Blinking (PWM)
- [`LAB02-2a/`](laboratories/laboratory-02/LAB02-2a/): Single Tone (PWM)
- [`LAB02-2b/`](laboratories/laboratory-02/LAB02-2b/): Song Playback (PWM)
- [`LAB02-2c/`](laboratories/laboratory-02/LAB02-2c/): Microphone Triggered Song Playback (PWM)
- [`HW03-2a/`](homeworks/homework-03/HW03-2a/): Microphone Triggered Non-Blocking Song Playback (PWM)
- [`LAB03-1a/`](laboratories/laboratory-03/LAB03-1a/): UART Communication
- [`LAB03-1b/`](laboratories/laboratory-03/LAB03-1b/): UART Communication (DMA)
- [`HW04-2a/`](homeworks/homework-04/HW04-2a/): LCD Scrolling Display
- [`LAB04-1a/`](laboratories/laboratory-04/LAB04-1a/): ADC single acquisition polling to UART
- [`LAB04-2a/`](laboratories/laboratory-04/LAB04-2a/): ADC single acquisition interrupt to UART
- [`LAB04-2b/`](laboratories/laboratory-04/LAB04-2b/): ADC single acquisition triggered by TIM to UART
- [`LAB04-2c/`](laboratories/laboratory-04/LAB04-2c/): ADC single acquisition triggered by TIM to LCD
- [`LAB04-3a/`](laboratories/laboratory-04/LAB04-3a/): ADC scan using DMA
- [`LAB04-3b/`](laboratories/laboratory-04/LAB04-3b/): ADC scan using DMA with LUX calculation
- [`HW05-1a/`](homeworks/homework-05/HW05-1a/): UART to LCD
- [`LAB05-1a/`](laboratories/laboratory-05/LAB05-1a/): Temperature Sensor (MSByte Only)
- [`LAB05-1b/`](laboratories/laboratory-05/LAB05-1b/): Temperature Sensor (MSByte and LSByte)

## Usage
The projects had been designed to be implemented using the STM32CubeIDE, which is an integrated development environment for STM32 microcontrollers. Each project includes a description of the objectives, the steps to follow, and the code templates to use.

To use a project you can use the import feature of STM32CubeIDE to import the project into your workspace.
(It is recommended to import the project as a copy, to avoid modifying the original template or any import issues related to the original project location.)
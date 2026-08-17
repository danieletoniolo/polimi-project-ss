# Sensor Systems - A.Y. 2025/2026

This repository contains the materials for the Sensor Systems course, held at the Politecnico di Milano in the academic year 2025/2026. The repository includes all the projects and homework assignments, as well as the documentation and the other materials used during the lectures.

## Repository Structure

The repository has three main directories:

- [`docs/`](docs/): Contains the software and hardware platforms documentation, including the datasheets, user manuals, and reference manuals for the STM32 microcontrollers and the various sensors used in the course.
- [`homeworks/`](homeworks/): Contains the materials for the homework assignments, including the assignment texts and the projects developed to solve them.
- [`laboratories/`](laboratories/): Contains the materials for the laboratory sessions, including the assignment texts and the projects developed during the sessions.

Every project directory contains a `README.md` with the objective of the project and a step by step guide to reproduce it from scratch.

## Laboratory Projects

| Project | Description |
|---------|-------------|
| [`LAB01-1a`](laboratories/laboratory-01/LAB01-1a/) | Push Button (Polling) |
| [`LAB01-1b`](laboratories/laboratory-01/LAB01-1b/) | Push Button (Interrupt) |
| [`LAB02-1c`](laboratories/laboratory-02/LAB02-1c/) | Green LED Blinking (PWM) |
| [`LAB02-2a`](laboratories/laboratory-02/LAB02-2a/) | Single Tone (PWM) |
| [`LAB02-2b`](laboratories/laboratory-02/LAB02-2b/) | Song Playback (PWM) |
| [`LAB02-2c`](laboratories/laboratory-02/LAB02-2c/) | Microphone Triggered Song Playback (PWM) |
| [`LAB03-1a`](laboratories/laboratory-03/LAB03-1a/) | UART Communication |
| [`LAB03-1b`](laboratories/laboratory-03/LAB03-1b/) | UART Communication (DMA) |
| [`LAB04-1a`](laboratories/laboratory-04/LAB04-1a/) | ADC Single Acquisition (Polling) to UART |
| [`LAB04-2a`](laboratories/laboratory-04/LAB04-2a/) | ADC Single Acquisition (Interrupt) to UART |
| [`LAB04-2b`](laboratories/laboratory-04/LAB04-2b/) | ADC Single Acquisition (TIM Triggered) to UART |
| [`LAB04-2c`](laboratories/laboratory-04/LAB04-2c/) | ADC Single Acquisition (TIM Triggered) to LCD |
| [`LAB04-3a`](laboratories/laboratory-04/LAB04-3a/) | ADC Scan using DMA |
| [`LAB04-3b`](laboratories/laboratory-04/LAB04-3b/) | Light Dependent Resistor (LDR) with LUX Calculation |
| [`LAB05-1a`](laboratories/laboratory-05/LAB05-1a/) | Temperature Sensor (MSByte Only) |
| [`LAB05-1b`](laboratories/laboratory-05/LAB05-1b/) | Temperature Sensor (MSByte and LSByte) |
| [`LAB06-1a`](laboratories/laboratory-06/LAB06-1a/) | Accelerometer (Basic Polling) |
| [`LAB06-1b`](laboratories/laboratory-06/LAB06-1b/) | Accelerometer (TIM Interrupt and UART DMA) |
| [`LAB06-1c`](laboratories/laboratory-06/LAB06-1c/) | Accelerometer (TIM Interrupt, UART DMA and I2C DMA) |
| [`LAB07-1a`](laboratories/laboratory-07/LAB07-1a/) | LED Matrix |
| [`LAB08-1a`](laboratories/laboratory-08/LAB08-1a/) | Keyboard |
| [`LAB09-1a`](laboratories/laboratory-09/LAB09-1a/) | Encoder Readout |
| [`LAB10-1a`](laboratories/laboratory-10/LAB10-1a/) | IR Communication (TX Only) |
| [`LAB10-2a`](laboratories/laboratory-10/LAB10-2a/) | IR Communication (RX Only) |
| [`LAB10-3a`](laboratories/laboratory-10/LAB10-3a/) | IR Communication (TX and RX) |

## Homework Projects

Four homework assignments are solved by a dedicated project:

| Project | Description |
|---------|-------------|
| [`homework-01`](homeworks/homework-01/) | Documentation Q&A on the NUCLEO board and the STM32F401RE datasheet (no code) |
| [`HW02-1a`](homeworks/homework-02/HW02-1a/) | Microphone (Interrupt) |
| [`HW03-2a`](homeworks/homework-03/HW03-2a/) | Microphone Triggered Non-Blocking Song Playback (PWM) |
| [`HW04-2a`](homeworks/homework-04/HW04-2a/) | LCD Scrolling Display |
| [`HW05-1a`](homeworks/homework-05/HW05-1a/) | UART to LCD |

All the remaining homework exercises ask for a project that was already developed during a laboratory session, so the corresponding directory is a symbolic link to that laboratory project:

| Homework | Links to |
|----------|----------|
| [`HW02-1b`](homeworks/homework-02/HW02-1b/) | [`LAB02-1c`](laboratories/laboratory-02/LAB02-1c/) |
| [`HW03-1a`](homeworks/homework-03/HW03-1a/) | [`LAB02-2c`](laboratories/laboratory-02/LAB02-2c/) |
| [`HW04-1a`](homeworks/homework-04/HW04-1a/) | [`LAB03-1b`](laboratories/laboratory-03/LAB03-1b/) |
| [`HW05-2b`](homeworks/homework-05/HW05-2b/) | [`LAB04-2b`](laboratories/laboratory-04/LAB04-2b/) |
| [`HW05-2c`](homeworks/homework-05/HW05-2c/) | [`LAB04-2c`](laboratories/laboratory-04/LAB04-2c/) |
| [`HW06-3a`](homeworks/homework-06/HW06-3a/) | [`LAB04-3a`](laboratories/laboratory-04/LAB04-3a/) |
| [`HW06-3b`](homeworks/homework-06/HW06-3b/) | [`LAB04-3b`](laboratories/laboratory-04/LAB04-3b/) |
| [`HW07-1b`](homeworks/homework-07/HW07-1b/) | [`LAB05-1b`](laboratories/laboratory-05/LAB05-1b/) |
| [`HW08-1b`](homeworks/homework-08/HW08-1b/) | [`LAB06-1b`](laboratories/laboratory-06/LAB06-1b/) |
| [`HW08-1c`](homeworks/homework-08/HW08-1c/) | [`LAB06-1c`](laboratories/laboratory-06/LAB06-1c/) |
| [`HW09-1a`](homeworks/homework-09/HW09-1a/) | [`LAB07-1a`](laboratories/laboratory-07/LAB07-1a/) |
| [`HW10-1a`](homeworks/homework-10/HW10-1a/) | [`LAB08-1a`](laboratories/laboratory-08/LAB08-1a/) |
| [`HW10-2a`](homeworks/homework-10/HW10-2a/) | [`LAB09-1a`](laboratories/laboratory-09/LAB09-1a/) |
| [`HW11-1a`](homeworks/homework-11/HW11-1a/) | [`LAB10-1a`](laboratories/laboratory-10/LAB10-1a/) |
| [`HW11-1b`](homeworks/homework-11/HW11-1b/) | [`LAB10-2a`](laboratories/laboratory-10/LAB10-2a/) |
| [`HW11-2a`](homeworks/homework-11/HW11-2a/) | [`LAB10-3a`](laboratories/laboratory-10/LAB10-3a/) |

## Usage

The projects have been designed to be implemented using the STM32CubeIDE, which is an integrated development environment for STM32 microcontrollers. Each project includes a description of the objective, the steps to follow, and the code to write.

To use a project you can use the import feature of STM32CubeIDE to import the project into your workspace.
(It is recommended to import the project as a copy, to avoid modifying the original project or any import issue related to the original project location.)

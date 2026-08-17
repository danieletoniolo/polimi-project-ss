# LAB03-1b: UART Communication (DMA)

## Description
The objective of this project is to send information from the microcontroller to the computer via the USART communication using DMA. The message "Hello World!" is sent from the microcontroller to the computer every second, using a timer interrupt as the time base.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the pins for USART communication. The PA2 pin is used for USART2 TX and the PA3 pin is used for USART2 RX.
3. In the "Connectivity" tab of the IOC file, select USART2 and set the Mode to "Asynchronous". Configure the Baud Rate to 115200 Bits/s, Word Length to 8 Bits, Parity to None, and Stop Bits to 1.
4. Still in the USART2 configuration, enable the "DMA Settings" and configure the DMA for transmission (TX) as follows:
    - DMA Request: USART2_TX
    - Direction: Memory to Peripheral
    - Mode: Normal
    - Priority: High
5. In the "NVIC Settings" tab, enable the USART2 global interrupt.
6. Configure a timer (TIM2) to generate an interrupt every second. Set the prescaler to 8399 and the period to 9999 to achieve a 1-second interval (assuming an 84 MHz clock) and enable the TIM2 global interrupt in the NVIC settings.
7. Generate the code and open the main.c file.
8. Define the variable to hold the message to be sent and the length of the message:

    ```c
    // Transmission buffer
    char tx_buffer[50];

    // Length of transmission buffer
    int length;
    ```
9. In the main function, before the infinite loop, start the timer in interrupt mode:

    ```c
    // Start the timer in interrupt mode
    HAL_TIM_Base_Start_IT(&htim2);
    ```
10. At the very end of the main.c file, implement the timer interrupt callback function to send the message every second:

    ```c
    // This function is automatically called when a Timer Period elapses
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        if (htim->Instance == TIM2) {

            // Format the message with a newline character
            length = snprintf(tx_buffer, sizeof(tx_buffer), "Hello World!\n");

            // Send the message via UART2 using DMA
            HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, length);
        }
    }
    ```

**Note**:
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
- *HAL_UART_Transmit_DMA() is non-blocking: it only starts the transfer and returns immediately, so the CPU is free while the DMA moves the buffer to the peripheral. The USART2 global interrupt must be enabled for the DMA transfer-complete handling to work.*

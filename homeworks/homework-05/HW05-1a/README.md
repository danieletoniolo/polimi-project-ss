# HW05-1a: UART to LCD

## Description
The objective of this project is to receive a string of variable length from a PC via UART and display it on the LCD connected to the Nucleo board. The reception is performed in interrupt mode with idle line detection, so that strings shorter than the buffer are printed as soon as they are complete.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the following pins as GPIO Output for the LCD: PA4 (LCD_BL_ON), PB1 (LCD_E), PB2 (LCD_RS), PB12 (LCD_D4), PB13 (LCD_D5), PB14 (LCD_D6), and PB15 (LCD_D7).
3. In the IOC file, configure the pins for USART communication. The PA2 pin is used for USART2 TX and the PA3 pin is used for USART2 RX.
4. In the "Connectivity" tab of the IOC file, select USART2 and set the Mode to "Asynchronous". Configure the Baud Rate to 115200 Bits/s, Word Length to 8 Bits, Parity to None, and Stop Bits to 1.
5. In the "NVIC Settings" tab of the USART2 configuration, enable the USART2 global interrupt. This is required for the HAL_UARTEx_ReceiveToIdle_IT function to work.
6. Generate the code and open the main.c file.
7. Copy the LCD library provided in the course materials into the project: PMDB16_LCD.c in Core/Src and PMDB16_LCD.h in Core/Inc.
8. Define the buffer size and the global variables required to handle the UART reception:

    ```c
    #define RX_BUFFER_SIZE 16

    // UART reception buffer and index
    char rx_buffer[RX_BUFFER_SIZE + 1];
    volatile uint8_t rx_index = 0;
    ```
9. In the main function, before the infinite loop, initialize the LCD and start the UART reception in interrupt mode:

    ```c
    // Initialize the LCD
    lcd_initialize();

    // Start UART in interrupt mode
    HAL_UARTEx_ReceiveToIdle_IT(&huart2, (uint8_t*)rx_buffer, RX_BUFFER_SIZE);
    ```
10. At the very end of the main.c file, implement the UART receive event callback function to handle the received data. This function is called when the idle line is detected or when the buffer is full:

    ```c
    // This callback is automatically called when the UART goes to idle or hits the buffer limit
    void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
    {
        if (huart->Instance == USART2) {

            // Update global tracking index with the new chunk of data
            rx_index += Size;

            // Check if we hit Enter or if the buffer is completely full
            if (rx_buffer[rx_index - 1] == '\n' || rx_buffer[rx_index - 1] == '\r' || rx_index >= RX_BUFFER_SIZE) {

                if (rx_buffer[rx_index - 1] == '\n' || rx_buffer[rx_index - 1] == '\r') {

                    // Override the enter with the null terminator
                    rx_buffer[rx_index - 1] = '\0';

                    // Check if there is a '\r' before the '\n'
                    if (rx_index > 1 && rx_buffer[rx_index - 2] == '\r') {
                        rx_buffer[rx_index - 2] = '\0';
                    }

                } else {

                    // Cap the buffer with the null terminator
                    rx_buffer[rx_index] = '\0';
                }

                // Print the new string to LCD
                lcd_println(rx_buffer, 0);

                // Reset the index
                rx_index = 0;

                // Restart the interrupt for the new string
                HAL_UARTEx_ReceiveToIdle_IT(&huart2, (uint8_t *)rx_buffer, RX_BUFFER_SIZE);

            } else {

                // Restart the interrupt with offset pointer
                HAL_UARTEx_ReceiveToIdle_IT(&huart2, (uint8_t *)&rx_buffer[rx_index], RX_BUFFER_SIZE - rx_index);

            }
        }
    }
    ```

**Note**:
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
- *The UART communication implemented can support LF, CR, or CRLF as the end of the string. The code will handle all three cases and print the string to the LCD without the end-of-line characters.*
- *The buffer is declared with RX_BUFFER_SIZE + 1 bytes: the extra byte holds the null terminator when a full 16-character string is received and there is no end-of-line character to overwrite.*
- *If the string is not complete yet, the reception is restarted from the current index with the remaining space, so that a string split into several chunks by the sender is still reassembled correctly.*

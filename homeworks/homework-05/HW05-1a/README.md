# Project 1a: UART to LCD

## Description
Objective of this project is to receive a string of variable length from a PC via UART and display it on the LCD connected to the Nucleo board.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the following pins as GPIO Output for the LCD: PA4 (LCD_BL_ON), PB1 (LCD_E), PB2 (LCD_RS), PB12 (LCD_D4), PB13 (LCD_D5), PB14 (LCD_D6), and PB15 (LCD_D7).
3. Configure the pin for USART communication. The PA2 pin is used for USART2 TX and the PA3 pin is used for USART2 RX.
4. In the "Connectivity" tab, select USART2. Set the Mode to "Asynchronous". Configure the Baud Rate to 115200 Bits/s, Word Length to 8 Bits, Parity to None, and Stop Bits to 1.
5. In the "NVIC Settings" of the USART2 configuration, enable the "USART2 global interrupt". This is required for the HAL_UARTEx_ReceiveToIdle_IT function to work.
6. Generate the code and open the main.c file.
7. Put in the src and inc files the LCD library provided in the course materials (PMDB16_LCD.c and PMDB16_LCD.h).
8. Define the buffer size and the global variables required to handle the UART reception:
    ```c
    #define RX_BUFFER_SIZE 16

    // UART transmission buffer and index
    char rx_buffer[RX_BUFFER_SIZE + 1];
    volatile uint8_t rx_index = 0;
    ```
9. In the main function, initialize the LCD and UART peripherals:
    ```c
    // Initialize the LCD
    lcd_initialize();

    // Start UART in interrupt mode
    HAL_UARTEx_ReceiveToIdle_IT(&huart2, (uint8_t*)rx_buffer, 16);
    ```
10. Implement the UART receive complete callback function to handle the received data. This function will be called when the UART reception is complete or when the idle line is detected:
    ```c
    // This callback is automatically called when the UART goes to idle or hits the buffer limit
    void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
    {
        if (huart->Instance == USART2) {

            // Update global tracking index with the new chuck of data
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
**Note**
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
- *The UART communication implemented can support LF, CR, or CRLF as the end of the string. The code will handle all three cases and print the string to the LCD without the end-of-line characters.*
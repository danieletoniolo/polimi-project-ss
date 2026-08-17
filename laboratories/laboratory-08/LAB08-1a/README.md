# LAB08-1a: Keyboard

## Description
The objective of this project is to read a 4x4 matrix keyboard by scanning one column at a time with a timer interrupt, debouncing the keys in software, and sending the character corresponding to the pressed key to a remote terminal via UART.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the 8 GPIOs required to scan the keyboard. Set PC8, PC9, PC10, and PC11 as GPIO_Output for the columns. Set PC12, PC13, PC2, and PC3 as GPIO_Input for the rows.
3. In the IOC file, configure the pins for USART communication. The PA2 pin is used for USART2 TX and the PA3 pin is used for USART2 RX.
4. In the "Connectivity" tab of the IOC file, select USART2 and set the Mode to "Asynchronous". Configure the Baud Rate to 115200 Bits/s, Word Length to 8 Bits, Parity to None, and Stop Bits to 1.
5. Still in the USART2 configuration, enable the "DMA Settings" and configure the DMA for transmission (TX) as follows:
    - DMA Request: USART2_TX
    - Direction: Memory to Peripheral
    - Mode: Normal
    - Priority: Low
6. In the "NVIC Settings" tab, enable the USART2 global interrupt.
7. Configure a timer (TIM2) to generate an interrupt every 4 ms for the column scanning. Set the prescaler to 839 and the period to 399 (assuming an 84 MHz clock) and enable the TIM2 global interrupt in the NVIC settings.
8. Generate the code and open the main.c file.
9. Define the pin arrays, the debounce matrices and the look-up table (LUT) for the characters:

    ```c
    // Pin definitions for columns (Outputs) and rows (Inputs)
    const uint16_t column_pins[4] = { GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11 };
    const uint16_t row_pins[4] = { GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_2, GPIO_PIN_3 };

    // State tracking arrays for debouncing
    uint8_t stable[4][4] = {0};
    uint8_t counter[4][4] = {0};

    // Index to iterate the current active column and row
    uint8_t col_index;
    uint8_t row_index;

    // Look-up table to map row/column coordinates to actual characters
    const uint8_t lut[4][4] = {
            {'0', '1', '2', '3'},
            {'4', '5', '6', '7'},
            {'8', '9', 'A', 'B'},
            {'C', 'D', 'E', 'F'}
    };
    ```
10. In the main function, before the infinite loop, start the polling timer in interrupt mode:

    ```c
    // Start the polling timer in interrupt mode
    HAL_TIM_Base_Start_IT(&htim2);
    ```
11. At the very end of the main.c file, implement the timer callback to acquire the status of the 4 rows of the active column and then enable the next column:

    ```c
    // This function is automatically called when a Timer Period elapses
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

        if(htim->Instance == TIM2){

            // Read the row for the current active column
            for (row_index = 0; row_index < 4; ++row_index) {

                // Check the status of the key
                uint8_t pressed = (HAL_GPIO_ReadPin(GPIOC, row_pins[row_index]) == GPIO_PIN_RESET);

                // Debounce logic
                if (pressed == stable[row_index][col_index]) {

                    counter[row_index][col_index] = 0;

                } else {

                    counter[row_index][col_index]++;

                    // Validate input after it has been stable for 4 consecutive polling cycles
                    if (counter[row_index][col_index] > 4) {

                        stable[row_index][col_index] = pressed;
                        counter[row_index][col_index] = 0;

                        // Transmit the character via UART DMA
                        if (pressed) HAL_UART_Transmit_DMA(&huart2, (uint8_t *)&lut[row_index][col_index], 1);
                    }
                }
            }

            // Reset the current active column
            HAL_GPIO_WritePin(GPIOC, column_pins[col_index], GPIO_PIN_RESET);

            // Move to the next column index
            col_index = (col_index + 1) % 4;

            // Set the new active column
            HAL_GPIO_WritePin(GPIOC, column_pins[col_index], GPIO_PIN_SET);
        }
    }
    ```

**Note**:
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
- *The open collector transistors on the column line pull it to ground or leave it floating. Therefore, writing a 1 (GPIO_PIN_SET) turns the transistor ON, grounding the column to make it active. Writing a 0 (GPIO_PIN_RESET) turns the transistor OFF, making the column inactive.*
- *The rows have pull-up resistors on the row lines. If a row is high, the key is NOT pressed; if it is low, the key IS pressed.*
- *A key press is validated only after the same state has been read for more than 4 consecutive scans of its column. Since a full scan of the 4 columns takes 16 ms, the resulting debounce time is around 64 ms.*

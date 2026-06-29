# Project 1a: IR communication (TX only)

## Description
The objective of this project is to scan a 4x4 matrix keyboard using a polling timer and transmit the pressed character over an Infrared (IR) link.

## Steps
1. Create a new project in STM32CubeIDE for your Nucleo board.
2. In the IOC file, configure the 8 GPIOs required to scan the keyboard. Set PC8, PC9, PC10, and PC11 as GPIO_Output for the columns. Set PC12, PC13, PC2, and PC3 as GPIO_Input for the rows.
3. Configure TIM2 to generate the 38 kHz PWM carrier wave for the IR transmitter. Set the Clock Source to Internal, Channel 3 to PWM Generation, Prescaler to 2, and Counter Period to 736. Set the Pulse to 368 for a 50% duty cycle. (Note: This automatically assigns the PWM output to PB10).
4. Configure TIM3 to generate an interrupt every 1/2400 seconds to drive the UART bit-banging transmission. Set the prescaler to 0 and the period to 34999. Enable the TIM3 global interrupt.
5. Configure TIM4 to generate an interrupt every 4 ms for keyboard polling. Set the prescaler to 8399 and the period to 39. Enable the TIM4 global interrupt.
6. In the "NVIC Settings" tab, configure the interrupt priorities. Set the TIM3 global interrupt to Priority 0 (highest) and the TIM4 global interrupt to Priority 1 (lower).
7. Generate the code and open the main.c file.
8. Define the variables for the keyboard matrix, debouncing, and IR transmission flags:
    ```c
    /* Keyboard variables --------------------------------------------------------*/

    // Row and Column pin definitions for the 4x4 keypad
    const uint16_t column_pins[4] = { GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11 };
    const uint16_t row_pins[4] = { GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_2, GPIO_PIN_3 };

    // State tracking arrays for debouncing
    uint8_t stable[4][4] = {0};
    uint8_t counter[4][4] = {0};

    uint8_t col_index = 0;
    uint8_t row_index = 0;

    const uint8_t lut[4][4] = {
            {'0', '1', '2', '3'},
            {'4', '5', '6', '7'},
            {'8', '9', 'A', 'B'},
            {'C', 'D', 'E', 'F'}
    };

    /* IR Transmission variables -------------------------------------------------*/

    volatile uint8_t IR_busy_flag = 0;
    volatile uint8_t IR_tx_byte;
    volatile uint8_t IR_tx_bit_index = 0;
    ```
9. Implement the IR transmission functions to handle the bit-banging:
    ```c
    // This function initiates the transmission of a byte via the IR transmitter.
    void IR_Transmit_Byte(uint8_t byte)
    {
        // Check if the IR transmitter is currently busy
        if (IR_busy_flag == 1) return;

        // Reset the bit index for the new transmission
        IR_busy_flag = 1;
        IR_tx_byte = byte;

        // Start the timer to begin the transmission process
        HAL_TIM_Base_Start_IT(&htim3);
    }

    // This function handles the transmission of bits for the IR transmitter.
    void IR_transmission_routine()
    {
        switch (IR_tx_bit_index) {

            case 0:

            // Start the PWM signal for the start bit
            HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
            break;

            case 9:

            // Stop the PWM signal for the stop bit
            HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
            break;

            case 10:

            HAL_TIM_Base_Stop(&htim3);
            IR_busy_flag = 0;
            break;

            default:

            if ((IR_tx_byte >> (IR_tx_bit_index - 1)) & 0x01) {

                // Send '1' by stopping the PWM signal
                HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);

            } else {

                // Send '0' by starting the PWM signal
                HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
            }
        }

        // Move to the next bit in the transmission sequence
        IR_tx_bit_index = (IR_tx_bit_index + 1) % 11;
    }
    ```
9. Implement the keyboard read routine:
    ```c
    // This function reads the state of the keyboard and handles debouncing.
    void keyboard_read_routine()
    {
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

                    // Transmit the character via IR
                    if (pressed) IR_Transmit_Byte(lut[row_index][col_index]);
                }
            }
        }

        // Move to the next column
        HAL_GPIO_WritePin(GPIOC, column_pins[col_index], GPIO_PIN_RESET);
        col_index = (col_index + 1) % 4;
        HAL_GPIO_WritePin(GPIOC, column_pins[col_index], GPIO_PIN_SET);
    }
    ```
10. In the main function, start the UI polling timer (TIM4) before the while loop:
    ```c
    // Start the polling timer
    HAL_TIM_Base_Start_IT(&htim4);
    ```
11. Implement the timer period elapsed callback:
    ```c
    // This callback is triggered when a timer period elapses.
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        // IR Transmission strictly timed at 2400Hz
        if (htim->Instance == TIM3) {
            IR_transmission_routine();
        }

        // Keyboard Polling
        if (htim->Instance == TIM4) {
            keyboard_read_routine();
        }
    }
    ```
**Note**: *The IR transmission timer (TIM3) must have a higher preemption priority than the keyboard timer (TIM4) to prevent timing jitter during UART bit-banging.*
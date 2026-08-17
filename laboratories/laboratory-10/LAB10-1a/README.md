# LAB10-1a: IR Communication (TX Only)

## Description
The objective of this project is to scan a 4x4 matrix keyboard using a polling timer and to transmit the character of the pressed key over an Infrared (IR) link. The UART frame is generated in software (bit-banging) and modulated on a 38 kHz PWM carrier wave.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the 8 GPIOs required to scan the keyboard. Set PC8, PC9, PC10, and PC11 as GPIO_Output for the columns. Set PC12, PC13, PC2, and PC3 as GPIO_Input for the rows.
3. In the "Timers" tab of the IOC file, select TIM2 and configure it to generate the 38 kHz PWM carrier wave for the IR transmitter. Set the Clock Source to "Internal Clock" and Channel 3 to "PWM Generation CH3". Then, in the "Parameter Settings" tab, set the Prescaler to 2, the Counter Period to 736 and the Pulse to 368 for a 50% duty cycle (assuming an 84 MHz clock).
4. Configure TIM3 to generate an interrupt every 1/2400 seconds, which is the bit time of the software UART transmission. Set the prescaler to 0 and the period to 34999 (assuming an 84 MHz clock) and enable the TIM3 global interrupt in the NVIC settings with priority 0.
5. Configure TIM4 to generate an interrupt every 4 ms for the keyboard scanning. Set the prescaler to 8399 and the period to 39 (assuming an 84 MHz clock) and enable the TIM4 global interrupt in the NVIC settings with priority 1, so that it is lower than the one of TIM3.
6. Generate the code and open the main.c file.
7. Define the variables for the keyboard matrix, the debouncing and the IR transmission flags:

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
8. Define the IR transmission functions to start a transmission and to handle the bit-banging of the single bits:

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
9. Define the keyboard read routine, which scans the active column, debounces the keys and starts the IR transmission of the pressed character:

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
10. In the main function, before the infinite loop, start the keyboard polling timer (TIM4) in interrupt mode:

    ```c
    // Start the polling timer
    HAL_TIM_Base_Start_IT(&htim4);
    ```
11. At the very end of the main.c file, implement the timer callback to dispatch the two routines according to the timer that raised the interrupt:

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

**Note**:
- *Selecting TIM2 Channel 3 for the PWM generation automatically assigns the carrier output to the PB10 pin, which is the pin connected to the IR transmitter of the expansion board.*
- *The IR frame is a standard UART frame generated in software: 1 start bit, 8 data bits (LSB first) and 1 stop bit, each lasting exactly one TIM3 period (1/2400 s). The carrier is ON for a logic '0' and OFF for a logic '1', because the IR receiver output is active low.*
- *The IR transmission timer (TIM3) must have a higher preemption priority than the keyboard timer (TIM4). Otherwise the keyboard scan could delay a bit transition, stretching the UART timing and corrupting the data sent over the optical link.*

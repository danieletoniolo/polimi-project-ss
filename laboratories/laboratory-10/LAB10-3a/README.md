# LAB10-3a: IR Communication (TX and RX)

## Description
The objective of this project is to integrate the keyboard scanner and the LED matrix display with an Infrared (IR) UART communication system. A pressed key is read using a polling timer, transmitted over a 38 kHz modulated IR signal via software bit-banging, received asynchronously via hardware UART and DMA, and finally displayed on the SPI-driven LED matrix.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the 8 GPIOs required to scan the keyboard. Set PC8, PC9, PC10, and PC11 as GPIO_Output for the columns. Set PC12, PC13, PC2, and PC3 as GPIO_Input for the rows.
3. In the IOC file, configure the pins used to communicate with the LED matrix. Set PA5 for the serial clock (SPI1_SCK), PA7 for the serial data output (SPI1_MOSI) and PB6 for the latch (GPIO_Output).
4. In the "Connectivity" tab of the IOC file, select SPI1 and enable it in "Transmit Only Master" mode. Then, in the "Parameter Settings" tab, configure it as follows:
    - Frame Format: Motorola
    - Data Size: 8 Bits
    - First Bit: MSB First
    - Prescaler: 4
    - Clock Polarity (CPOL): Low
    - Clock Phase (CPHA): 1 Edge
5. Still in the SPI1 configuration, enable the "DMA Settings" and configure the DMA for transmission (TX) as follows:
    - DMA Request: SPI1_TX
    - Direction: Memory to Peripheral
    - Mode: Normal
    - Priority: Low
6. In the "Connectivity" tab of the IOC file, select USART1 and set the Mode to "Asynchronous". Configure the Baud Rate strictly to 2400 Bits/s, Word Length to 8 Bits, Parity to None, and Stop Bits to 1.
7. Still in the USART1 configuration, enable the "DMA Settings" and configure the DMA for reception (RX) as follows:
    - DMA Request: USART1_RX
    - Direction: Peripheral to Memory
    - Mode: Normal
    - Priority: Low
8. In the "NVIC Settings" tab, enable the USART1 global interrupt and ensure that the DMA stream global interrupts used by SPI1_TX and USART1_RX are enabled.
9. In the "Timers" tab of the IOC file, select TIM2 and configure it to generate the 38 kHz PWM carrier wave for the IR transmitter. Set the Clock Source to "Internal Clock" and Channel 3 to "PWM Generation CH3". Then, in the "Parameter Settings" tab, set the Prescaler to 2, the Counter Period to 736 and the Pulse to 368 for a 50% duty cycle (assuming an 84 MHz clock).
10. Configure TIM3 to generate an interrupt every 1/2400 seconds, which is the bit time of the software UART transmission. Set the prescaler to 0 and the period to 34999 (assuming an 84 MHz clock) and enable the TIM3 global interrupt in the NVIC settings with priority 0.
11. Configure TIM4 to generate an interrupt every 4 ms for the keyboard scanning and the LED column multiplexing. Set the prescaler to 8399 and the period to 39 (assuming an 84 MHz clock) and enable the TIM4 global interrupt in the NVIC settings with priority 1, so that it is lower than the one of TIM3.
12. Generate the code and open the main.c file.
13. Define the variables for the keyboard matrix, the debouncing, the LED font data, the SPI buffer and the IR transmission flags:

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

    /* LED matrix variables ------------------------------------------------------*/

    // 5-column pattern for the led matrix letters
    const uint8_t led_matrix_letters[16][5][2] = {
        {{62,  16}, {113, 8}, {73,  4}, {71,  2}, {62,  1}}, // Number 0
        {{33,  16}, {65,  8}, {127, 4}, {1,   2}, {1,   1}}, // Number 1
        {{35,  16}, {69,  8}, {73,  4}, {81,  2}, {33,  1}}, // Number 2
        {{65,  16}, {73,  8}, {73,  4}, {73,  2}, {54,  1}}, // Number 3
        {{120, 16}, {8,   8}, {8,   4}, {127, 2}, {8,   1}}, // Number 4
        {{121, 16}, {73,  8}, {73,  4}, {73,  2}, {70,  1}}, // Number 5
        {{62,  16}, {73,  8}, {73,  4}, {73,  2}, {70,  1}}, // Number 6
        {{64,  16}, {64,  8}, {67,  4}, {76,  2}, {112, 1}}, // Number 7
        {{54,  16}, {73,  8}, {73,  4}, {73,  2}, {54,  1}}, // Number 8
        {{48,  16}, {72,  8}, {72,  4}, {72,  2}, {63,  1}}, // Number 9
        {{31,  16}, {36,  8}, {68,  4}, {36,  2}, {31,  1}}, // Letter A
        {{127, 16}, {73,  8}, {73,  4}, {73,  2}, {54,  1}}, // Letter B
        {{62,  16}, {65,  8}, {65,  4}, {65,  2}, {34,  1}}, // Letter C
        {{127, 16}, {65,  8}, {65,  4}, {65,  2}, {62,  1}}, // Letter D
        {{127, 16}, {73,  8}, {73,  4}, {73,  2}, {65,  1}}, // Letter E
        {{127, 16}, {72,  8}, {72,  4}, {72,  2}, {64,  1}}, // Letter F
    };

    uint8_t led_current_column = 0;
    uint8_t led_current_char_index = 0;

    // SPI buffer
    uint8_t tx_buffer[2];

    /* IR Transmission variables -------------------------------------------------*/

    volatile uint8_t IR_busy_flag = 0;
    volatile uint8_t IR_tx_byte;
    volatile uint8_t IR_tx_bit_index = 0;

    /* IR receive variables ------------------------------------------------------*/

    uint8_t IR_rx_byte;
    ```
14. Define the IR transmission functions to start a transmission and to handle the bit-banging of the single bits:

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
15. Define the keyboard read routine and the LED refresh routine:

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

    // This function refreshes the LED matrix display by sending the current column data via SPI.
    void led_refresh_routine()
    {
        // Load the buffer explicitly safely for DMA
        tx_buffer[0] = led_matrix_letters[led_current_char_index][led_current_column][0];
        tx_buffer[1] = led_matrix_letters[led_current_char_index][led_current_column][1];

        // Pull the RCLK pin LOW
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

        // Start the 2-byte SPI transfer using DMA and the dedicated buffer
        HAL_SPI_Transmit_DMA(&hspi1, tx_buffer, 2);
    }
    ```
16. In the main function, before the infinite loop, start the UART receiver in DMA mode and the UI polling timer (TIM4) in interrupt mode:

    ```c
    // Start listening for incoming IR UART data
    HAL_UART_Receive_DMA(&huart1, &IR_rx_byte, 1);

    // Start the UI polling timer (Keypad and LED multiplexing)
    HAL_TIM_Base_Start_IT(&htim4);
    ```
17. At the very end of the main.c file, implement the SPI transmission complete callback to pull the RCLK pin HIGH (latching the shift registers) and increment the active LED column:

    ```c
    // This callback is triggered when the SPI transmission is complete.
    void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
    {
        if (hspi->Instance == SPI1) {

            // Latch the 16 bits to output the pins
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

            // Increment the column index
            led_current_column = (led_current_column + 1) % 5;
        }
    }
    ```
18. Just after the SPI callback, implement the UART receive complete callback to decode the received character and the UART error callback to restart the reception in case of errors:

    ```c
    // This callback is triggered when the UART reception is complete.
    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
    {
        // Decode ASCII to index for led_matrix_letters
        if (IR_rx_byte >= '0' && IR_rx_byte <= '9') {

            led_current_char_index = IR_rx_byte - '0';

        } else if (IR_rx_byte >= 'A' && IR_rx_byte <= 'F') {

            led_current_char_index = IR_rx_byte - 'A' + 10;
        }

        // Restart reception for the next byte
        HAL_UART_Receive_DMA(&huart1, &IR_rx_byte, 1);
    }

    // This callback is triggered when a UART error occurs.
    void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
    {
        // If the IR receiver gets overwhelmed by noise and crashes, just restart it.
        if (huart->Instance == USART1) {
            HAL_UART_Receive_DMA(&huart1, &IR_rx_byte, 1);
        }
    }
    ```
19. Just after the UART callbacks, implement the timer callback to handle the IR transmission routine and the UI multiplexing:

    ```c
    // This callback is triggered when a timer period elapses.
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        // IR Transmission strictly timed at 2400Hz
        if (htim->Instance == TIM3) {
            IR_transmission_routine();
        }

        // UI Multiplexing and Keyboard Polling
        if (htim->Instance == TIM4) {
            keyboard_read_routine();
            led_refresh_routine();
        }
    }
    ```

**Note**:
- *Selecting TIM2 Channel 3 for the PWM generation automatically assigns the carrier output to the PB10 pin, which is the pin connected to the IR transmitter of the expansion board.*
- *The physical IR receiver hardware automatically filters and strips the 38 kHz carrier wave, allowing the incoming signal to be fed directly into the standard hardware USART1 RX pin via DMA. Transmitter modulation must be handled manually via software and PWM.*
- *The IR frame is a standard UART frame generated in software: 1 start bit, 8 data bits (LSB first) and 1 stop bit, each lasting exactly one TIM3 period (1/2400 s). The carrier is ON for a logic '0' and OFF for a logic '1', because the IR receiver output is active low.*
- *The IR transmission timer (TIM3) must have a higher preemption priority than the UI timer (TIM4). If the UI timer interrupts the transmission bit-banging, the UART timing will stretch, resulting in corrupted data over the optical link.*

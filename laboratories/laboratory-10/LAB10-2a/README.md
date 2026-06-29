# Project 2a: IR communication (RX only)

## Description
The objective of this project is to receive an Infrared (IR) UART transmission and display the received character on an SPI-driven LED matrix.

## Steps
1. Create a new project in STM32CubeIDE for your Nucleo board.
2. Configure the SPI peripheral (SPI1) to communicate with the LED matrix. Set PA5 (SCK), PA7 (MOSI), and PB6 (GPIO_Output for Latch/RCLK). Configure it as Transmit Only Master, 8 Bits, MSB First, CPOL Low, CPHA 1 Edge. Add a DMA request for SPI1_TX (Memory to Peripheral).
3. Configure the USART1 peripheral for IR reception. Set the baud rate strictly to 2400, data bits to 8, no parity, and stop bits to 1. Add a DMA request for USART1_RX (Peripheral to Memory).
4. Configure TIM4 to generate an interrupt every 4 ms for LED column multiplexing. Set the prescaler to 8399 and the period to 39. Enable the TIM4 global interrupt.
5. Generate the code and open the main.c file.
6. Define the variables for the LED font data, SPI buffer, and UART reception:
    ```c
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

    /* IR receive variables ------------------------------------------------------*/

    uint8_t IR_rx_byte;
    ```
7. Implement the LED refresh routine:
    ```c
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
8. In the main function, start the UART receiver DMA and the multiplexing timer (TIM4):
    ```c
    // Start listening for incoming IR UART data
    HAL_UART_Receive_DMA(&huart1, &IR_rx_byte, 1);

    // Start the polling timer
    HAL_TIM_Base_Start_IT(&htim4);
    ```
9. Implement the callbacks for SPI Tx completion, UART Rx completion, UART errors, and Timer elapsed:
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

    // This callback is triggered when a timer period elapses.
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        // LED Multiplexing
        if (htim->Instance == TIM4) {
            led_refresh_routine();
        }
    }
    ```
**Note**: *The physical IR receiver hardware automatically filters and strips the 38 kHz carrier wave, allowing the incoming signal to be fed directly into the standard hardware USART1 RX pin via DMA. Transmitter modulation must be handled manually via software and PWM.*
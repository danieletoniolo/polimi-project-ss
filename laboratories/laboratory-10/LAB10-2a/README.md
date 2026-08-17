# LAB10-2a: IR Communication (RX Only)

## Description
The objective of this project is to receive an Infrared (IR) UART transmission and to display the received character on the LED matrix driven via SPI. The IR receiver strips the 38 kHz carrier by itself, so the incoming signal is fed directly to the hardware USART1 receiver in DMA mode.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the pins used to communicate with the LED matrix. Set PA5 for the serial clock (SPI1_SCK), PA7 for the serial data output (SPI1_MOSI) and PB6 for the latch (GPIO_Output).
3. In the "Connectivity" tab of the IOC file, select SPI1 and enable it in "Transmit Only Master" mode. Then, in the "Parameter Settings" tab, configure it as follows:
    - Frame Format: Motorola
    - Data Size: 8 Bits
    - First Bit: MSB First
    - Prescaler: 4
    - Clock Polarity (CPOL): Low
    - Clock Phase (CPHA): 1 Edge
4. Still in the SPI1 configuration, enable the "DMA Settings" and configure the DMA for transmission (TX) as follows:
    - DMA Request: SPI1_TX
    - Direction: Memory to Peripheral
    - Mode: Normal
    - Priority: Low
5. In the "Connectivity" tab of the IOC file, select USART1 and set the Mode to "Asynchronous". Configure the Baud Rate strictly to 2400 Bits/s, Word Length to 8 Bits, Parity to None, and Stop Bits to 1.
6. Still in the USART1 configuration, enable the "DMA Settings" and configure the DMA for reception (RX) as follows:
    - DMA Request: USART1_RX
    - Direction: Peripheral to Memory
    - Mode: Normal
    - Priority: Low
7. In the "NVIC Settings" tab, enable the USART1 global interrupt and ensure that the DMA stream global interrupts used by SPI1_TX and USART1_RX are enabled.
8. Configure TIM4 to generate an interrupt every 4 ms for the LED column multiplexing. Set the prescaler to 8399 and the period to 39 (assuming an 84 MHz clock) and enable the TIM4 global interrupt in the NVIC settings.
9. Generate the code and open the main.c file.
10. Define the variables for the LED font data, the SPI buffer and the UART reception:

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
11. Define the LED refresh routine, which sends the pattern of the current column to the shift registers via SPI:

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
12. In the main function, before the infinite loop, start the UART receiver in DMA mode and the multiplexing timer (TIM4) in interrupt mode:

    ```c
    // Start listening for incoming IR UART data
    HAL_UART_Receive_DMA(&huart1, &IR_rx_byte, 1);

    // Start the polling timer
    HAL_TIM_Base_Start_IT(&htim4);
    ```
13. At the very end of the main.c file, implement the SPI transmission complete callback to latch the data and update the column index:

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
14. Just after the SPI callback, implement the UART receive complete callback to decode the received character and the UART error callback to restart the reception in case of errors:

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
15. Just after the UART callbacks, implement the timer callback to refresh the LED matrix:

    ```c
    // This callback is triggered when a timer period elapses.
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        // LED Multiplexing
        if (htim->Instance == TIM4) {
            led_refresh_routine();
        }
    }
    ```

**Note**:
- *The physical IR receiver hardware automatically filters and strips the 38 kHz carrier wave, allowing the incoming signal to be fed directly into the standard hardware USART1 RX pin via DMA. Transmitter modulation must be handled manually via software and PWM.*
- *The baud rate of USART1 must be exactly 2400 Bits/s, because it has to match the bit time used by the transmitter for the software bit-banging.*
- *The reception is restarted at the end of every byte because the DMA is configured in Normal mode and not in Circular mode. The error callback restarts it as well, so that a burst of optical noise cannot leave the receiver stuck.*

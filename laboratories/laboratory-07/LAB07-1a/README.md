# LAB07-1a: LED Matrix

## Description
The objective of this project is to display a letter on the LED matrix of the expansion board, transmitting the column patterns via SPI, and to alternate between two letters ('D' and 'T') every second using a second timer interrupt. The matrix is refreshed by multiplexing one column every 4 ms.

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
5. In the "NVIC Settings" tab, ensure that the DMA stream global interrupt used by SPI1_TX is enabled.
6. Configure a timer (TIM2) to generate an interrupt every 4 ms for the column multiplexing. Set the prescaler to 839 and the period to 399 (assuming an 84 MHz clock) and enable the TIM2 global interrupt in the NVIC settings with priority 0.
7. Configure a second timer (TIM3) to generate an interrupt every second for the letter toggling. Set the prescaler to 8399 and the period to 9999 (assuming an 84 MHz clock) and enable the TIM3 global interrupt in the NVIC settings with priority 1, so that it is lower than the one of TIM2.
8. Generate the code and open the main.c file.
9. Define the variables for the font data, the transmission buffer and the control state:

    ```c
    // 5-column pattern for the letter 'D'
    const uint8_t letter_D[5][2] = {
        {127, 16}, {65, 8}, {65, 4}, {65, 2}, {62, 1}
    };

    // 5-column pattern for the letter 'T'
    const uint8_t letter_T[5][2] = {
        {64, 16}, {64, 8}, {127, 4}, {64, 2}, {64, 1}
    };

    // SPI buffer and column index
    uint8_t current_column = 0;
    uint8_t tx_buffer[2];

    // Current shown letter
    const uint8_t (* volatile active_letter)[2];
    uint8_t current_letter = 0;
    ```
10. In the main function, before the infinite loop, initialize the active letter to point to the first letter ('D') and start both timers in interrupt mode:

    ```c
    // Set the initial pointer target before starting the timer
    active_letter = letter_D;

    // Start the multiplexing timer in interrupt mode (4ms)
    HAL_TIM_Base_Start_IT(&htim2);

    // Start the letter-swapping timer in interrupt mode (1000 ms)
    HAL_TIM_Base_Start_IT(&htim3);
    ```
11. At the very end of the main.c file, implement the timer callback to trigger the SPI transmission of the current column and to toggle the displayed letter:

    ```c
    // This function is automatically called when a Timer Period elapses
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        if (htim->Instance == TIM2) {

            // Load the buffer
            tx_buffer[0] = active_letter[current_column][0];
            tx_buffer[1] = active_letter[current_column][1];

            // Pull the RCLK pin LOW so that the data will be latched on the rising edge
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

            // Start the DMA transmission
            HAL_SPI_Transmit_DMA(&hspi1, tx_buffer, 2);

        } else if (htim->Instance == TIM3) {

            // Change letter
            current_letter = !current_letter;
            active_letter = current_letter == 0 ? letter_D : letter_T;
        }
    }
    ```
12. Just after the timer callback, implement the SPI transmission complete callback to latch the data and update the column index:

    ```c
    // This function is automatically called when the SPI transmission is complete
    void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
    {
        if (hspi->Instance == SPI1) {

            // Pull the RCLK pin HIGH to latch the data from the shift registers
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

            // Increment the column index for the next timer interrupt.
            current_column++;
            if (current_column >= 5) current_column = 0;
        }
    }
    ```

**Note**:
- *Because the two shift registers are daisy-chained, a 16-bit payload (2 bytes) is sent per transaction. The first byte sent will ripple through into the furthest shift register, while the second byte will remain in the closest register.*
- *The HAL_SPI_Transmit_DMA function is non-blocking. The RCLK pin (PB6) cannot be pulled HIGH immediately after calling the transmit function, as the bits have not yet physically left the microcontroller. The RCLK pin is properly toggled inside HAL_SPI_TxCpltCallback, which guarantees the hardware has finished shifting.*
- *The two timers are configured with different priorities. The multiplexing timer (TIM2) has a higher priority than the letter toggling timer (TIM3). This ensures that the multiplexing operation is not interrupted by the letter toggling operation, which could cause visual artifacts on the LED matrix.*
- *Refreshing one column every 4 ms means the whole 5-column letter is redrawn every 20 ms (50 Hz), which is fast enough for the eye not to perceive any flickering.*

# LAB09-1a: Encoder Readout

## Description
The objective of this project is to read the position of a quadrature encoder using the dedicated hardware encoder mode of the STM32 timers. The microcontroller computes the rotation speed in RPM (Rotations Per Minute) and determines the direction of rotation ("+" for clockwise, "-" for counterclockwise). The result is then transmitted to a PC via UART using DMA.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the pins for USART communication. The PA2 pin is used for USART2 TX and the PA3 pin is used for USART2 RX.
3. In the "Connectivity" tab of the IOC file, select USART2 and set the Mode to "Asynchronous". Configure the Baud Rate to 115200 Bits/s, Word Length to 8 Bits, Parity to None, and Stop Bits to 1.
4. Still in the USART2 configuration, enable the "DMA Settings" and configure the DMA for transmission (TX) as follows:
    - DMA Request: USART2_TX
    - Direction: Memory to Peripheral
    - Mode: Normal
    - Priority: Low
5. In the "NVIC Settings" tab, enable the USART2 global interrupt.
6. In the IOC file, configure the pins of the encoder PC6 and PC7 as TIM3_CH1 and TIM3_CH2 respectively.
7. In the "Timers" tab of the IOC file, select TIM3 and set the Combined Channels to "Encoder Mode". Then, in the "Parameter Settings" tab, configure it as follows:
    - Counter Period (AutoReload Register): 65535
    - Encoder Mode: Encoder Mode TI1 and TI2. This mode updates the timer counter on both A and B signal edges, resulting in a ±48 increase of the counter per revolution
    - Polarity (CH1 and CH2): Rising Edge
    - Input Filter (CH1 and CH2): 15. Mechanical encoders bounce for a few microseconds, so this digital filter requires 8 consecutive stable samples to validate a transition, eliminating the need for hardware debouncing
8. Configure a timer (TIM2) to generate an interrupt every second. Set the prescaler to 8399 and the period to 9999 to achieve a 1-second interval (assuming an 84 MHz clock) and enable the TIM2 global interrupt in the NVIC settings.
9. Generate the code and open the main.c file.
10. Define the variables to track the encoder count and to buffer the UART transmission:

    ```c
    // Encoder count
    uint16_t previous_count = 0;

    // UART transmission buffer and length
    char tx_buffer[50];
    int length = 0;
    ```
11. In the main function, before the infinite loop, start TIM3 in encoder mode and TIM2 in interrupt mode:

    ```c
    // Start the encoder interface on TIM3 for all channels
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);

    // Start the timer in interrupt mode (1000 ms)
    HAL_TIM_Base_Start_IT(&htim2);
    ```
12. At the very end of the main.c file, implement the timer callback to calculate the delta of the counter, compute the RPM and transmit the data:

    ```c
    // This function is automatically called when a Timer Period elapses
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        if (htim->Instance == TIM2) {

            // Retrieve the current counter value
            uint16_t current_count = __HAL_TIM_GET_COUNTER(&htim3);

            // Calculate delta
            int16_t delta = (int16_t)(current_count - previous_count);

            // Compute RPM
            int32_t rpm = (delta * 60) / 48;

            // Send to PC via UART using DMA
            length = snprintf(tx_buffer, sizeof(tx_buffer), "Speed: %ld RPM\r\n", rpm);
            HAL_UART_Transmit_DMA(&huart2, (uint8_t *)tx_buffer, length);

            // Save the current count for the next cycle
            previous_count = current_count;
        }
    }
    ```

**Note**:
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
- *By casting the delta calculation to a signed 16-bit integer (int16_t), the C compiler automatically resolves the overflow and underflow scenarios that occur when the timer wraps around 65535 or drops below 0.*
- *Because the delta is signed, turning the encoder backward naturally results in a negative delta, automatically providing the "-" sign for counterclockwise rotation requested by the project specifications.*
- *The delta is measured over exactly one second and there are 48 counts per revolution, so the RPM are obtained as delta * 60 / 48.*

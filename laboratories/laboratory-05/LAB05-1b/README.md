# LAB05-1b: Temperature Sensor (MSByte and LSByte)

## Description
The objective of this project is to read both the Most Significant Byte (MSByte) and the Least Significant Byte (LSByte) of the temperature measured by the LM75/LM75B sensor every second and send it to a remote terminal via UART. The register is read twice in a row to detect a torn reading, so that the fractional part of the temperature is always consistent with its integer part.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the pins for USART communication. The PA2 pin is used for USART2 TX and the PA3 pin is used for USART2 RX.
3. In the IOC file, configure the pins used for I2C communication (PB9 for I2C1_SDA and PB8 for I2C1_SCL).
4. In the "Connectivity" tab of the IOC file, select USART2 and set the Mode to "Asynchronous". Configure the Baud Rate to 115200 Bits/s, Word Length to 8 Bits, Parity to None, and Stop Bits to 1.
5. Still in the USART2 configuration, enable the "DMA Settings" and configure the DMA for transmission (TX) as follows:
    - DMA Request: USART2_TX
    - Direction: Memory to Peripheral
    - Mode: Normal
    - Priority: Low
6. In the "NVIC Settings" tab, enable the USART2 global interrupt.
7. In the "Connectivity" tab of the IOC file, select I2C1 and enable it in I2C mode with a standard speed of 100 kHz.
8. Still in the I2C1 configuration, enable the "DMA Settings" and configure the DMA for reception (RX) as follows:
    - DMA Request: I2C1_RX
    - Direction: Peripheral to Memory
    - Mode: Normal
    - Priority: Low
9. In the "NVIC Settings" tab, enable the I2C1 event interrupt.
10. Configure a timer (TIM2) to generate an interrupt every second. Set the prescaler to 8399 and the period to 9999 to achieve a 1-second interval (assuming an 84 MHz clock) and enable the TIM2 global interrupt in the NVIC settings.
11. Generate the code and open the main.c file.
12. Define the variables to hold the sensor address, the register to read, the two received buffers and the state of the double read, together with the UART transmission buffer:

    ```c
    // I2C communication address and buffer
    uint8_t temp_reg = 0x00;
    uint8_t temp_rx_1[2] = {0};
    uint8_t temp_rx_2[2] = {0};
    uint8_t read_state = 0;
    const uint16_t LM75_ADDR = (0x48 << 1);

    // UART communication buffer and length
    char tx_buffer[50];
    int length = 0;
    ```
13. In the main function, before the infinite loop, start the timer in interrupt mode:

    ```c
    // Start the Timer
    HAL_TIM_Base_Start_IT(&htim2);
    ```
14. At the very end of the main.c file, implement the timer callback to trigger the first non-blocking I2C read every second:

    ```c
    // This function is automatically called when a Timer Period elapses
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        if (htim->Instance == TIM2) {

            // Start first I2C DMA read from sensor
            read_state = 1;
            HAL_I2C_Mem_Read_DMA(&hi2c1, LM75_ADDR, temp_reg, I2C_MEMADD_SIZE_8BIT, temp_rx_1, 2);
        }
    }
    ```
15. Just after the timer callback, implement the I2C receive complete callback to start the second read, compare the two readings and send the result via UART:

    ```c
    // This function is automatically called when the DMA finishes the I2C read
    void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
    {
        if (hi2c->Instance == I2C1) {

            if (read_state == 1) {

                // Start second I2C DMA read from sensor
                read_state = 2;
                HAL_I2C_Mem_Read_DMA(&hi2c1, LM75_ADDR, temp_reg, I2C_MEMADD_SIZE_8BIT, temp_rx_2, 2);

            } else if (read_state == 2) {

                int16_t raw_temp;

                // Check if the MSByte changed between the first reading and second reading
                if (temp_rx_1[0] == temp_rx_2[0]) {

                    // No tear, use the first reading
                    raw_temp = (int16_t)((temp_rx_1[0] << 8) | temp_rx_1[1]);

                } else {

                    // Tear, use the second
                    raw_temp = (int16_t)((temp_rx_2[0] << 8) | temp_rx_2[1]);
                }

                // Dividing by 256.0 handles the bit shifting and decimal conversion automatically
                float final_temp = raw_temp / 256.0f;

                // Format the message
                length = snprintf(tx_buffer, sizeof(tx_buffer), "Temperature: %.3f C\r\n", final_temp);

                // Send it via UART DMA
                HAL_UART_Transmit_DMA(&huart2, (uint8_t *)tx_buffer, length);
            }
        }
    }
    ```

**Note**:
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
- *To format the string with floating-point values, we have to enable the Float formatting option in the project settings under C/C++ Build > Settings > MCU Settings. It is disabled by default to save resources.*
- *If the raw bytes are stored in an int8_t array instead of uint8_t, the C compiler will pad the extra bits with 1s when extending the negative number to 16 bits to preserve the two's complement sign. This corrupts the data calculation (e.g., resulting in -0.125°C instead of the correct value).*
- *The LM75B updates its temperature register independently of the I2C bus. If the sensor finishes a conversion exactly between the MCU reading the MSByte and the LSByte, the data packet is torn: this is why the register is read twice and the second reading is used whenever the MSByte has changed.*

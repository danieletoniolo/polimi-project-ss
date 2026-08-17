# LAB05-1a: Temperature Sensor (MSByte Only)

## Description
The objective of this project is to read the Most Significant Byte (MSByte) of the temperature measured by the LM75/LM75B sensor every second and send it to a remote terminal via UART. The sensor is read over I2C using DMA, so that no blocking transfer happens inside the interrupt callbacks.

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
12. Define the variables to hold the sensor address, the register to read and the received temperature, together with the UART transmission buffer:

    ```c
    // I2C communication address and buffer
    uint8_t temp_reg = 0x00;
    int8_t temp_msb = 0;
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
14. At the very end of the main.c file, implement the timer callback to trigger the non-blocking I2C read every second:

    ```c
    // This function is automatically called when a Timer Period elapses
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        if (htim->Instance == TIM2) {

            // Start I2C DMA read from sensor (Non-blocking)
            HAL_I2C_Mem_Read_DMA(&hi2c1, LM75_ADDR, temp_reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&temp_msb, 1);
        }
    }
    ```
15. Just after the timer callback, implement the I2C receive complete callback to format and send the data via UART once the DMA has finished fetching it:

    ```c
    // This function is automatically called when the DMA finishes the I2C read
    void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
    {
        if (hi2c->Instance == I2C1) {

            // Format the message
            length = snprintf(tx_buffer, sizeof(tx_buffer), "Temperature: %d °C\r\n", temp_msb);

            // Send it via UART DMA
            HAL_UART_Transmit_DMA(&huart2, (uint8_t *)tx_buffer, length);
        }
    }
    ```

**Note**:
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
- *The LM75 I2C address is 0x48 on 7 bits, but the HAL functions expect the address already shifted on 8 bits, which is why it is defined as (0x48 << 1).*
- *The temperature register (0x00) returns two bytes, but only the MSByte is read here: it holds the temperature in °C as a signed integer, so it is stored in an int8_t to preserve the sign of negative temperatures.*
- *The I2C1 event interrupt must be enabled: the HAL uses it to generate the STOP condition and to raise HAL_I2C_MemRxCpltCallback() when the DMA transfer is over.*

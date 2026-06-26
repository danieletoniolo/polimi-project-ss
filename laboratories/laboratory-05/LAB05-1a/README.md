# Project 1a: Temperature Sensor (MSByte Only)

## Description
Objective of this project is to read the Most Significant Byte (MSByte) of the temperature measured by the LM75/LM75B sensor every second and send it to a remote terminal using the UART interface.

## Steps
1. Create a new project in STM32CubeIDE for your Nucleo board.
2. In the IOC file, configure the pin for USART communication. The PA2 pin is used for USART2 TX and the PA3 pin is used for USART2 RX.
3. Configure the pin use for I2C communication (PB9 for I2C1_SDA and PB8 for I2C1_SCL).
4. In the "Connectivity" tab of the IOC file, select USART2 and enable it in asynchronous mode. Set the baud rate to 115200, data bits to 8 including parity, and stop bits to 1.
5. Still in the UART2 configuration, enable the "DMA Settings" and configure the DMA for transmission (TX) as follows:
    - DMA Request: USART2_TX
    - Direction: Memory to Peripheral
    - Mode: Normal
    - Priority: High
6. In the NVIC settings, enable the USART2 global interrupt and set its priority to 0 (highest priority).
7. In the "Connectivity" tab of the IOC file, select I2C1 and enable it in I2C mode in normal mode (100 kHz).
8. Still in the I2C1 configuration, enable the "DMA Settings" and configure the DMA for reception (RX) as follows:
    - DMA Request: I2C1_RX
    - Direction: Peripheral to Memory
    - Mode: Normal
    - Priority: High
9. Configure a timer (TIM2) to generate an interrupt every second. Set the prescaler to 8399 and the period to 9999 to achieve a 1-second interval and enable the TIM2 global interrupt in the NVIC settings.
10. Generate the code and open the main.c file.
11. Define the variables for the I2C and UART handles, as well as a variable to store the temperature value.
    ```c
    // I2C communication address and buffer
    uint8_t temp_reg = 0x00;
    int8_t temp_msb = 0;
    const uint16_t LM75_ADDR = (0x48 << 1);

    // UART communication buffer and length
    char tx_buffer[50];
    int length = 0;
    ```
12. In the main function start the timer in interrupt mode:
    ```c
    // Start the Timer
    HAL_TIM_Base_Start_IT(&htim2);
    ```
13. Implement the Timer callback to trigger the non-blocking I2C read every 1 second:
    ```c
    // This function is automatically called when a Timer Period elapse
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        if (htim->Instance == TIM2) {

            // Start I2C DMA read from sensor (Non-blocking)
            HAL_I2C_Mem_Read_DMA(&hi2c1, LM75_ADDR, temp_reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*)&temp_msb, 1);
        }
    }
    ```
14. Implement the I2C Receive Complete callback to format and send the data via UART once the DMA finishes fetching it:
    ```c
    // This function is automatically called when DMA finish the I2C read
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
**Note:**
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
# LAB06-1b: Accelerometer (TIM Interrupt and UART DMA)

## Description
The objective of this project is to read the acceleration measured by the accelerometer of the expansion board and send it to a remote terminal via UART every second. Compared to the polling version, the 1-second time base is given by a timer interrupt (TIM2) and the UART transmission is performed in DMA mode, so the main loop stays free. The I2C read is still blocking. The code automatically detects which of the two accelerometers (LIS2DE or LIS2DW) is mounted on the board.

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
8. Configure a timer (TIM2) to generate an interrupt every second. Set the prescaler to 8399 and the period to 9999 to achieve a 1-second interval (assuming an 84 MHz clock) and enable the TIM2 global interrupt in the NVIC settings.
9. Generate the code and open the main.c file.
10. Define the variables for the I2C addresses, the sensor registers and the data storage:

    ```c
    // Sensor Identification Enum
    typedef enum {
        SENSOR_NONE = 0,
        SENSOR_LIS2DE,
        SENSOR_LIS2DW
    } SensorType_t;

    // I2C Addresses (8-bit shifted)
    #define LIS2DE_ADDR       (0x28 << 1)
    #define LIS2DW_ADDR       (0x18 << 1)

    // LIS2DE Registers
    #define LIS2DE_CTRL_REG1  0x20
    #define LIS2DE_CTRL_REG4  0x23
    #define LIS2DE_OUT_X      (0x28 | 0x80)

    // LIS2DW Registers
    #define LIS2DW_CTRL1      0x20
    #define LIS2DW_CTRL2      0x21
    #define LIS2DW_CTRL6      0x25
    #define LIS2DW_OUT_X_L    0x28

    // Active Sensor
    SensorType_t active_sensor = SENSOR_NONE;

    // Sensor reading variables
    uint8_t cfg_reg;
    uint8_t raw_data[6];
    int8_t x_raw, y_raw, z_raw;
    float acc_x, acc_y, acc_z;

    // UART transmission buffer and length
    char tx_buffer[100];
    int length;
    ```
11. In the main function, before the infinite loop, detect the type of accelerometer present on the board, configure it and start the timer in interrupt mode:

    ```c
    if (HAL_I2C_IsDeviceReady(&hi2c1, LIS2DE_ADDR, 2, 100) == HAL_OK) {

        // Select active sensor
        active_sensor = SENSOR_LIS2DE;

        // Initialize LIS2DE (1 Hz, Normal mode, +/- 2g)
        cfg_reg = 0x17; // CTRL_REG1
        HAL_I2C_Mem_Write(&hi2c1, LIS2DE_ADDR, LIS2DE_CTRL_REG1, I2C_MEMADD_SIZE_8BIT, &cfg_reg, 1, 100);
        cfg_reg = 0x00; // CTRL_REG4
        HAL_I2C_Mem_Write(&hi2c1, LIS2DE_ADDR, LIS2DE_CTRL_REG4, I2C_MEMADD_SIZE_8BIT, &cfg_reg, 1, 100);

        length = snprintf(tx_buffer, sizeof(tx_buffer), "LIS2DE detected and configured.\r\n");
        HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, length);

    } else if (HAL_I2C_IsDeviceReady(&hi2c1, LIS2DW_ADDR, 2, 100) == HAL_OK) {

        // Select active sensor
        active_sensor = SENSOR_LIS2DW;

        // Initialize LIS2DW (12.5 Hz, High-Performance mode, BDU on, IF_INC on, +/- 2g)
        cfg_reg = 0x14; // CTRL1
        HAL_I2C_Mem_Write(&hi2c1, LIS2DW_ADDR, LIS2DW_CTRL1, I2C_MEMADD_SIZE_8BIT, &cfg_reg, 1, 100);
        cfg_reg = 0x0C; // CTRL2
        HAL_I2C_Mem_Write(&hi2c1, LIS2DW_ADDR, LIS2DW_CTRL2, I2C_MEMADD_SIZE_8BIT, &cfg_reg, 1, 100);
        cfg_reg = 0x00; // CTRL6
        HAL_I2C_Mem_Write(&hi2c1, LIS2DW_ADDR, LIS2DW_CTRL6, I2C_MEMADD_SIZE_8BIT, &cfg_reg, 1, 100);

        length = snprintf(tx_buffer, sizeof(tx_buffer), "LIS2DW detected and configured.\r\n");
        HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, length);

    } else {

        length = snprintf(tx_buffer, sizeof(tx_buffer), "Error: No Accelerometer Detected.\r\n");
        HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, length);
    }

    // Start the timer in interrupt mode
    HAL_TIM_Base_Start_IT(&htim2);
    ```
12. At the very end of the main.c file, implement the timer callback to read the sensor over I2C and transmit the data via UART DMA every second:

    ```c
    // This function is automatically called when a Timer Period elapses
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        if (htim->Instance == TIM2) {

            if (active_sensor == SENSOR_LIS2DE) {

                // LIS2DE Read
                HAL_I2C_Mem_Read(&hi2c1, LIS2DE_ADDR, LIS2DE_OUT_X, I2C_MEMADD_SIZE_8BIT, raw_data, 6, HAL_MAX_DELAY);

                // 8-bit extraction
                int8_t x_raw = (int8_t)raw_data[1];
                int8_t y_raw = (int8_t)raw_data[3];
                int8_t z_raw = (int8_t)raw_data[5];

                // 15.6 mg/digit sensitivity
                acc_x = x_raw * 0.0156f;
                acc_y = y_raw * 0.0156f;
                acc_z = z_raw * 0.0156f;

            } else if (active_sensor == SENSOR_LIS2DW) {

                // LIS2DW Read
                HAL_I2C_Mem_Read(&hi2c1, LIS2DW_ADDR, LIS2DW_OUT_X_L, I2C_MEMADD_SIZE_8BIT, raw_data, 6, HAL_MAX_DELAY);

                // 16-bit extraction (14-bit left justified)
                int16_t x_raw = (int16_t)((raw_data[1] << 8) | raw_data[0]);
                int16_t y_raw = (int16_t)((raw_data[3] << 8) | raw_data[2]);
                int16_t z_raw = (int16_t)((raw_data[5] << 8) | raw_data[4]);

                // 0.061 mg/digit sensitivity
                acc_x = x_raw * 0.000061f;
                acc_y = y_raw * 0.000061f;
                acc_z = z_raw * 0.000061f;
            }

            // Send read data via UART
            if (active_sensor != SENSOR_NONE) {
                length = snprintf(tx_buffer, sizeof(tx_buffer), "X: %+.2f g\r\nY: %+.2f g\r\nZ: %+.2f g\r\n\r\n", acc_x, acc_y, acc_z);
                HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, length);
            }
        }
    }
    ```

**Note**:
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
- *To format the string with floating-point values, we have to enable the Float formatting option in the project settings under C/C++ Build > Settings > MCU Settings. It is disabled by default to save resources.*
- *The 8-bit data for the LIS2DE must be cast to a signed int8_t before multiplying by the sensitivity float. This ensures the C compiler preserves the two's complement sign properly when doing the math.*
- *To read multiple registers in a single transaction from the LIS2DE, the Most Significant Bit (MSB) of the starting address must be set to '1'. This is why we read from 0x28 | 0x80. The LIS2DW handles auto-incrementing automatically via its CTRL2 register.*
- *The LIS2DE is configured for 1 Hz Normal mode with a ±2g range. CTRL_REG1 (0x20) is set to 0x17 to enable all 3 axes, set Normal mode, and set ODR to 1 Hz. CTRL_REG4 (0x23) is set to 0x00 to select the ±2g scale and continuous update. The high-pass filter is bypassed by default.*
- *The LIS2DW is configured for 12.5 Hz High-Performance mode with a ±2g range. CTRL1 (0x20) is set to 0x14 to select High-Performance mode and an ODR of 12.5 Hz. CTRL2 (0x21) is set to 0x0C to enable Block Data Update (BDU) and register address auto-increment (IF_INC). CTRL6 (0x25) is set to 0x00 for the ±2g range and default ODR/2 filtering.*
- *The I2C read is still blocking inside the timer callback: the CPU waits for the 6 bytes to be transferred. The next project removes this last blocking call by reading the sensor in DMA mode.*

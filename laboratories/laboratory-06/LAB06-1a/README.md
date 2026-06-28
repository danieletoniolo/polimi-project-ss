# Project 1a: Accelerometer (Basic Polling)

## Description
Objective of this project is to read the acceleration measured by accelerometer and send it to a remote terminal via UART every second.

## Steps
1. Create a new project in STM32CubeIDE for your Nucleo board.
2. In the IOC file, configure the pin for USART communication. The PA2 pin is used for USART2 TX and the PA3 pin is used for USART2 RX.
3. Configure the pin use for I2C communication (PB9 for I2C1_SDA and PB8 for I2C1_SCL).
4. In the "Connectivity" tab of the IOC file, select USART2 and enable it in asynchronous mode. Set the baud rate to 115200, data bits to 8 including parity, and stop bits to 1.
5. In the "Connectivity" tab of the IOC file, select I2C1 and enable it in I2C mode in normal mode (100 kHz).
6. Generate the code and open the main.c file.
7. Define the variables for the I2C addresses, registers, and data storage:
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
8. In the main function we detect the type of accelerometer present on the board:
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
        HAL_UART_Transmit(&huart2, (uint8_t*)tx_buffer, length, 100);

    } else if (HAL_I2C_IsDeviceReady(&hi2c1, LIS2DW_ADDR, 2, 100) == HAL_OK) {

        // Select active sensor
        active_sensor = SENSOR_LIS2DW;

        // Initialize LIS2DW (12.5 Hz, HP mode, BDU on, IF_INC on, +/- 2g)
        cfg_reg = 0x14; // CTRL1
        HAL_I2C_Mem_Write(&hi2c1, LIS2DW_ADDR, LIS2DW_CTRL1, I2C_MEMADD_SIZE_8BIT, &cfg_reg, 1, 100);
        cfg_reg = 0x0C; // CTRL2
        HAL_I2C_Mem_Write(&hi2c1, LIS2DW_ADDR, LIS2DW_CTRL2, I2C_MEMADD_SIZE_8BIT, &cfg_reg, 1, 100);
        cfg_reg = 0x00; // CTRL6
        HAL_I2C_Mem_Write(&hi2c1, LIS2DW_ADDR, LIS2DW_CTRL6, I2C_MEMADD_SIZE_8BIT, &cfg_reg, 1, 100);

        length = snprintf(tx_buffer, sizeof(tx_buffer), "LIS2DW detected and configured.\r\n");
        HAL_UART_Transmit(&huart2, (uint8_t*)tx_buffer, length, 100);

    } else {

        length = snprintf(tx_buffer, sizeof(tx_buffer), "Error: No Accelerometer Detected.\r\n");
        HAL_UART_Transmit(&huart2, (uint8_t*)tx_buffer, length, 100);
    }
    ```
9. In the main loop, implement the data fetching based on the type of the sensor and transmit the data:
    ```c
    while (1)
    {
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
            HAL_UART_Transmit(&huart2, (uint8_t*)tx_buffer, length, 100);
        }

        // Wait 1s for the next reading
        HAL_Delay(1000);
    }
    ```
**Note**:
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
- *To format the string with floating-point values, we have to enable the Float formatting option in the project settings under C/C++ Build > Settings > MCU Settings. It is disabled by default to save resources.*
- *The 8-bit data for the LIS2DE must be cast to a signed int8_t before multiplying by the sensitivity float. This ensures the C compiler preserves the two's complement sign properly when doing the math.*
- *To read multiple registers in a single transaction from the LIS2DE, the Most Significant Bit (MSB) of the starting address must be set to '1'. This is why we read from 0x28 | 0x80. The LIS2DW handles auto-incrementing automatically via its CTRL2 register.*
- *The LIS2DE is configured for 1 Hz Normal mode with a ±2g range. CTRL_REG1 (0x20) is set to 0x17 to enable all 3 axes, set Normal mode, and set ODR to 1 Hz. CTRL_REG4 (0x23) is set to 0x00 to select the ±2g scale and continuous update. The high-pass filter is bypassed by default.*
- *The LIS2DW is configured for 1.6 Hz Low-Power Mode 1 with a ±2g range. CTRL1 (0x20) is set to 0x10 to select Low-Power mode 1 and an ODR of 1.6 Hz. CTRL2 (0x21) is set to 0x0C to enable Block Data Update (BDU) and auto-increment. CTRL6 (0x25) is set to 0x00 for the ±2g range and default ODR/2 filtering.*
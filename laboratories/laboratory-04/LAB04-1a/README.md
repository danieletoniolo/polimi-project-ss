# Project 1a: ADC software triggered

## Description
Objective of this project is to acquire the voltage of the potentiometer connected to the ADC pin of the Nucleo board every second and send the value to the PC via UART. The ADC will be used in polling mode and the UART will be used in DMA mode to send the data to the PC.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the pin for USART communication. The PA2 pin is used for USART2 TX and the PA3 pin is used for USART2 RX.
3. Also in the IOC file, configure the pin connected to the potentiometer (PA1) as an ADC input (ADC1_IN1).
5. In the "Analog" tab select ADC1 and enable the channel corresponding to the pin connected to the potentiometer (ADC1_IN1). Set the sampling time to 480 cycles and the resolution to 12 bits.
4. In the "Connectivity" tab of the IOC file, select USART2 and enable it in asynchronous mode. Set the baud rate to 115200, data bits to 8 including parity, and stop bits to 1.
6. Still in the UART2 configuration, enable the "DMA Settings" and configure the DMA for transmission (TX) as follows:
    - DMA Request: USART2_TX
    - Direction: Memory to Peripheral
    - Mode: Normal
    - Priority: High
6. In the NVIC settings, enable the USART2 global interrupt and set its priority to 0 (highest priority).
7. Generate the code and open the main.c file.
8. Define the variable to handle UART transmission and ADC reading:
    ```c
    // UART transmission buffer and length
    char tx_buffer[50];
    int length;

    // ADC raw value and voltage conversion
    uint32_t raw_value = 0;
    float voltage = 0;
    ```
9. In the main loop function, start the ADC conversion and read the value, then convert it to voltage and send it via UART every second using a delay:
    ```c
    while (1)
    {
        // Start the ADC conversion
        HAL_ADC_Start(&hadc1);

        // We give it a timeout of 100 milliseconds just in case it gets stuck
        if (HAL_ADC_PollForConversion(&hadc1, 100) == HAL_OK) {

            // Read the digital value from the ADC data register
            raw_value = HAL_ADC_GetValue(&hadc1);

            // Convert the raw 12-bit value (0-4095) to a voltage (0 to 3.3V)
            voltage = (raw_value / 4095.0) * 3.3;

            // Format the string to 3 decimal places with a carriage return/newline
            length = snprintf(tx_buffer, sizeof(tx_buffer), "Voltage: %.3f V\r\n", voltage);

            // Send the value to the remote terminal via UART DMA
            HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, length);
        }

        // Stop the ADC
        HAL_ADC_Stop(&hadc1);

        // Wait exactly 1 second before acquiring the next value
        HAL_Delay(1000);
    }
    ```

**Note**:
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
- *To format the string with floating-point values, we have to enable the Float formatting option in the project settings under C/C++ Build > Settings > MCU Settings. It is disabled by default to save resources.*
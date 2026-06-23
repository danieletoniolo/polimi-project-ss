# Project 2a: ADC single acquisition interrupt

## Description
Objective of this project is to acquire the voltage of the potentiometer connected to the ADC pin of the Nucleo board every second and send the value to the PC via UART. The ADC will be used in interrupt mode and the UART will be used in DMA mode to send the data to the PC.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the pin for USART communication. The PA2 pin is used for USART2 TX and the PA3 pin is used for USART2 RX.
3. Also in the IOC file, configure the pin connected to the potentiometer (PA1) as an ADC input (ADC1_IN1).
5. In the "Analog" tab select ADC1 and enable the channel corresponding to the pin connected to the potentiometer (ADC1_IN1). Set the sampling time to 480 cycles and the resolution to 12 bits. Also in the "NVIC Settings" of the ADC1 configuration, enable the global interrupt.
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
        // Start the ADC conversion with Interrupts enabled
        HAL_ADC_Start_IT(&hadc1);

        // Wait 1 second before triggering the next conversion
        HAL_Delay(1000);
    }
    ```
10. Implement the ADC conversion complete callback function to read the raw value, convert it to voltage, and send it via UART using DMA:
    ```c
    // This callback is automatically called when the ADC finishes a conversion
    void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
    {
        // Check if the interrupt was triggered by ADC1
        if (hadc->Instance == ADC1)
        {
            // Read the digital value (no need to poll, we know it's ready!)
            raw_value = HAL_ADC_GetValue(&hadc1);

            // Convert to voltage
            voltage = (raw_value / 4095.0) * 3.3;

            // Format the string
            length = snprintf(tx_buffer, sizeof(tx_buffer), "Voltage: %.3f V\r\n", voltage);

            // Send the value via UART (DMA)
            HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, length);
        }
    }
    ```

**Note**:
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
- *To format the string with floating-point values, we have to enable the Float formatting option in the project settings under C/C++ Build > Settings > MCU Settings. It is disabled by default to save resources.*
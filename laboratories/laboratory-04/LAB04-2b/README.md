# LAB04-2b: ADC Single Acquisition (TIM Triggered) to UART

## Description
The objective of this project is to acquire the voltage of the potentiometer connected to the ADC pin of the Nucleo board every second and send the value to the PC via UART. The conversion is started by the hardware through a timer (TIM2), the ADC is used in interrupt mode and the UART is used in DMA mode to send the data to the PC.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the pins for USART communication. The PA2 pin is used for USART2 TX and the PA3 pin is used for USART2 RX.
3. In the IOC file, configure the pin connected to the potentiometer (PA1) as an ADC input (ADC1_IN1).
4. In the "Connectivity" tab of the IOC file, select USART2 and set the Mode to "Asynchronous". Configure the Baud Rate to 115200 Bits/s, Word Length to 8 Bits, Parity to None, and Stop Bits to 1.
5. Still in the USART2 configuration, enable the "DMA Settings" and configure the DMA for transmission (TX) as follows:
    - DMA Request: USART2_TX
    - Direction: Memory to Peripheral
    - Mode: Normal
    - Priority: Low
6. In the "NVIC Settings" tab, enable the USART2 global interrupt.
7. In the "Analog" tab of the IOC file, select ADC1 and enable the channel connected to the potentiometer (IN1). Then, in the "Parameter Settings" tab, set the Resolution to 12 bits, the Sampling Time to 480 Cycles and the "External Trigger Conversion Source" to "Timer 2 Trigger Out event".
8. In the "NVIC Settings" tab of the ADC1 configuration, enable the ADC1 global interrupt.
9. To make the ADC conversion start every second, configure TIM2 to generate an update event every second. In the "Timers" tab select TIM2, set the prescaler to 8399 and the counter period to 9999 to reach a frequency of 1 Hz (assuming an 84 MHz clock), and in the "Trigger Output (TRGO) Parameters" section select "Update Event" as the trigger event.
10. Generate the code and open the main.c file.
11. Define the variables to handle the UART transmission and the ADC reading:

    ```c
    // UART transmission buffer and length
    char tx_buffer[50];
    int length;

    // ADC raw value and voltage conversion
    uint32_t raw_value = 0;
    float voltage = 0;
    ```
12. In the main function, before the infinite loop, start the timer and the ADC in interrupt mode:

    ```c
    // Start the Timer
    HAL_TIM_Base_Start(&htim2);

    // Start the ADC in Interrupt Mode
    HAL_ADC_Start_IT(&hadc1);
    ```
13. At the very end of the main.c file, implement the ADC conversion complete callback function to read the ADC value, convert it to voltage, and send it via UART using DMA:

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
- *The timer is started with HAL_TIM_Base_Start() and not with HAL_TIM_Base_Start_IT(): the update event is routed to the ADC through the TRGO line by the hardware, so no timer interrupt is needed on the CPU side.*

# Project 1b: UART communication (DMA)

## Description
Objective of this project is to send information from the microcontroller to the computer via the USART communication using DMA. 
We will send the message "Hello World!" from the microcontroller to the computer every second. 

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the pin for USART communication. The PA2 pin is used for USART2 TX and the PA3 pin is used for USART2 RX.
3. Configure a timer (TIM2) to generate an interrupt every second. Set the prescaler to 8399 and the period to 9999 to achieve a 1-second interval and enable the TIM2 global interrupt in the NVIC settings. 
4. In the "Connectivity" tab of the IOC file, select USART2 and enable it in asynchronous mode. Set the baud rate to 115200, data bits to 8 including parity, and stop bits to 1.
5. Still in the UART2 configuration, enable the "DMA Settings" and configure the DMA for transmission (TX) as follows:
    - DMA Request: USART2_TX
    - Direction: Memory to Peripheral
    - Mode: Normal
    - Priority: High
6. In the NVIC settings, enable the USART2 global interrupt and set its priority to 0 (highest priority).
7. Generate the code and open the main.c file.
8. Define the variable to hold the message to be sent and the length of the message:
    ```c
    // Transmission buffer
    char tx_buffer[50];

    // Length of transmission buffer
    int length;
    ```
9. In the main function, initialize the timer to generate an interrupt every second:
    ```c
    // Start the timer in interrupt mode
    HAL_TIM_Base_Start_IT(&htim2);
10. Override the callback function for the timer interrupt to send the message every second:
    ```c
    // This function is automatically called when a Timer Period elapse
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        if (htim->Instance == TIM2) {

            // Format the string with name, birth year and a newline character
            length = snprintf(tx_buffer, sizeof(tx_buffer), "Hello World!\n");

            // Send the string via UART2
            HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, length);
        }
    }
    ```

**Note**: *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
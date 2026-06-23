# Project 3a: ADC scan using DMA

## Description
Objective of the project is to acquire 3 voltages (potentiometer, temperature sensor, Vref) every 1 s and to send them to a remote terminal. The acquisition are started via hardware by a timer and data are saved in the microcontroller memory using DMA.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the pin for USART communication. The PA2 pin is used for USART2 TX and the PA3 pin is used for USART2 RX.
3. Also in the IOC file, configure the pin connected to the potentiometer (PA1) as an ADC input (ADC1_IN1).
4. In the "Connectivity" tab of the IOC file, select USART2 and enable it in asynchronous mode. Set the baud rate to 115200, data bits to 8 including parity, and stop bits to 1.
5. Still in the UART2 configuration, enable the "DMA Settings" and configure the DMA for transmission (TX) as follows:
    -DMA Request: USART2_TX
    -Direction: Memory to Peripheral
    -Mode: Normal
    -Priority: High
6. In the NVIC settings of the USART2 configuration, enable the global interrupt for USART2.
7. In the "Analog" tab, select ADC1 and enable three channels: IN1 (PA1 for the potentiometer), Temperature Sensor Channel, and Vrefint Channel.
8. Configure the ADC parameters for Scan Mode:
    - Scan Conversion Mode: Enable
    - Continuous Conversion Mode: Disable
    - DMA Continuous Requests: Enable
    - Number of Conversion: 3
    -External Trigger: Timer 2 Trigger Out event (Rising Edge)
    - Set the Ranks (Rank 1 = Channel 1, Rank 2 = Temperature Sensor, Rank 3 = Vrefint) and set all Sampling Times to 480 Cycles.
9. In the ADC1 "DMA Settings" tab, add a DMA request for ADC1. Configure it as Circular mode, and set the Data Width to Word for both Peripheral and Memory.
10. The ADC will be triggered by TIM2, so in the "Trigger" tab select "External Trigger Conversion Source" as "Timer 2 Trigger Out event".
11. To make the ADC conversion start every second, configure TIM2 to generate an update event every second. In the "Timers" tab select TIM2 and set the prescaler to 8399 and the counter period to 9999 to reach a frequency of 1 Hz. In the"Trigger Out Parameters" select "Update Event" as the trigger output event.
12. Generate the code and open the main.c file.
13. Define the variables to handle the ADC DMA buffer, calculations, and UART transmission:
    ```c
    // Array to hold the 3 raw ADC values: [0]=Pot, [1]=Temp, [2]=Vref
    uint32_t adc_buffer[3];

    // Variables for calculated values
    float v_pot = 0;
    float temp_celsius = 0;
    float v_refint = 0;

    // UART transmission buffer
    char tx_buffer[100];
    int length;
    ```
14. In the main function, start the ADC in DMA mode and start the timer:
    ```c
    // Start the Timer
    HAL_TIM_Base_Start(&htim2);

    // Start the ADC in DMA mode.
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 3);
    ```
15. Implement the ADC conversion complete callback function (which is triggered by the DMA when the array is full) to calculate the values and send them via UART:
    ```c
    // This callback fires when the DMA finishes transferring all 3 ADC readings into adc_buffer
    void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
    {
        if (hadc->Instance == ADC1)
        {
            // Compute Potentiometer Voltage (in V)
            v_pot = (adc_buffer[0] / 4095.0) * 3.3;

            // Compute Temperature Sensor Voltage (in mV)
            float v_sense = (adc_buffer[1] / 4095.0) * 3300.0;

            // Convert Temperature Sensor Voltage to Celsius
            temp_celsius = ((v_sense - 760.0) / 2.5) + 25.0;

            // Compute Vrefint Voltage (in V)
            v_refint = (adc_buffer[2] / 4095.0) * 3.3;

            // Format the string to send all values
            length = snprintf(tx_buffer, sizeof(tx_buffer), "Vpot: %.3fV | Temp: %.3f C | Vrefint: %.3f\r\n", v_pot, temp_celsius, v_refint);

            // Send via UART
            HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, length);
        }
    }
    ```
**Note**:
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
- *To format the string with floating-point values, we have to enable the Float formatting option in the project settings under C/C++ Build > Settings > MCU Settings. It is disabled by default to save resources.*
- *The Raw ADC to Celsius conversion uses the following sequence of formulas:*
    - Sensor Voltage Calculation (in mV):
        $$V_{SENSE} = \frac{ADC_{RAW}}{4095} * 3300 \ mV$$
    - Temperature Calculation (based on the sensor's characteristics):
        $$Temperature_{(°C)} = \frac{V_{SENSE} - 760 \ mV}{2.5 \ mV/°C} + 25°C$$
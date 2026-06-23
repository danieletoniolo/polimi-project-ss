# Project 3b: Light Dependent Resistor (LDR)

## Description
The objective of this project is to acquire the resistance value of a Light Dependent Resistor (LDR) every 1 ms using a hardware timer. The data is saved directly to the microcontroller memory using DMA in circular mode. Every 1 second (when 1000 samples are collected), the CPU calculates the average, converts the resistance value to a lux level (illuminance), and transmits it via UART.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure USART2 for asynchronous communication (115200 baud). Enable the DMA Settings for USART2_TX (Memory to Peripheral, Normal Mode) and enable the USART2 global interrupt in the NVIC settings.
3. Configure the pin connected to the LDR voltage divider (PA0) as an ADC input (ADC1_IN0).
4. In the "Analog" tab, select ADC1 and enable the IN0 channel. Configure the ADC parameters:
    - External Trigger: Timer 2 Trigger Out event (Rising Edge)
    - DMA Continuous Requests: Enable
5. In the ADC1 "DMA Settings" tab, add a DMA request for ADC1. Configure it as Circular mode, and set the Data Width to Word for both Peripheral and Memory. Ensure the DMA stream interrupt is enabled in the NVIC settings.
6. To make the ADC conversion start every second, configure TIM2 to generate an update event every second. In the "Timers" tab select TIM2 and set the prescaler to 83 and the counter period to 9999 to reach a frequency of 1 Hz. In the"Trigger Out Parameters" select "Update Event" as the trigger output event.
7. Generate the code and open the main.c file.
8. Define the variables to handle the ADC DMA buffer, calculations, and UART transmission:
    ```c
    // Array to hold 2000 samples (2 second of data at 1ms intervals)
    uint32_t adc_buffer[2000];

    // Variables for calculations
    float v_adc = 0;
    float r_ldr = 0;
    float lux = 0;

    // UART transmission buffer
    char tx_buffer[100];
    int length;
    ```
9. In the main function, start the ADC in DMA mode and the timer:
    ```c
    // Start the ADC in DMA mode
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 2000);

    // Start the Timer
    HAL_TIM_Base_Start(&htim2);
    ```
10. Add a helper function to process the math.
    ```c
    // Helper function to process 1000 samples and send UART
    void Process_LDR_Data(uint16_t start_index)
    {
        uint32_t adc_sum = 0;

        // Sum up 1000 values from the specified half of the buffer
        for(int i = start_index; i < start_index + 1000; i++) {
            adc_sum += adc_buffer[i];
        }

        // Compute the average raw value
        float avg_raw = adc_sum / 1000.0;

        // Convert average raw value to Voltage
        v_adc = (avg_raw / 4095.0) * 3.3;

        // Convert to Resistance (kOhms)
        if (v_adc >= 3.29) {
            v_adc = 3.29;
        }
        r_ldr = (v_adc * 100.0) / (3.3 - v_adc);

        // Convert Resistance to LUX
        lux = 10.0 * pow((100.0 / r_ldr), 1.25);

        // Format and transmit via UART
        length = snprintf(tx_buffer, sizeof(tx_buffer), "V: %.2fV | LDR: %.1f kOhm | LUX: %.1f\r\n", v_adc, r_ldr, lux);
        HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, length);
    }
    ```
11. In the DMA interrupt callback, call the helper function to process the data:
    ```c
    // This callback is automatically called when ADC finishes a conversion of the first 1000 sample
    void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
    {
        if (hadc->Instance == ADC1) {
            Process_LDR_Data(0);
        }
    }

    // This callback is automatically called when ADC finishes a conversion of the second 1000 sample
    void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
    {
        if (hadc->Instance == ADC1) {
            Process_LDR_Data(1000);
        }
    }
    ```
**Note**:
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
- *To format the string with floating-point values, we have to enable the Float formatting option in the project settings under C/C++ Build > Settings > MCU Settings. It is disabled by default to save resources.*
- *The Volt to Resistance to LUX conversion is the following formula:*
    - Voltage Divider Equation:
        $$V_{ADC} = 3.3 V * \left(\frac{R_{LDR}}{R_{LDR} + 100 k\Omega}\right)$$
    - LDR Resistance Calculation (inverting the voltage divider equation):
        $$R_{LDR} = \frac{V_{ADC} * 100 k\Omega}{3.3 V - V_{ADC}}$$
    - Illumination (LUX) Calculation (derived from the LDR characteristic curve):
        $$LUX \approx 10 * \left(\frac{100 k\Omega}{R_{LDR}}\right)^{1.25}$$
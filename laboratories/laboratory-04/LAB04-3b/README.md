# LAB04-3b: Light Dependent Resistor (LDR) with LUX Calculation

## Description
The objective of this project is to acquire the voltage across a Light Dependent Resistor (LDR) every 1 ms using a hardware timer. The data are saved directly to the microcontroller memory using DMA in circular mode. Every second (when 1000 samples have been collected), the CPU calculates the average, converts it to a resistance value and then to a lux level (illuminance), and transmits it via UART.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the pins for USART communication. The PA2 pin is used for USART2 TX and the PA3 pin is used for USART2 RX.
3. In the IOC file, configure the pin connected to the LDR voltage divider (PA0) as an ADC input (ADC1_IN0).
4. In the "Connectivity" tab of the IOC file, select USART2 and set the Mode to "Asynchronous". Configure the Baud Rate to 115200 Bits/s, Word Length to 8 Bits, Parity to None, and Stop Bits to 1.
5. Still in the USART2 configuration, enable the "DMA Settings" and configure the DMA for transmission (TX) as follows:
    - DMA Request: USART2_TX
    - Direction: Memory to Peripheral
    - Mode: Normal
    - Priority: Low
6. In the "NVIC Settings" tab, enable the USART2 global interrupt.
7. In the "Analog" tab of the IOC file, select ADC1 and enable the channel connected to the LDR (IN0). Then, in the "Parameter Settings" tab, set the Resolution to 12 bits, the Sampling Time to 480 Cycles, the "DMA Continuous Requests" to Enable and the "External Trigger Conversion Source" to "Timer 2 Trigger Out event".
8. Still in the ADC1 configuration, enable the "DMA Settings" and configure the DMA for ADC1 as follows:
    - DMA Request: ADC1
    - Direction: Peripheral to Memory
    - Mode: Circular
    - Priority: Low
    - Data Width: Word for both Peripheral and Memory
9. In the "NVIC Settings" tab, ensure that the DMA stream global interrupt used by ADC1 is enabled.
10. To make the ADC conversion start every 1 ms, configure TIM2 to generate an update event every 1 ms. In the "Timers" tab select TIM2, set the prescaler to 83 and the counter period to 999 to reach a frequency of 1 kHz (assuming an 84 MHz clock), and in the "Trigger Output (TRGO) Parameters" section select "Update Event" as the trigger event.
11. Generate the code and open the main.c file.
12. Define the variables to handle the ADC DMA buffer, the calculations and the UART transmission:

    ```c
    // Array to hold 2000 samples (2 seconds of data at 1ms intervals)
    uint32_t adc_buffer[2000];

    // Variables for calculations
    float v_adc = 0;
    float r_ldr = 0;
    float lux = 0;

    // UART transmission buffer
    char tx_buffer[100];
    int length;
    ```
13. In the main function, before the infinite loop, start the ADC in DMA mode and the timer:

    ```c
    // Start the ADC in DMA mode
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 2000);

    // Start the Timer
    HAL_TIM_Base_Start(&htim2);
    ```
14. Define a helper function to average the samples of half of the buffer, convert them to lux and transmit the result:

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
15. At the very end of the main.c file, implement the ADC half-conversion and conversion complete callbacks, calling the helper function on the half of the buffer that has just been filled:

    ```c
    // This callback is automatically called when ADC finishes a conversion of the first 1000 samples
    void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
    {
        if (hadc->Instance == ADC1) {
            Process_LDR_Data(0);
        }
    }

    // This callback is automatically called when ADC finishes a conversion of the second 1000 samples
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
- *The DMA works in circular mode on a 2000-sample buffer: the half-complete callback processes the first 1000 samples while the DMA is already filling the second half, and vice versa. Since one sample is acquired every 1 ms, each callback fires exactly once per second.*
- *The Volt to Resistance to LUX conversion uses the following sequence of formulas:*
    - Voltage Divider Equation:
        $$V_{ADC} = 3.3 V * \left(\frac{R_{LDR}}{R_{LDR} + 100 k\Omega}\right)$$
    - LDR Resistance Calculation (inverting the voltage divider equation):
        $$R_{LDR} = \frac{V_{ADC} * 100 k\Omega}{3.3 V - V_{ADC}}$$
    - Illumination (LUX) Calculation (derived from the LDR characteristic curve):
        $$LUX \approx 10 * \left(\frac{100 k\Omega}{R_{LDR}}\right)^{1.25}$$

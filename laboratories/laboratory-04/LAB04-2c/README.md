# LAB04-2c: ADC Single Acquisition (TIM Triggered) to LCD

## Description
The objective of this project is to acquire the voltage of the potentiometer connected to the ADC pin of the Nucleo board every second and display the value on the LCD. The LCD shows the voltage value in volts with 3 decimal places and a bar graph representation of the same value.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the following pins as GPIO Output for the LCD: PA4 (LCD_BL_ON), PB1 (LCD_E), PB2 (LCD_RS), PB12 (LCD_D4), PB13 (LCD_D5), PB14 (LCD_D6), and PB15 (LCD_D7).
3. In the IOC file, configure the pin connected to the potentiometer (PA1) as an ADC input (ADC1_IN1).
4. In the "Analog" tab of the IOC file, select ADC1 and enable the channel connected to the potentiometer (IN1). Then, in the "Parameter Settings" tab, set the Resolution to 12 bits, the Sampling Time to 480 Cycles and the "External Trigger Conversion Source" to "Timer 2 Trigger Out event".
5. In the "NVIC Settings" tab of the ADC1 configuration, enable the ADC1 global interrupt.
6. To make the ADC conversion start every second, configure TIM2 to generate an update event every second. In the "Timers" tab select TIM2, set the prescaler to 8399 and the counter period to 9999 to reach a frequency of 1 Hz (assuming an 84 MHz clock), and in the "Trigger Output (TRGO) Parameters" section select "Update Event" as the trigger event.
7. Generate the code and open the main.c file.
8. Copy the LCD library provided in the course materials into the project: PMDB16_LCD.c in Core/Src and PMDB16_LCD.h in Core/Inc.
9. Define the variables to handle the ADC reading and the LCD display:

    ```c
    // ADC raw value and voltage conversion
    uint32_t raw_value = 0;
    float voltage = 0;

    // LCD row buffer and bar graph status
    char top_row[17];
    int bar_status;
    ```
10. In the main function, before the infinite loop, initialize the LCD and start the timer and the ADC in interrupt mode:

    ```c
    // Initialize the LCD
    lcd_initialize();

    // Start the Timer
    HAL_TIM_Base_Start(&htim2);

    // Start the ADC in interrupt mode
    HAL_ADC_Start_IT(&hadc1);
    ```
11. At the very end of the main.c file, implement the ADC conversion complete callback function to read the ADC value, convert it to voltage, and update the LCD display:

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

            // Format LCD top row
            snprintf(top_row, sizeof(top_row), "Voltage: %.3f V", voltage);
            lcd_println(top_row, 0);

            // Compute bar graph
            bar_status = (raw_value * 80) / 4095;
            lcd_drawBar(bar_status);
        }
    }
    ```

**Note**:
- *To format the string with floating-point values, we have to enable the Float formatting option in the project settings under C/C++ Build > Settings > MCU Settings. It is disabled by default to save resources.*
- *The timer is started with HAL_TIM_Base_Start() and not with HAL_TIM_Base_Start_IT(): the update event is routed to the ADC through the TRGO line by the hardware, so no timer interrupt is needed on the CPU side.*
- *The bar graph of the LCD is 80 pixels wide, so the raw 12-bit value (0-4095) is rescaled to the 0-80 range before being passed to lcd_drawBar().*

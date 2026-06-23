# Project 2c: ADC triggered by TIM to LCD

## Description
Objective of this project is to acquire the voltage of the potentiometer connected to the ADC pin of the Nucleo board every second using a timer (TIM) and display the value on the LCD. In the LCD we will display the voltage value in volts with 3 decimal places and a bar graph representation of the voltage value.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the following pins as GPIO Output for the LCD: PA4 (LCD_BL_ON), PB1 (LCD_E), PB2 (LCD_RS), PB12 (LCD_D4), PB13 (LCD_D5), PB14 (LCD_D6), and PB15 (LCD_D7).
3. Also in the IOC file, configure the pin connected to the potentiometer (PA1) as an ADC input (ADC1_IN1).
4. In the "Analog" tab select ADC1 and enable the channel corresponding to the pin connected to the potentiometer (ADC1_IN1). Set the sampling time to 480 cycles and the resolution to 12 bits. Also in the "NVIC Settings" of the ADC1 configuration, enable the global interrupt. The ADC will be triggered by TIM2, so in the "Trigger" tab select "External Trigger Conversion Source" as "Timer 2 Trigger Out event".
5. To make the ADC conversion start every second, configure TIM2 to generate an update event every second. In the "Timers" tab select TIM2 and set the prescaler to 8399 and the counter period to 9999 to reach a frequency of 1 Hz. In the"Trigger Out Parameters" select "Update Event" as the trigger output event.
6. Generate the code and open the main.c file.
7. Put in the src and inc files the LCD library provided in the course materials (PMDB16_LCD.c and PMDB16_LCD.h).
8. Define the variables to handle the the ADC reading and LCD display:
    ```c
    // ADC raw value and voltage conversion
    uint32_t raw_value = 0;
    float voltage = 0;

    // LCD row buffer and bar graph status
    char top_row[17];
    int bar_status;
    ```
9. In the main function, initialize the LCD and start the ADC and TIM2:
    ```c
    // Initialize the LCD
    lcd_initialize();

    // Start the Timer
    HAL_TIM_Base_Start(&htim2);

    // Start the ADC in interrupt mode
    HAL_ADC_Start_IT(&hadc1);
    ```
10. Implement the ADC conversion complete callback function to read the ADC value, convert it to voltage, and update the LCD display:
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
- *The baud rate could be set to any value supported by the hardware as long as it is within the supported range and the receiver is configured accordingly.*
- *To format the string with floating-point values, we have to enable the Float formatting option in the project settings under C/C++ Build > Settings > MCU Settings. It is disabled by default to save resources.*
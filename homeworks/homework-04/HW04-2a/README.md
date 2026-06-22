# Project 2a: LCD Scrolling Display

## Description
The objective of this project is to display the names of group members on a 16x2 LCD screen. The display must update every second, overlapping the names to create a scrolling effect.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the following pins as GPIO Output for the LCD: PA4 (LCD_BL_ON), PB1 (LCD_E), PB2 (LCD_RS), PB12 (LCD_D4), PB13 (LCD_D5), PB14 (LCD_D6), and PB15 (LCD_D7).
3. Configure a timer (TIM2) to generate an interrupt every second. Set the prescaler to 8399 and the period to 9999 to achieve a 1-second interval and enable the TIM2 global interrupt in the NVIC settings.
4. Generate the code and open the main.c file.
5. Put in the src and inc files the LCD library provided in the course materials (PMDB16_LCD.c and PMDB16_LCD.h).
6. Define the variable to hold the names of the group members and the number of names:
    ```c
    // Members name and number
    char *members[] = {"R. Bresnik", "L. Parmitano", "F. Rubio", "A. Douglas"};
    int num_members = 4;
    ```
7. In the main function, initialize the LCD and the timer to generate an interrupt every second:
    ```c
    // Initialize the LCD
    lcd_initialize();

    // Start TIM2 in Interrupt Mode
    HAL_TIM_Base_Start_IT(&htim2);
    ````
8. Override the callback function for the timer interrupt to update the LCD display every second:
    ```c
    // This function is automatically called when a Timer Period elapse
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        if (htim->Instance == TIM2) {

            static int current_state = 0;

            if (current_state == 0) {

                // Initial state: Blank line on top, Member 1 on bottom
                lcd_println(" ", 0);
                lcd_println(members[0], 1);

            } else {

                // Subsequent states: overlapping names to create the scroll effect
                int top_index = current_state - 1;
                int bottom_index = current_state % num_members;

                lcd_println(members[top_index], 0);
                lcd_println(members[bottom_index], 1);
            }

            // Increment state and wrap around to let the cycle repeat
            current_state++;
            if (current_state > num_members) {
                current_state = 0;
            }
        }
    }
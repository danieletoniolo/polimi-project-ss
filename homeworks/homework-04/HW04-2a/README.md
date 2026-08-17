# HW04-2a: LCD Scrolling Display

## Description
The objective of this project is to display the names of the group members on a 16x2 LCD screen. The display is updated every second, overlapping the names on the two rows to create a scrolling effect.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the following pins as GPIO Output for the LCD: PA4 (LCD_BL_ON), PB1 (LCD_E), PB2 (LCD_RS), PB12 (LCD_D4), PB13 (LCD_D5), PB14 (LCD_D6), and PB15 (LCD_D7).
3. Configure a timer (TIM2) to generate an interrupt every second. Set the prescaler to 8399 and the period to 9999 to achieve a 1-second interval (assuming an 84 MHz clock) and enable the TIM2 global interrupt in the NVIC settings.
4. Generate the code and open the main.c file.
5. Copy the LCD library provided in the course materials into the project: PMDB16_LCD.c in Core/Src and PMDB16_LCD.h in Core/Inc.
6. Define the variables to hold the names of the group members and their number:

    ```c
    // Members name and number
    char *members[] = {"R. Bresnik", "L. Parmitano", "F. Rubio", "A. Douglas"};
    int num_members = 4;
    ```
7. In the main function, before the infinite loop, initialize the LCD and start the timer in interrupt mode:

    ```c
    // Initialize the LCD
    lcd_initialize();

    // Start TIM2 in Interrupt Mode
    HAL_TIM_Base_Start_IT(&htim2);
    ```
8. At the very end of the main.c file, implement the timer interrupt callback function to update the LCD display every second:

    ```c
    // This function is automatically called when a Timer Period elapses
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
    ```

**Note**:
- *The state machine has num_members + 1 states: one initial state with a blank top row, and one state for every member. The modulo on bottom_index makes the last state show the last member on top and the first member on the bottom, so the sequence loops seamlessly.*
- *The static keyword on current_state ensures that the variable is initialized only once and retains its value between the calls of the callback.*

# Project 1b: Push Button (Interrupts)

## Description
Objective of this project is to switch on an LED every time the blue pushbutton is pressed, and to switch it off when the pushbutton is released. The pushbutton input will be used in interrupt mode.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the "Blue Push Button" pin (PC13) as GPIO External Interrupt (with rising and falling edge triggers) and the "Green LED" pin (PA5) as GPIO Output.
3. Also, in the IOC file, in the "NVIC Settings" tab, enable the EXTI line for the "Blue Push Button" (EXTI line[15:10] interrupts) and set its priority.
4. Generate the code and open the main.c file.
5. Override the GPIO EXTI callback function to read the button state and set the LED accordingly (ON when pressed, OFF when released).

    ```c
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

        // Check if we are in the correct GPIO interrupt
        // (Redundant since there is just one GPIO interrupt enabled)

        if (GPIO_Pin == GPIO_PIN_13) {

            // The "Blue Button" is ACTIVE LOW,
            // so we read '0' when it is pressed.

            // The "Green Led" is ACTIVE HIGH,
            // so we write '1' when it is on.

            if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {

                // If the "Blue Button" is pressed (PC13 == '0'),
                // then we turn on the "Green Led" (PA5 == '1').

                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

            } else {

                // If the "Blue Button" is not pressed (PC13 == '1'),
                // then we turn off the "Green Led" (PA5 == '0').

                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

            }
        }
    }
    ```

**Note**: *The "Blue Button" is ACTIVE LOW, which means it reads '0' when pressed. The "Green LED" is ACTIVE HIGH, which means it turns on when we write '1'.*
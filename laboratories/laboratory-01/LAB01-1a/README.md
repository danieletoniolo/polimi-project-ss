# Project 1a: Push Button (Polling)

## Description
Objective of this project is to switch on the green LED on Nucleo board (LD2), every time the blue push button is pressed and to switch off when the push button is released. A polling operation will be used to monitor the state of the push button.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the "Blue Push Button" pin (PC13) as GPIO Input and the "Green LED" pin (PA5) as GPIO Output.
3. Generate the code and open the main.c file.
4. In the main loop, implement a polling mechanism to check the state of the push button:

    ```c
    while (1) {

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
    ```

**Note**: *The "Blue Button" is ACTIVE LOW, which means it reads '0' when pressed. The "Green LED" is ACTIVE HIGH, which means it turns on when we write '1'.*
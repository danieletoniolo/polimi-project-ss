# Project 1a: Microphone (Interrupt)

## Description

Objective of this project is to modify the status (switch on/off) the "Green LED" on the Nucleo board every time you snap your fingers.


## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the microphone pin (PA8) as an external interrupt (GPIO_EXTI8) with a rising edge trigger and also a pull-down resistor to avoid false triggering and the "Green LED" pin (PA5) as GPIO Output.
3. In the "System Core" tab, select "NVIC Settings" and enable the EXTI line[9:5] interrupts.
4. Generate the code and open the main.c file.
5. At the very end of the main.c file, implement the EXTI interrupt callback function to toggle the "Green LED" every time the microphone detects a sound. A 200ms software debounce is implemented to prevent multiple triggers from a single snap.

    ```c
    // This function is automatically called when an External Interrupt (EXTI) triggers
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
    {
        // Check if the interrupt came from the microphone pin (PA8)
        if(GPIO_Pin == GPIO_PIN_8) {

            // Store the time of the last valid snap
            static uint32_t last_snap_time = 0;

            // Get the current millisecond time
            uint32_t current_time = HAL_GetTick();

            // 200-millisecond debounce shield
            if ((current_time - last_snap_time) > 200) {

                // Toggle the Green LED on PA5
                HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        
                // Update the time of the last valid snap
                last_snap_time = current_time;
            }
        }
    }
    ```
**Note**:
- *The microphone pin (PA8) is configured as an external interrupt to trigger the start of the song. The priority of the interrupt can be set in the "NVIC Settings" tab of the IOC file. It is skipped because there is no other interrupt in this project.*
- *The static keyword on last_snap_time ensures that the variable is initialized only once and retains its value between function calls, allowing us to track the time of the last valid snap across multiple interrupts.* 
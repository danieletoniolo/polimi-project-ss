# LAB02-2a: Single Tone (PWM)

## Description
The objective of this project is to play a tone for 3 seconds on the speaker using a PWM signal. The tone to play is a 440 Hz tone (LA4 / A4 note) with a duty cycle of 50%.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the speaker pin (PA9) as an alternate function for TIM1 channel 2 (TIM1_CH2) to generate the PWM signal.
3. In the "Timers" tab of the IOC file, select TIM1 and set "PWM Generation CH2" for Channel 2. Then, in the "Parameter Settings" tab, set the Prescaler to 83, the Counter Period to 2271 and the Pulse to 1135. This generates a PWM signal with a frequency of 440 Hz and a duty cycle of 50%, as described by the following formulas:
$$
f_{PWM} = \frac{f_{TIM}}{(Prescaler + 1) * (Period + 1)} = \frac{84 MHz}{(83 + 1) * (2271 + 1)} \sim 440 Hz
$$
$$
Duty \ Cycle = \frac{Pulse + 1}{Period + 1} = \frac{1135 + 1}{2271 + 1} = 50\%
$$
4. Generate the code and open the main.c file.
5. In the main function, before the infinite loop, start the PWM signal by calling the HAL_TIM_PWM_Start() function with the TIM1 handle and the channel number (TIM_CHANNEL_2) as arguments, then stop it after 3 seconds:

    ```c
    // Start the PWM signal on the speaker channel
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

    // Wait for 3 seconds (3000 milliseconds)
    HAL_Delay(3000);

    // Stop the PWM signal so the note ends
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    ```

**Note**:
- *Ensure that the timer clock is enabled and its frequency is set correctly (84 MHz) in the "Clock Configuration" tab of the IOC file to achieve the desired PWM frequency. The TIM1 clock is derived from the APB2 clock.*

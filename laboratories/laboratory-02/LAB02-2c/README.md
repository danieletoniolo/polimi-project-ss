# Project 2c: Playing a Song (PWM) with Interrupts

## Description

Objective of this project is to play the song "London Bridge is Falling Down" using the speaker via PWM signal generation. The start of the song is triggered by the sound detection of the microphone using an external interrupt (EXTI). The following notes are provided and are the following (fixed Prescaler at 99):

| Note | Frequency (Hz) | Period (ms) | Pulse (ms) |
|------|----------------|-------------|------------|
|DO4 - C4| 262 | 3206.10687 | 1603.053435 |
|DO#4 - C#4| 277 | 3032.490975 | 1516.245487 |
|RE4 - D4| 294 | 2857.142857 | 1428.571429 |
|RE#4 - D#4| 311 | 2700.96463 | 1350.482315 |
|MI4 - E4| 330 | 2545.454545 | 1272.727273 |
|FA4 - F4| 349 | 2406.876791 | 1203.438395 |
|FA#4 - F#4| 370 | 2270.27027 | 1135.135135 |
|SOL4 - G4| 392 | 2142.857143 | 1071.428571 |
|SOL#4 - G#4| 415 | 2024.096386 | 1012.048193 |
|LA4 - A4| 440 | 1909.090909 | 954.5454545 |
|LA#4 - A#4| 466 | 1802.575107 | 901.2875536 |
|SI4 - B4| 494 | 1700.404858 | 850.2024291 |

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the speaker pin (PA9) as an alternate function for TIM1 channel 2 (TIM1_CH2) to generate the PWM signal and the microphone pin (PA8) as an external interrupt (GPIO_EXTI8) to trigger the start of the song. The microphone pin should be configured as an external interrupt with a rising edge trigger and also a pull-down resistor to avoid false triggering.
3. In the "System Core" tab, select "NVIC Settings" and enable the EXTI line[9:5] interrupts.
4. Also, in the IOC file, in the "TIM1" tab, select "PWM Generation CH2" for Channel 2 and then in the "Parameter settings" tab, set the Prescaler to 99, the Counter Period and the pulse can be set to 0 since they will be updated at runtime based on the desired frequency.
5. Generate the code and open the main.c file.
6. Define a struct to hold the note frequency and duration:

    ```c
    typedef struct {
      uint16_t frequency; // Note frequency in Hz
      uint16_t duration;  // Note duration in milliseconds
    } Note;
    ```
7. Create an array of `Note` structs to represent the song "London Bridge is Falling Down" with the corresponding frequencies and durations for each note and calculate the length of the song:

    ```c
    Note londonBridge[] = {
        // "Lon - don bridge is"
        {392, 600}, // G4 (Dotted Quarter)
        {440, 200}, // A4 (Eighth)
        {392, 400}, // G4 (Quarter)
        {349, 400}, // F4 (Quarter)

        // "fal - ling down"
        {330, 400}, // E4 (Quarter)
        {349, 400}, // F4 (Quarter)
        {392, 800}, // G4 (Half)

        // "fal - ling down"
        {294, 400}, // D4 (Quarter)
        {330, 400}, // E4 (Quarter)
        {349, 800}, // F4 (Half)

        // "fal - ling down"
        {330, 400}, // E4 (Quarter)
        {349, 400}, // F4 (Quarter)
        {392, 800}, // G4 (Half)

        // "Lon - don bridge is"
        {392, 600}, // G4 (Dotted Quarter)
        {440, 200}, // A4 (Eighth)
        {392, 400}, // G4 (Quarter)
        {349, 400}, // F4 (Quarter)

        // "fal - ling down"
        {330, 400}, // E4 (Quarter)
        {349, 400}, // F4 (Quarter)
        {392, 800}, // G4 (Half)

        // "my fair"
        {294, 800}, // D4 (Half)
        {392, 800}, // G4 (Half)

        // "la - dy"
        {330, 400},  // E4 (Quarter)
        {262, 1200}, // C4 (Dotted Half)
    };

    // Calculate how many notes are in the array
    int songLength = sizeof(londonBridge) / sizeof(londonBridge[0]);
    ```
8. Create a volatile flag variable to indicate when the microphone interrupt has been triggered.

    ```c
    volatile uint8_t playSongFlag = 0;
    ```

9. Define a function to play a note using PWM by setting the TIM1 channel 2 Pulse and Counter Period based on the note frequency and duration.

    ```c
    void playNote(uint16_t freq) {
        if (freq == 0) {

            // Mute the volume by setting Pulse (Duty Cycle) to 0
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);

        } else {

            // Calculate Period (ARR) based on the 840000 Hz timer frequency
            uint32_t arr = (840000 / freq) - 1;

            // Calculate Pulse (CCRx) for exactly 50% duty cycle
            uint32_t pulse = (arr + 1) / 2 - 1;

            // Write values to the timer registers dynamically
            __HAL_TIM_SET_AUTORELOAD(&htim1, arr);
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, pulse);
        }
    }
    ```
10. In the main function, inside the infinite loop, check if the `playSongFlag` is set. If it is, play the song by iterating through the `londonBridge` array and calling the `playNote` function for each note. After playing the song, reset the `playSongFlag`.

    ```c
    while (1)
    {
        // Check if the microphone triggered the interrupt flag
        if (playSongFlag == 1) {

            // Start the timer
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

            // Step through the array to play the song
            for(int i = 0; i < songLength; i++) {

                playNote(londonBridge[i].frequency);
                HAL_Delay(londonBridge[i].duration);

            }

            // Stop the timer when the song is done
            HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);

            // Reset the flag so it waits for the next loud noise
            playSongFlag = 0;
        }
    }
    ```
11. At the very end of the main.c file, implement the EXTI interrupt callback function to set the `playSongFlag` when the microphone detects a loud noise.

    ```c
    // This function is automatically called when an External Interrupt (EXTI) triggers
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
    {
        // Check if the interrupt came from the microphone pin (PA8)
        if(GPIO_Pin == GPIO_PIN_8) {

            // Only set the flag if the song isn't already playing
            // This prevents the song from glitching if it hears its own noise.
            if (playSongFlag == 0) {
                playSongFlag = 1;
            }
        }
    }
    ```
**Note**:
- *Ensure that the timer clock is enabled and its frequency is set correctly (84 MHz) in the "Clock Configuration" tab of the IOC file to achieve the desired PWM frequency. The TIM1 clock is derived from the APB2 clock.*
- *The microphone pin (PA8) is configured as an external interrupt to trigger the start of the song. The priority of the interrupt can be set in the "NVIC Settings" tab of the IOC file. It is skipped because there is no other interrupt in this project.*
- *Do not put the song loop directly inside the callback function. Doing so will block the SysTick timer, causing the microcontroller to freeze. Always use a flag to defer long executions to the main loop.*

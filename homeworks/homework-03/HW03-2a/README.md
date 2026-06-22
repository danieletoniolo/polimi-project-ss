# Project 2a: Non-Blocking Song Playback (PWM)

## Description
The objective of this project is to recreate the ["Song Playback"](../../../laboratories/laboratory-02/LAB02-2c/) project without using the blocking HAL_Delay() function. Instead, this implementation uses a hardware timer interrupt to create a fully non-blocking state machine.

## Steps
1. Create a new project in STM32CubeIDE for the F401RE Nucleo board.
2. In the IOC file, configure the speaker pin (PA9) as an alternate function for TIM1 channel 2 (TIM1_CH2) to generate the PWM signal and the microphone pin (PA8) as an external interrupt (GPIO_EXTI8) to trigger the start of the song. The microphone pin should be configured as an external interrupt with a rising edge trigger and also a pull-down resistor to avoid false triggering.
3. In the "System Core" tab, select "NVIC Settings" and enable the EXTI line[9:5] interrupts.
4. Also, in the IOC file, in the "TIM1" tab, select "PWM Generation CH2" for Channel 2 and then in the "Parameter settings" tab, set the Prescaler to 99, the Counter Period and the pulse can be set to 0 since they will be updated at runtime based on the desired frequency.
5. To handle the song playback, configure the TIM2 clock source to be the internal clock (APB1) and set the Prescaler to 8399 and the Counter Period to 1999. This will create a timer that ticks every 200 ms, which will be used to manage the timing of the song playback without blocking the main loop.
6. Generate the code and open the main.c file.
7. Define a struct to hold the note frequency and duration:

    ```c
    typedef struct {
      uint16_t frequency; // Note frequency in Hz
      uint16_t duration;  // Note duration in milliseconds
    } Note;
    ```
8. Create an array of `Note` structs to represent the song "London Bridge is Falling Down" with the corresponding frequencies and durations for each note and calculate the length of the song:

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

9. Create volatile flags to handle the state of the song playback and the microphone interrupt.

    ```c
    volatile uint8_t playSongFlag = 0;
    volatile uint8_t isPlaying = 0;

    volatile int currentNoteIndex = 0;
    volatile int ticksRemaining = 0;
    ```

10. Define a function to play a note using PWM by setting the TIM1 channel 2 Pulse and Counter Period based on the note frequency and duration.

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
11. In the main function, inside the infinite loop, check if the `playSongFlag` is set. If it is, play the song by iterating through the `londonBridge` array and calling the `playNote` function for each note. After playing the song, reset the `playSongFlag`.

    ```c
    while (1)
    {
        // Check if the microphone triggered the interrupt flag
        if (playSongFlag == 1) {

            isPlaying = 1;
            playSongFlag = 0;

            // Initialiaze the note index and ticks duration
            currentNoteIndex = 0;
            ticksRemaining = londonBridge[0].duration / TIMER_TICK_MS;

            // Start the song playback
            HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
            playNote(londonBridge[0].frequency);
            HAL_TIM_Base_Start_IT(&htim2);
        }
    }
    ```
12. At the very end of the main.c file, implement the EXTI interrupt callback function to set the `playSongFlag` when the microphone detects a loud noise.

    ```c
    // This function is automatically called when an External Interrupt (EXTI) triggers
    void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
    {
        // Check if the interrupt came from the microphone pin (PA8)
        if(GPIO_Pin == GPIO_PIN_8) {

            // Only set the flag if the song isn't already playing
            // This prevents the song from glitching if it hears its own noise.
            if (isPlaying == 0) {
                playSongFlag = 1;
            }
        }
    }
    ```

13. Just after the EXTI callback function, implement the TIM2 interrupt callback function to handle the timing of the song playback. This function will decrement the `ticksRemaining` counter and move to the next note when the current note's duration has elapsed.

    ```c
    // This function is automatically called when a Timer Period elapse
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
    {
        if (htim->Instance == TIM2) {

            // Subtract 1 tick (200ms) from the current note's duration
            ticksRemaining--;

            // Check if the note has finished to play
            if (ticksRemaining <= 0) {

                // Move to the next note in the array
                currentNoteIndex++;

                // Check if we are still playing the song
                if (currentNoteIndex < songLength) {

                    // Play the new note and compute its tick duration
                    playNote(londonBridge[currentNoteIndex].frequency);
                    ticksRemaining = londonBridge[currentNoteIndex].duration / TIMER_TICK_MS;

                } else {

                    // Stop the speaker and the timer interrupt
                    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
                    HAL_TIM_Base_Stop_IT(&htim2);

                    isPlaying = 0;
                }
            }
        }
    }
    ```

**Note**:
- *Ensure that the timer clock is enabled and its frequency is set correctly (84 MHz) in the "Clock Configuration" tab of the IOC file to achieve the desired PWM frequency. The TIM1 clock is derived from the APB2 clock.*
- *The microphone pin (PA8) is configured as an external interrupt to trigger the start of the song. The priority of the interrupt can be set in the "NVIC Settings" tab of the IOC file. It is skipped because there is no other interrupt in this project.*
- *The TIM2 timer is configured to generate an interrupt every 200 ms because the shortest note duration in the song is 200 ms. This allows for a non-blocking implementation of the song playback, as the main loop can continue to run while the song is being played.*
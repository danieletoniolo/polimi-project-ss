# Project 2b: Playing a Song (PWM)

## Description
Objective of this project is to play the song "London Bridge is Falling Down" using the speaker via PWM signal generation. The following notes are provided and are the following (fixed Prescaler at 99):

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
2. In the IOC file, configure the speaker pin (PA9) as an alternate function for TIM1 channel 2 (TIM1_CH2) to generate the PWM signal.
3. Also, in the IOC file, in the "TIM1" tab, select "PWM Generation CH2" for Channel 2 and then in the "Parameter settings" tab, set the Prescaler to 99, the Counter Period and the pulse can be set to 0 since they will be updated at runtime based on the desired frequency.
4. Generate the code and open the main.c file.
5. Define a struct to hold the note frequency and duration:

    ```c
    typedef struct {
      uint16_t frequency; // Note frequency in Hz
      uint16_t duration;  // Note duration in milliseconds
    } Note;
    ```
6. Create an array of `Note` structs to represent the song "London Bridge is Falling Down" with the corresponding frequencies and durations for each note and calculate the length of the song:

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

    int songLength = sizeof(londonBridge) / sizeof(londonBridge[0]);
    ```

7. Define a function to play a note using PWM by setting the TIM1 channel 2 Pulse and Counter Period based on the note frequency and duration.

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
8. In the main function, start the PWM signal generation and loop through the `londonBridge` array to play each note for its specified duration.

    ```c
    // Start the timer on the speaker channel
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

    // Step through the array to play the song
    for(int i = 0; i < songLength; i++) {

	    // Set the frequency
        playNote(londonBridge[i].frequency);

        // Wait for the duration of the note
        HAL_Delay(londonBridge[i].duration);
    }

    // Stop the timer completely when the song is done
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
    ```




**Note**: *Ensure that the timer clock is enabled and its frequency is set correctly (84 MHz) in the "Clock Configuration" tab of the IOC file to achieve the desired PWM frequency. The TIM1 clock is derived from the APB2 clock.*
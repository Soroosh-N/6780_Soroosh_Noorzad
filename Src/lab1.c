#include <stm32f0xx_hal.h>
#include <assert.h>

int lab1_main(void) {
    HAL_Init(); // Reset of all peripherals, init the Flash and Systick
    SystemClock_Config(); //Configure the system clock
    /* This example uses HAL library calls to control
    the GPIOC peripheral. You’ll be redoing this code
    with hardware register access. */
    __HAL_RCC_GPIOC_CLK_ENABLE(); // Enable the GPIOC clock in the RCC
    // Set up a configuration struct to pass to the initialization function
    // GPIO-C
    GPIO_InitTypeDef initStrGPC = {GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL};
    HAL_GPIO_Init(GPIOC, &initStrGPC); // Initialize pins PC8 & PC9
    HAL_Delay(1);  // Optional delay to ensure the initialization is complete
    assert(GPIOC->MODER == 0x55000); // Assertion on GPIOC
    // GPIO-A
    GPIO_InitTypeDef initStrGPA = {GPIO_PIN_0, GPIO_MODE_INPUT, GPIO_SPEED_FREQ_LOW, GPIO_PULLDOWN};
    HAL_GPIO_Init(GPIOA, &initStrGPA); // Initialize pins PA0
    HAL_Delay(1);  // Optional delay to ensure the initialization is complete
    // assert(GPIOA->MODER == 0x0); // Assertion on GPIOA
    assert((GPIOA->MODER & 0x03) == 0x00);  // Assert that PA0 is configured as input (bits 0 and 1 should be 00)
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); // Start PC8 high
    while (1) {
        HAL_Delay(200); // Delay 200ms
        // Toggle the output state of both PC8 and PC9
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
    }
}
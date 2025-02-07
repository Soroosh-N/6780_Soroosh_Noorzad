#include <stm32f0xx_hal.h>
#include <hal_gpio.h>
#include <assert.h>
#include <main.h>

int lab2_main(void) {
    // Configure the system clock
    SystemClock_Config();

    // ************ Check off 1 ************
    My_HAL_RCC_GPIOA_CLK_ENABLE();
    My_HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef initStrGPC = {0};
    // GPIO-C:
    // Initialize pins
    My_HAL_GPIO_Init(GPIOC, &initStrGPC);
    HAL_Delay(1);
    // Assertion on GPIOC (All the pins should have the 01 value => 01010101 = 55)
    assert(GPIOC->MODER == 0x55000);
    // GPIO-A: Configure PA0 as Digital Input (bits 0 and 1 should be 00)
    My_HAL_GPIO_Init(GPIOA, &initStrGPC);
    HAL_Delay(1);
    // Assert that PA0 is configured as input (00)
    assert((GPIOA->MODER & 0x03) == 0x00);
    // LD3: I/O PC6 - Red LED
    // LD4: I/O PC8 - Orange LED
    // LD5: I/O PC9 - Green LED
    // LD6: I/O PC7 - Blue LED
    // Start PC6 high
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
    uint32_t a = 0;
    uint32_t debouncer = 0;
    HAL_Delay(600);
    while (1) {
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
        HAL_Delay(600);
    }
}
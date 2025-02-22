#include <stm32f0xx_hal.h>
#include <hal_gpio.h>
#include <assert.h>
#include <main.h>

// Commented this function, because we are using this interrupt for the 4th lab.
// Uncomment this function while compiling lab2.
// void EXTI0_1_IRQHandler(void){
//     // ************ Check off 2 ************
//     volatile uint32_t i = 0;
//     My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
//     while(i<4500000){
//         i += 1;
//     }
//     My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
//     // Clear the pending interrupt flag
//     EXTI->PR |= EXTI_PR_PR0;
// }

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
    // Start Green LED
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
    HAL_Delay(1);
    // Assert initial conditions:
    // EXTI0 should be masked and no trigger configured
    assert((EXTI->IMR & EXTI_IMR_IM0) == 0); // Interrupt is disabled
    assert((EXTI->RTSR & EXTI_RTSR_TR0) == 0); // Rising trigger is disabled
    assert((SYSCFG->EXTICR[0] & SYSCFG_EXTICR1_EXTI0) == 0); // Is set to default (PA0)
    my_exti_config();
    // Assert final conditions:
    // EXTI0 should be unmasked and rising trigger enabled
    assert((EXTI->IMR & EXTI_IMR_IM0)); // Interrupt is enabled
    assert((EXTI->RTSR & EXTI_RTSR_TR0)); // Rising trigger is enabled
    assert((SYSCFG->EXTICR[0] & SYSCFG_EXTICR1_EXTI0) == 0); // Ensure PA0 is correctly mapped to EXTI0
    while (1) {
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
        HAL_Delay(600);
    }
}
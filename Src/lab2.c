#include <stm32f0xx_hal.h>
#include <hal_gpio.h>
#include <assert.h>
#include <main.h>

void EXTI0_1_IRQHandler(void){
    // Check if EXTI0 triggered the interrupt
    if (EXTI->PR & EXTI_PR_PR0){
        // Clear the pending interrupt flag
        EXTI->PR |= EXTI_PR_PR0;
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
    }
}


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
    // // GPIO-A: Configure PA0 as Digital Input (bits 0 and 1 should be 00)
    // My_HAL_GPIO_Init(GPIOA, &initStrGPC);
    // HAL_Delay(1);
    // // Assert that PA0 is configured as input (00)
    // assert((GPIOA->MODER & 0x03) == 0x00);
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
    assert((SYSCFG->EXTICR[0] & SYSCFG_EXTICR1_EXTI0) == 0); // Is correctly mapped to PA0
    while (1) {
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
        HAL_Delay(600);
    }
}
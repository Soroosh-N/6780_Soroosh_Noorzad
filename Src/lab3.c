#include <stm32f0xx_hal.h>
#include <hal_gpio.h>
#include <assert.h>
#include <main.h>

// TIM2 Interrupt Handler
void TIM2_IRQHandler(void) {
    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
    TIM2->SR &= ~TIM_SR_UIF;            // Clear update interrupt flag
}

// Timer 3 helper function
void TIM3_PWM_Init(void) {
    My_HAL_RCC_TIMER3_ENABLE();
    TIM3->PSC = 0;                      // Prescaler: 0 (no prescaling)
    TIM3->ARR = 10;                     // Auto-Reload Reg: For 1.25 ms period (800 Hz)

    // Page 386 => Peripheral datasheet
    // Channel 1 - Red
    TIM3->CCMR1 &= ~(0x3);              // Output (CC1S = 00)
    TIM3->CCMR1 |= 0x7 << 4;            // PWM Mode 2 (OC1M[2:0] = 111)
    // Channel 2 - Blue
    TIM3->CCMR1 &= ~(0x3 << 8);         // Output (CC2S = 00)
    TIM3->CCMR1 |= 0x6 << 12;           // PWM Mode 1 (OC2M[2:0] = 110)
    // Both channels
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE;   // Enable preload
    TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;        // Enable output
    // Duty cycle to 20% for both channels (20% of ARR = 2)
    TIM3->CCR1 = 9;                     // Channel 1 - Red
    TIM3->CCR2 = 2;                     // Channel 2 - Blue
    TIM3->CR1 |= TIM_CR1_CEN;           // Start Timer 3
}

int lab3_main(void) {
    // Configure the system clock
    SystemClock_Config();

    // GPIO-C
    My_HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef initStrGPC = {0};
    My_HAL_GPIO_Init(GPIOC, &initStrGPC);   // Initialize pins
    GPIOC->MODER &= ~(0xF << 2 * 6);        // Clear PC6 and PC7 bits to change its function
    GPIOC->MODER |= (0xA << 2 * 6);         // Set PC6 and PC7 as alternate function
    GPIOC->AFR[0] &= ~(0xFF << 4 * 6);      // Clear bits for PC6 (4 bits) and PC7 (4 bits)
    // Set PC6 and PC7 to AF0 = 0x0 (TIM3_CH1 and TIM3_CH2):
    // AF0 is zero and we can just clear the register and don't use line below. but just
    // for the sake of completeness:
    GPIOC->AFR[0] |= ((0x0 << (4 * 6)) | (0x0 << (4 * 7))); 

    // Note: AFR[0] is different than AF0:
    // AFR[0] has eight pins to cover for alternate functions. Each pin has 4 bits,
    // and eight values can be set in these 4 bits. The values are AF0 = 0x0, AF1 = 0x1,
    // ..., AF7 = 0x7. These values are placeholders for different functions, found in
    // the Alternate functions mapping table in the processor datasheet.

    // Configure TIM2
    My_HAL_RCC_TIMER2_ENABLE();                 // Enable timer2 in RCC
    TIM2->PSC = 47999;                          // Prescaler: divides 48 MHz to 1 kHz
    TIM2->ARR = 124;                            // Auto-Reload: 1 kHz / (124+1) = 8 Hz
    TIM2->DIER |= TIM_DIER_UIE;                 // Enable Update Interrupt (Peripheral datasheet => page 451)
    NVIC_EnableIRQ(TIM2_IRQn);                  // Enable TIM2 interrupt in NVIC
    NVIC_SetPriority(TIM2_IRQn, 2);             // Set priority
    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
    // Start Timer
    TIM2->CR1 = TIM_CR1_CEN;  // Enable counter (CEN bit)

    // For Timer 3 we use a helper function:
    TIM3_PWM_Init();
    while (1) {}
}
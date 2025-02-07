#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

void my_exti_config(void){
    // Configure PA0 as input mode
    // Clear bits for PA0
    GPIOA->MODER &= ~(0x3);
    // Set speed as Low Speed
    GPIOA->OSPEEDR &= ~(0x3);
    // Clear bits
    GPIOA->PUPDR &= ~(0x3);
    // Set pull-down (10)
    GPIOA->PUPDR |= (0x2);

    // Enable the RCC clock for SYSCFG
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // Enable rising edge trigger for EXTI0
    EXTI->RTSR |= EXTI_RTSR_TR0;  
    // Unmask EXTI0 (Enable interrupt)
    EXTI->IMR |= EXTI_IMR_IM0;

    // Route PA0 to EXTI0: Connect EXTI0 to PA0 (EXTI0[3:0] = 0000 for PA0)
    SYSCFG->EXTICR[0] &= ~(0xF);

    // Enable the EXTI0_1 interrupt in NVIC
    NVIC_EnableIRQ(EXTI0_1_IRQn);
    // Set EXTI0_1 priority to 0
    NVIC_SetPriority(EXTI0_1_IRQn, 0);
}


void My_HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init){
    if (GPIOx == GPIOA) {
        // Configure PA0
        // Set as Digital Input
        GPIOA->MODER &= ~(0x3);
        // Set speed as Low Speed
        GPIOA->OSPEEDR &= ~(0x3);
        // Clear bits
        GPIOA->PUPDR &= ~(0x3);
        // Set pull-down (10)
        GPIOA->PUPDR |= (0x2);
    } else if (GPIOx == GPIOC) {
        // Configure PC6, PC7, PC8, PC9 as General Purpose Output
        // Clear bits
        GPIOC->MODER &= ~(0xFF000);
        // Set as output mode
        GPIOC->MODER |= (0x55 << 2 * 6);
        // Configure output type as Push-Pull
        GPIOC->OTYPER &= ~(0xF<<6);
        // Configure output speed as Low Speed
        GPIOC->OSPEEDR &= ~(0xFF000);
        // No pull-up or pull-down resistors
        GPIOC->PUPDR &= ~(0xFF000);
    } else {
        // Two long red flash (Error):
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
        HAL_Delay(2000);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
        HAL_Delay(100);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
        HAL_Delay(2000);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
    }
}

void My_HAL_RCC_GPIOC_CLK_ENABLE(void){
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN; // Enable GPIOC clock using bitwise OR
}
void My_HAL_RCC_GPIOA_CLK_ENABLE(void){
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // Enable GPIOA clock
}

/*
void My_HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin)
{
}
*/

GPIO_PinState My_HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    // Read button state
    return (GPIOx->IDR & GPIO_Pin) ? GPIO_PIN_SET : GPIO_PIN_RESET;
}



void My_HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState){
    if (PinState == GPIO_PIN_SET) {
        // Set pin high using BSRR register
        GPIOx->BSRR = GPIO_Pin;
    } else {
        // Set pin low by shifting to the upper half of BSRR
        GPIOx->BSRR = (uint32_t)GPIO_Pin << 16;
    }
}

void My_HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pins) {
    GPIOx->ODR ^= GPIO_Pins;  // Toggle multiple pins using XOR operation
}

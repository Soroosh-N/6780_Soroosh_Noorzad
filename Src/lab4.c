#include <stm32f0xx_hal.h>
#include <hal_gpio.h>
#include <helper_usart.h>
#include <assert.h>
#include <main.h>

/* 
USART 3:
RX,     TX,     RTS,    CTS,    CK
PC5,    PC4,    PB1,    PA6,    PB0,
PB11,   PB10,   PB14,   PB13,   PB12,
PD9,    PD8,    PD12,   PD11,   PD10,
PC11,   PC10,   PD2,    -   ,   PC12,
*/
// Recommended by TA: PB10 and PB11


int lab4_main(void) {
    // Configure the system clock
    SystemClock_Config();
    NVIC_SetPriority(SysTick_IRQn, 0);  // Set Systick priority
    GPIO_InitTypeDef initStr = {0};

    // GPIO-A
    My_HAL_RCC_GPIOA_CLK_ENABLE();      // Enable Clock
    My_HAL_GPIO_Init(GPIOA, &initStr);  // Initialize pins
    PA0_Interrupt_Init();

    // GPIO-C
    My_HAL_RCC_GPIOC_CLK_ENABLE();      // Enable Clock
    My_HAL_GPIO_Init(GPIOC, &initStr);  // Initialize pins
    // PC4 and PC5 for USART3:
    GPIOC->MODER &= ~(0xF << (4 * 2));  // Clear Bits for PC4 and PC5
    GPIOC->MODER |= (0xA << (4 * 2));   // Set PC4 and PC5 to AF mode (10)
    // Select AF1 (USART3)
    GPIOC->AFR[0] &= ~(0xFF << 4 * 4);  // Clear AF bits for PC4 (4 bits) and PC5 (4 bits)
    GPIOC->AFR[0] |= (0x11 << 4 * 4);   // Set AF1 for PC4 and Pc5 in AF Register

    USART_Init();
    while (1) {
        USART3_TransmitString("\r\nCMD?\r\n");  // Print command prompt
        while (!USART3_IsDataAvailable());      // Wait for first character (LED color)
        char color = USART3_GetChar();          // Get 1st Char
        USART3_TransmitChar(color);             // Display the input as a feedback
        while (!USART3_IsDataAvailable());      // Wait for second character (Action)
        char action = USART3_GetChar();         // Get 2nd Char
        USART3_TransmitChar(action);            // Display the input as a feedback
        USART3_TransmitString(" => ");
        Process_Command(color, action);         // Process command
    }
}

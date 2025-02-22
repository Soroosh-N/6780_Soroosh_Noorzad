#include <stm32f0xx_hal.h>
#include <hal_gpio.h>
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
//  PB10 and PB11
void USART_Init(void) {
    // USART3
    My_HAL_RCC_USART_CLK_ENABLE();              // Enable clock
    uint32_t pclk = HAL_RCC_GetHCLKFreq();      // System clock frequency (Typically 48MHz for STM32F0)
    USART3->BRR = pclk / 115200;                // Set baud rate to 115200
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; // Enable Transmitter and Receiver
    // Enable USART3
    USART3->CR1 |= USART_CR1_UE;
}

void USART3_TransmitChar(char c) {
    while (!(USART3->ISR & USART_ISR_TXE));     // Wait until transmit data register is empty (TXE bit is set)
    USART3->TDR = c;                            // Write the character
}

void USART3_TransmitString(const char *str) {
    while (*str) {               // Loop until null character ('\0') is encountered
        USART3_TransmitChar(*str); // Transmit
        str++;
    }
}

char USART3_ReceiveChar(void) {
    while (!(USART3->ISR & USART_ISR_RXNE)); // Wait for RXNE (data received)
    return USART3->RDR; // Read received character
}

void PA0_Interrupt_Init(void) {
    // Enable SYSCFG clock (needed for external interrupts)
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
    // Connect EXTI0 line to PA0
    SYSCFG->EXTICR[0] &= ~(SYSCFG_EXTICR1_EXTI0);   // Ensure PA0 is mapped to EXTI0
    EXTI->FTSR |= EXTI_FTSR_TR0;                    // Falling edge trigger
    // Enable EXTI0 interrupt
    EXTI->IMR |= EXTI_IMR_IM0;                      // Unmask interrupt line 0
    NVIC_EnableIRQ(EXTI0_1_IRQn);                   // Enable EXTI0 interrupt in NVIC
    NVIC_SetPriority(EXTI0_1_IRQn, 3);              // Set priority
    NVIC_SetPriority(SysTick_IRQn, 0);              // Set Systick priority
}

void EXTI0_1_IRQHandler(void) {
    USART3_TransmitString("Button Pressed!\r\n");
    GPIOC->ODR &= ~(GPIO_PIN_6);
    for (uint32_t i = 0; i < 6; i++) {
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
        volatile uint32_t C = 0;
        while(C<500000){
            C += 1;
        }
    }
    EXTI->PR |= EXTI_PR_PR0; // Clear interrupt flag (write 1 to clear)
}

int lab4_main(void) {
    // Configure the system clock
    SystemClock_Config();
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
    GPIOC->AFR[0] |= (0x11 << 4 * 4); // Set AF1 for PC4 and Pc5 in AF Register

    USART_Init();
    while (1) {
        // Receiving Character
        char received = USART3_ReceiveChar(); // Get character from terminal
        switch (received) {
            case 'r':
                USART3_TransmitString("R => Red LED Toggled!\r\n");
                My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);   // Toggle Red LED (PC6)
                break;
            case 'g':
                USART3_TransmitString("G => Green LED Toggled!\r\n");
                My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);   // Toggle Green LED (PC9)
                break;
            case 'o':
                USART3_TransmitString("O => Orange LED Toggled!\r\n");
                My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);   // Toggle Blue LED (PC7)
                break;
            default:
                USART3_TransmitChar(received); // Transmit received char
                USART3_TransmitString(" is not a command!\r\n");
                GPIOC->ODR &= ~(GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9);
                for (uint32_t i = 0; i < 6; i++) {
                    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
                    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
                    My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
                    HAL_Delay(300);
                }
                break;
        }
    }
}

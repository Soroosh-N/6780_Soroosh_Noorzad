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

int lab4_main(void) {
    // Configure the system clock
    SystemClock_Config();    

    // GPIO-C
    My_HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef initStrGPC = {0};
    My_HAL_GPIO_Init(GPIOC, &initStrGPC);   // Initialize pins
    // PC4 and PC5:
    GPIOC->MODER &= ~(0xF << (4 * 2));  // Clear Bits for PC4 and PC5
    GPIOC->MODER |= (0xA << (4 * 2));   // Set PC4 and PC5 to AF mode (10)
    // Select AF1 (USART3)
    GPIOC->AFR[0] &= ~(0xFF << 4 * 4);  // Clear AF bits for PC4 (4 bits) and PC5 (4 bits)
    GPIOC->AFR[0] |= (0x11 << 4 * 4); // Set AF1 for PC4 and Pc5 in AF Register

    USART_Init();
    while (1) {
        // Sending Character:
        // Loop Method:
        USART3_TransmitChar('A'); // Transmit 'A'
        for (volatile int i = 0; i < 100000; i++); // Simple delay
        // // Button Push:
        // if (!(GPIOA->IDR & GPIO_IDR_0)) { // Check if button (PA0) is pressed
        //     USART3_TransmitChar('B'); // Send 'B'
        //     while (!(GPIOA->IDR & GPIO_IDR_0)); // Wait for button release
        // }

        // Sending String:
        // //Loop Method:
        // USART3_TransmitString("Soroosh Noorzad!\r\n"); // Send a message
        // for (volatile int i = 0; i < 1000000; i++);  // Delay to prevent flooding the terminal
        // // Button Push:
        // if (!(GPIOA->IDR & GPIO_IDR_0)) { // Check if button is pressed
        //     USART3_TransmitString("Button Pressed!\r\n");
        //     while (!(GPIOA->IDR & GPIO_IDR_0)); // Wait for button release
        // }
    }
}

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
// Recommended by TA: PB10 and PB11

// Global variables for interrupt handling
volatile char received_data = 0;
volatile uint8_t data_available = 0;

void USART_Init(void) {
    // USART3
    My_HAL_RCC_USART_CLK_ENABLE();              // Enable clock
    uint32_t pclk = HAL_RCC_GetHCLKFreq();      // System clock frequency (Typically 48MHz for STM32F0)
    USART3->BRR = pclk / 115200;                // Set baud rate to 115200
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; // Enable Transmitter and Receiver
    USART3->CR1 |= USART_CR1_RXNEIE;            // Enable RXNE (Receive Register Not Empty) interrupt
    USART3->CR1 |= USART_CR1_UE;                // Enable USART3
    NVIC_EnableIRQ(USART3_4_IRQn);              // Enable USART3 interrupt in NVIC
    NVIC_SetPriority(USART3_4_IRQn, 2);         // Set priority
}

// Interrupt handler for USART3
void USART3_4_IRQHandler(void) {
    received_data = USART3->RDR;    // Read received character (automatically clears RXNE flag)
    data_available = 1;             // Set flag indicating new data
}

// Non-blocking function to check if data is available
uint8_t USART3_IsDataAvailable(void) {
    return data_available;
}

// Function to get received character (clears flag)
char USART3_GetChar(void) {
    data_available = 0; // Clear flag
    return received_data;
}

void USART3_TransmitChar(char c) {
    while (!(USART3->ISR & USART_ISR_TXE));     // Wait until transmit data register is empty (TXE bit is set)
    USART3->TDR = c;                            // Write the character
}

void USART3_TransmitString(const char *str) {
    while (*str) {                  // Loop until null character ('\0') is encountered
        USART3_TransmitChar(*str);  // Transmit
        str++;
    }
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
}

void EXTI0_1_IRQHandler(void) {
    USART3_TransmitString("Button Pressed! Red LED will toggle 3 times.\r\n\r\nCMD?\r\n");
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    for (uint32_t i = 0; i < 6; i++) {
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
        volatile uint32_t C = 0;
        while(C<500000){
            C += 1;
        }
    }
    EXTI->PR |= EXTI_PR_PR0; // Clear interrupt flag (write 1 to clear)
}

// Function to control LEDs based on command
void Process_Command(char color, char action) {
    uint16_t led_pin = 0;
    
    // Identify LED color
    switch (color) {
        case 'r': led_pin = GPIO_PIN_6; break; // Red LED
        case 'g': led_pin = GPIO_PIN_9; break; // Green LED
        case 'o': led_pin = GPIO_PIN_8; break; // Orange LED
        default:
            USART3_TransmitChar(color); // Transmit received char
            USART3_TransmitString(" is not a valid LED **COLOR** command!\r\n");
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
            for (uint32_t i = 0; i < 6; i++) {
                My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9);
                HAL_Delay(300);
            }
            return;
    }

    // Perform action
    switch (action) {
        case '0': // Turn OFF
            My_HAL_GPIO_WritePin(GPIOC, led_pin, GPIO_PIN_RESET);
            USART3_TransmitString("Turned OFF\r\n");
            break;
        case '1': // Turn ON
            My_HAL_GPIO_WritePin(GPIOC, led_pin, GPIO_PIN_SET);
            USART3_TransmitString("Turned ON\r\n");
            break;
        case '2': // Toggle
            My_HAL_GPIO_TogglePin(GPIOC, led_pin);
            USART3_TransmitString("TOGGLED\r\n");
            break;
        default:
            USART3_TransmitChar(action); // Transmit received char
            USART3_TransmitString(" is not a valid LED **ACTION** command!\r\n");
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);
            for (uint32_t i = 0; i < 6; i++) {
                My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_8 | GPIO_PIN_9);
                HAL_Delay(300);
            }
            return;
    }
}

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

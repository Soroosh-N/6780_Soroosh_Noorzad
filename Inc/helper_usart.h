#include <stm32f0xx_hal.h>

void USART_Init(void);
void USART3_4_IRQHandler(void);
uint8_t USART3_IsDataAvailable(void);
char USART3_GetChar(void);
void USART3_TransmitChar(char c);
void USART3_TransmitString(const char *str);
void PA0_Interrupt_Init(void);
void EXTI0_1_IRQHandler(void);
void Process_Command(char color, char action);
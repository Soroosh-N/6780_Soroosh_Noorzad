#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>

void My_HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void My_HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin);
GPIO_PinState My_HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void My_HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void My_HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pins);

void My_HAL_RCC_GPIOA_CLK_ENABLE(void);
void My_HAL_RCC_GPIOB_CLK_ENABLE(void);
void My_HAL_RCC_GPIOC_CLK_ENABLE(void);
void My_HAL_RCC_TIMER2_ENABLE(void);
void My_HAL_RCC_TIMER3_ENABLE(void);
void My_HAL_RCC_USART_CLK_ENABLE(void);
void My_HAL_RCC_I2C2_CLK_ENABLE(void);
uint8_t My_HAL_I2C_Master_Transmit(uint8_t devAddr, uint8_t *pData, uint16_t size);
uint8_t My_HAL_I2C_Master_Receive(uint8_t devAddr, uint8_t *pData, uint16_t size);

#include <stm32f0xx_hal.h>
#include <hal_gpio.h>
#include <helper_usart.h>
#include <assert.h>
#include <main.h>

#include <stdio.h>
#include <stdint.h>

#define DAC_CHANNEL 1  // Using DAC_OUT1 (PA4)

void ADC_Init(void) {
    My_HAL_RCC_ADC_CLK_ENABLE();

    // Set PA0 to Analog Mode (ADC Input) (PA0 -> ADC_IN0)
    GPIOA->MODER |= (3 << (0 * 2));  // Analog Mode
    GPIOA->PUPDR &= ~(3 << (0 * 2)); // No Pull-up/down

    // ADC Calibration
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL); // Wait until calibration is done

    // Configure ADC
    ADC1->CFGR1 |= ADC_CFGR1_RES_1 | ADC_CFGR1_CONT;   // 8-bit resolution
    ADC1->CHSELR = ADC_CHSELR_CHSEL0; // Select ADC_IN0
    // ADC1->CFGR1 &= ~ADC_CFGR1_CONT;   // Enable Continuous Mode
    ADC1->CR |= ADC_CR_ADEN;          // Enable ADC

    while (!(ADC1->ISR & ADC_ISR_ADRDY)); // Wait for ADC ready

    ADC1->CR |= ADC_CR_ADSTART; // Start ADC Conversion
}

void DAC_Init(void) {
    //My_HAL_RCC_DAC_CLK_ENABLE();
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;

    // Set PA4 (DAC_OUT1) to Analog Mode
    GPIOA->MODER |= (3 << (4 * 2));  // Analog Mode
    GPIOA->PUPDR &= ~(3 << (4 * 2)); // No Pull-up/down
    DAC->CR |= ( 7 << 3 );
    // Enable DAC channel, software trigger mode
    DAC->CR |= DAC_CR_EN1;    // Enable DAC channel 1

    DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;  // Trigger DAC update
}

void GPIO_lab6_Init(void) {
    // Enable Clock
    My_HAL_RCC_GPIOA_CLK_ENABLE();      // GPIO-A
    My_HAL_RCC_GPIOB_CLK_ENABLE();      // GPIO-B
    My_HAL_RCC_GPIOC_CLK_ENABLE();      // GPIO-C
    GPIO_InitTypeDef initStr = {0};

    // GPIO-C
    My_HAL_GPIO_Init(GPIOC, &initStr);  // Initialize pins
    // Configure PC0 (SPI/I2C Mode)
    GPIOC->MODER &= ~(0xFF << (6 * 2)); // Clear MODER bits
    GPIOC->MODER |= (0x55 << (6 * 2));    // Output Mode for PC0-PC3

    // USART
    // PC4 and PC5 for USART3:
    GPIOC->MODER &= ~(0xF << (4 * 2));  // Clear Bits for PC4 and PC5
    GPIOC->MODER |= (0xA << (4 * 2));   // Set PC4 and PC5 to AF mode (10)
    // Select AF1 (USART3)
    GPIOC->AFR[0] &= ~(0xFF << 4 * 4);  // Clear AF bits for PC4 (4 bits) and PC5 (4 bits)
    GPIOC->AFR[0] |= (0x11 << 4 * 4);   // Set AF1 for PC4 and Pc5 in AF Register
}

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 4000; i++) __NOP();  // Approximate delay (assuming 8 MHz clock)
}

int lab6_main(void) {
    // Configure the system clock
    SystemClock_Config();
    NVIC_SetPriority(SysTick_IRQn, 0);  // Set Systick priority

    GPIO_lab6_Init();
    USART_Init();
    char str[60];

    // // Check off 1
    // ADC_Init();
    // uint8_t adc_val = (uint8_t)(ADC1->DR & 0xFF); // Read ADC 8-bit value
    // while (1) {
    //     adc_val = (uint8_t)(ADC1->DR & 0xFF); // Read ADC 8-bit value
    //     sprintf(str, "ADC Value: %d\r\n", adc_val);
    //     USART3_TransmitString(str);
    //     My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 0);
    //     My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 0);
    //     My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0);
    //     My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 0);
    //     if (adc_val > 50){
    //         My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
    //     };
    //     if (adc_val > 100){
    //         My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
    //     };
    //     if (adc_val > 150){
    //         My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
    //     };
    //     if (adc_val > 200){
    //         My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
    //     };
    //     HAL_Delay(100);
    // }

    // Check off 2
    DAC_Init();
    uint8_t i = 0;
    // Sine Wave: 8-bit, 32 samples per cycle
    uint8_t waveform1[32] = {127,151,175,197,216,232,244,251,254,251,244,232,216,197,175,151,127,102,78,56,37,21,9,2,0,2,9,21,37,56,78,102};
    // Triangle Wave: 8-bit, 32 samples/cycle
    uint8_t waveform2[32] = {0,15,31,47,63,79,95,111,127,142,158,174,190,206,222,238,254,238,222,206,190,174,158,142,127,111,95,79,63,47,31,15};

    while (1) {
        DAC->DHR8R1 = waveform1[i];  // Write value to DAC
        // sprintf(str, "Wave Value: %d\r\n", waveform1[i]);
        // USART3_TransmitString(str);
        if (i >= 31){
            i = 0;
        }else{
            i = i + 1;
        }
        delay_ms(1);       // 1ms delay → 31Hz waveform
        // HAL_Delay(1);
    }
}
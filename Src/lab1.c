#include <stm32f0xx_hal.h>
#include <hal_gpio.h>
#include <assert.h>
#include <main.h>

int lab1_main(void) {
    // Configure the system clock
    SystemClock_Config();

    // // ************ Check off 1 ************
    // /* Part A: This example uses HAL library calls to control the GPIOC peripheral. You’ll be redoing this code with hardware register access. */
    // // Reset of all peripherals, init the Flash and Systick
    // HAL_Init();
    // // Enable the GPIOC clock in the RCC
    // __HAL_RCC_GPIOC_CLK_ENABLE();
    // // Set up a configuration struct to pass to the initialization function
    // GPIO_InitTypeDef initStrGPC = {0};
    // // GPIO-C:
    // initStrGPC.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    // // GPIOx_OTYPER – Push-Pull Output Type
    // initStrGPC.Mode = GPIO_MODE_OUTPUT_PP;
    // // GPIOx_PUPDR – No Pull-Up or Pull-Down
    // initStrGPC.Pull = GPIO_NOPULL;
    // // GPIOx_OSPEEDR – Low Speed
    // initStrGPC.Speed = GPIO_SPEED_FREQ_LOW;
    // // Initialize pins
    // HAL_GPIO_Init(GPIOC, &initStrGPC);
    // // Optional delay to ensure the initialization is complete
    // HAL_Delay(1);
    // // Assertion on GPIOC (All the pins should have the 01 value => 01010101 = 55)
    // assert(GPIOC->MODER == 0x55000);
    // // GPIO-A: Configure PA0 as Digital Input (bits 0 and 1 should be 00)
    // initStrGPC.Pin = GPIO_PIN_0;
    // // USER Button (input):
    // initStrGPC.Mode = GPIO_MODE_INPUT;
    // // GPIOx_PUPDR – Pull-Down Resistor
    // initStrGPC.Pull = GPIO_PULLDOWN;
    // initStrGPC.Speed = GPIO_SPEED_FREQ_LOW;
    // HAL_GPIO_Init(GPIOA, &initStrGPC);
    // HAL_Delay(1);
    // // Assert that PA0 is configured as input (00)
    // assert((GPIOA->MODER & 0x03) == 0x00);
    // Start PC8 high
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    // while (1) {
    //     // Delay in ms
    //     HAL_Delay(200);
    //     // Toggle the output state of both PC8 and PC9
    //     HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
    // }

    // // ************ Check off 2 ************
    // My_HAL_RCC_GPIOC_CLK_ENABLE();
    // GPIO_InitTypeDef initStrGPC = {0};
    // // GPIO-C:
    // // Initialize pins
    // My_HAL_GPIO_Init(GPIOC, &initStrGPC);
    // HAL_Delay(1);
    // // Assertion on GPIOC (All the pins should have the 01 value => 01010101 = 55)
    // assert(GPIOC->MODER == 0x55000);
    // // GPIO-A: Configure PA0 as Digital Input (bits 0 and 1 should be 00)
    // My_HAL_GPIO_Init(GPIOA, &initStrGPC);
    // HAL_Delay(1);
    // // Assert that PA0 is configured as input (00)
    // assert((GPIOA->MODER & 0x03) == 0x00);
    // // LD3: I/O PC6 - Red LED
    // // LD4: I/O PC8 - Orange LED
    // // LD5: I/O PC9 - Green LED
    // // LD6: I/O PC7 - Blue LED
    // // Start PC6 high
    // My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
    // while (1) {
    //     HAL_Delay(300);
    //     // Toggle the output state of both PC6 and PC7
    //     My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_7);
    // }

    // ************ Check off 3 ************
    My_HAL_RCC_GPIOA_CLK_ENABLE();
    My_HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef initStrGPC = {0};
    // GPIO-C:
    // Initialize pins
    My_HAL_GPIO_Init(GPIOC, &initStrGPC);
    HAL_Delay(1);
    // Assertion on GPIOC (All the pins should have the 01 value => 01010101 = 55)
    assert(GPIOC->MODER == 0x55000);
    // GPIO-A: Configure PA0 as Digital Input (bits 0 and 1 should be 00)
    My_HAL_GPIO_Init(GPIOA, &initStrGPC);
    HAL_Delay(1);
    // Assert that PA0 is configured as input (00)
    assert((GPIOA->MODER & 0x03) == 0x00);
    // LD3: I/O PC6 - Red LED
    // LD4: I/O PC8 - Orange LED
    // LD5: I/O PC9 - Green LED
    // LD6: I/O PC7 - Blue LED
    // Start PC6 high
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
    uint32_t a = 0;
    uint32_t debouncer = 0;
    while (1) {
        // When BTN is bouncing the bit-vector value is random since bits are set when the BTN is high and not when it bounces low.
        // Always shift every loop iteration
        debouncer = (debouncer << 1);
        if (My_HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
            // Read the user BTN state
            // If BTN is set/high (BTN is pressed) set lowest bit of bit-vector
            debouncer |= 0x01;
        }
        if (debouncer == 0xFFFFFFFF) {
            // Triggers repeatedly when button is steady high!
        }
        if (debouncer == 0x00000000) {
            // Triggers repeatedly when button is steady low!
        }
        if (debouncer == 0x7FFFFFFF) {
            // Triggers only once when transitioning to steady high!
            // Toggle LED state
            a += 1;
            if (a == 1){
                My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_8);
            }else if(a == 2){
                My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_7);
            }else if(a == 3){
                My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7 | GPIO_PIN_9);
            }else{
                My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9 | GPIO_PIN_6);
                a = 0;
            }
        }
        HAL_Delay(5);
    }
}
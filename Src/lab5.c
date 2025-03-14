// I2C2->CR2 (Control Register 2)
// Peripherals Datasheet [Page 687 -> Register format]
// Bits     | Title         | Value
// 23:16    | NBYTES[7:0]   | 1
// 9:0      | SADD[9:0]     | 0x69
// NBYTES is 7 bits wide, not 8.
// In 7 bit address mode SADD[9], SADD[8] and SADD[0] are don't care.

// I2C2->CR1 (Control Register 1)
// Peripherals Datasheet [Page 684 -> Register format]

// I2C2->TXDR (Transmit data register)
// Peripherals Datasheet [Page 697 -> Register format]
// Bits     | Title         | Value
// 31:8     | Reserved      | 0
// 7:0      | TXDATA[7:0]   | 0x20

// TIMINGR
// must be configured when the I2C is disabled
// Peripherals Datasheet [Page 665 -> How to set] - [Page 691 -> Register format]
// Bits     | Title       | Value
// 31:28    | PRESC[3:0]  | 1
// 27:24    | Reserved    | 0
// 23:20    | SCLDEL[3:0] | 0x4
// 19:16    | SDADEL[3:0] | 0x2
// 15:8     | SCLH[7:0]   | 0xF
// 7:0      | SCLL[7:0]   | 0x13

// L3GD20 Datasheet:
// NAME       Addr Bit7 Bit6 Bit5 Bit4 Bit3 Bit2 Bit1 Bit0
// WHO_AM_I   0F   1    1    0    1    0    0    1    1      = D3
// CTRL_REG1  20   DR1  DR0  BW1  BW0  PD   Zen  Yen  Xen

#include <stm32f0xx_hal.h>
#include <hal_gpio.h>
#include <helper_usart.h>
#include <assert.h>
#include <main.h>

#include <stdio.h>
#include <stdint.h>

void GPIO_lab5_Init(void) {
    // Enable Clock
    My_HAL_RCC_GPIOA_CLK_ENABLE();      // GPIO-A
    My_HAL_RCC_GPIOB_CLK_ENABLE();      // GPIO-B
    My_HAL_RCC_GPIOC_CLK_ENABLE();      // GPIO-C
    GPIO_InitTypeDef initStr = {0};
    // GPIO-A
    My_HAL_GPIO_Init(GPIOA, &initStr);  // Initialize pins

    // USART
    PA0_Interrupt_Init();

    // GPIO-B
    // Configure PB11 (SDA) and PB13 (SCL) as Alternate Function Open-Drain
    GPIOB->MODER &= ~((0x3 << (11 * 2)) | (0x3 << (13 * 2)));               // Clear mode bits
    GPIOB->MODER |= ((0x2 << (11 * 2)) | (0x2 << (13 * 2)));                // Set Alternate Function (10)
    GPIOB->OTYPER |= (GPIO_OTYPER_OT_11 | GPIO_OTYPER_OT_13);               // Set Open-Drain Output Type
    GPIOB->OSPEEDR |= ((0x3 << (11 * 2)) | (0x3 << (13 * 2)));              // Set Speed to High 
    GPIOB->PUPDR &= ~((0x3 << (11 * 2)) | (0x3 << (13 * 2)));               // No Pull-up/Pull-down
    GPIOB->PUPDR |= ((0x1 << (11 * 2)) | (0x1 << (13 * 2)));                // Set Pull-up
    // Set Alternate Function to I2C2
    GPIOB->AFR[1] &= ~((0xF << ((11 - 8) * 4)) | (0xF << ((13 - 8) * 4))); // Clear AF bits
    GPIOB->AFR[1] |= ((0x1 << ((11 - 8) * 4)) | (0x5 << ((13 - 8) * 4)));  // Set [PB11:AF1 > I2C2_SDA], [PB13:AF5 > I2C2_SCL]

    // Configure PB14 (I2C Address Selection) as Output Push-Pull
    GPIOB->MODER &= ~(0x3 << (14 * 2));  // Clear mode bits
    GPIOB->MODER |= (0x1 << (14 * 2));   // Set as General Purpose Output (01)
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_14); // Clear bit to set push-pull
    GPIOB->OSPEEDR &= ~(0x3 << (14 * 2)); // Set as Low Speed (00)
    GPIOB->PUPDR &= ~(0x3 << (14 * 2));  // No pull-up, no pull-down
    My_HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);  // Set PB14 High

    // GPIO-C
    My_HAL_GPIO_Init(GPIOC, &initStr);  // Initialize pins
    // Configure PC0 (SPI/I2C Mode)
    GPIOC->MODER &= ~(0x3 << (0 * 2));  // Clear mode bits
    GPIOC->MODER |= (0x1 << (0 * 2));   // Set as General Purpose Output (01)
    GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_0); // Clear bit to set push-pull
    GPIOC->OSPEEDR &= ~(0x3 << (0 * 2)); // Set as Low Speed (00)
    GPIOC->PUPDR &= ~(0x3 << (0 * 2));  // No pull-up, no pull-down
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);  // Set PC0 High

    // USART
    // PC4 and PC5 for USART3:
    GPIOC->MODER &= ~(0xF << (4 * 2));  // Clear Bits for PC4 and PC5
    GPIOC->MODER |= (0xA << (4 * 2));   // Set PC4 and PC5 to AF mode (10)
    // Select AF1 (USART3)
    GPIOC->AFR[0] &= ~(0xFF << 4 * 4);  // Clear AF bits for PC4 (4 bits) and PC5 (4 bits)
    GPIOC->AFR[0] |= (0x11 << 4 * 4);   // Set AF1 for PC4 and Pc5 in AF Register
}

void I2C2_Init(void) {
    My_HAL_RCC_I2C2_CLK_ENABLE();       // Enable I2C2 Clock in RCC
    I2C2->CR1 &= ~I2C_CR1_PE;                           // Disable I2C to configure it
    I2C2->CR1 = 0x00000000;                             // Reset I2C peripheral control register | clean, disabled state
    I2C2->TIMINGR = 0x10420F13;                         // Timing for Standard-mode (100 kHz)
    I2C2->CR1 |= I2C_CR1_PE; 
}

uint8_t WRITE_over_I2C(uint8_t NBYTE, uint16_t ADDR, uint8_t DATA[]) {
    // Wait until I2C is not busy
    while (I2C2->ISR & I2C_ISR_BUSY);
    // Configure address and write operation
    I2C2->CR2 &= ~((0x7F << I2C_CR2_NBYTES_Pos) | (0x3FF << 0));    // Clear NBYTES & SADD
    I2C2->CR2 |= (NBYTE << I2C_CR2_NBYTES_Pos) | (ADDR << 1);       // bytes count & Slave addr
    I2C2->CR2 &= ~I2C_CR2_RD_WRN;           // RD_WRN (0 = Write)
    I2C2->CR2 |= I2C_CR2_START;             // START
    uint8_t i = 0;
    while (i < NBYTE){
        while (!(I2C2->ISR & I2C_ISR_TXIS));    // Wait for Transmit Interrupt Status flag
        I2C2->TXDR = DATA[i];               // Send Data
        i++;
    };
    while (!(I2C2->ISR & I2C_ISR_TC));      // Wait for transfer complete
    if (I2C2->ISR & I2C_ISR_NACKF) {        // Check for any errors
        return 0;  // Error occurred
    }
    I2C2->CR2 |= I2C_CR2_STOP;              // Stop condition
    HAL_Delay(1);
    return 1;
}

uint8_t READ_over_I2C(uint8_t NBYTE, uint16_t ADDR){
    // READ from slave
    // Clear NBYTES & SADD
    I2C2->CR2 &= ~((0x7F << 16) | (0x3FF << 0));    
    // Bytes Number | Slave addr (7bit addr: shift 1bit) | RD_WRN (1 = Read) | START
    I2C2->CR2 |= (NBYTE << 16) | (ADDR << 1) | I2C_CR2_RD_WRN | I2C_CR2_START;
    uint8_t received = 0;
    while (!(I2C2->ISR & (I2C_ISR_RXNE | I2C_ISR_NACKF))) // Wait for either RXNE or NACKF flag
    while (!(I2C2->ISR & I2C_ISR_TC));                    // Wait for Transfer Complete (TC) flag
    received = I2C2->RXDR;  // Read received data
    I2C2->CR2 |= I2C_CR2_STOP;              // Stop condition
    while (!(I2C2->ISR & I2C_ISR_STOPF));  // Wait for STOP flag to be set
    I2C2->ICR |= I2C_ICR_STOPCF;        // Clear STOP flag
    HAL_Delay(1);
    return received;
}

int lab5_main(void) {
    // Configure the system clock
    SystemClock_Config();
    NVIC_SetPriority(SysTick_IRQn, 0);  // Set Systick priority

    GPIO_lab5_Init();
    I2C2_Init();
    USART_Init();

    uint8_t sending_data[2];    // Buffer for Sending data

    uint16_t L3GD20_I2C_ADDR = 0x69;    // L3GD20 I2C Address
    uint8_t NBYTE = 1;                  // Number of Bytes
    uint8_t ack = 1;                    // Default: Acknowledgement received
    uint8_t WHO_AM_I_ADDR = 0x0F;       // WHO_AM_I Register
    uint8_t RECEIVED = 0;
    char str[60];

    sending_data[0] = WHO_AM_I_ADDR;
    ack = WRITE_over_I2C(NBYTE, L3GD20_I2C_ADDR, sending_data);
    if (ack == 0){while (1){My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);}}
    RECEIVED = READ_over_I2C(NBYTE, L3GD20_I2C_ADDR);
    sprintf(str, "WHO_AM_I_ADDR Address: 0x%x\r\n", WHO_AM_I_ADDR);
    USART3_TransmitString(str);
    sprintf(str, "WHO_AM_I_ADDR Value: 0x%x\r\n\r\n", RECEIVED);
    USART3_TransmitString(str);
    HAL_Delay(1000);

    // // CHECK OFF 1
    // // Communication to Receive WHO_AM_I
    // uint8_t WHO_AM_I_CORRECT = 0xD3;        // WHO_AM_I Register
    // if (RECEIVED == WHO_AM_I_CORRECT) {
    //     while (1) {// Success: GREEN LED Blinking
    //         My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
    //         HAL_Delay(300);
    //     }
    // }else{// Error: RED LED Blinking
    //     while (1) {
    //         My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
    //         HAL_Delay(300);
    //     }
    // }

    // // CHECK OFF 2
    // Communication with L3GD20 to Receive X & Y
    uint8_t CTRL_REG1_ADDR = 0x20; // Register address for CTRL_REG1
    uint8_t CTRL_REG1_CONF = 0x0B; // 0b00001011 => PD = 1 (Normal or Sleep), Y & X enable = 1
    uint8_t OUT_X_L = (0x28);   // X-axis LSB register
    uint8_t OUT_Y_L = (0x2A);   // Y-axis LSB register
    // OUT_X_L = (OUT_X_L | 0x80); // X-axis LSB + MSB
    // OUT_Y_L = (OUT_Y_L | 0x80); // Y-axis LSB + MSB
    uint8_t OUT_X_H = (0x29);   // X-axis MSB register
    uint8_t OUT_Y_H = (0x2B);   // Y-axis MSB register
    uint8_t X_data[2];          // Buffer for X-axis data
    uint8_t Y_data[2];          // Buffer for Y-axis data
    int16_t X_axis = 0;
    int16_t Y_axis = 0;
    int16_t x_dir = 0;
    int16_t y_dir = 0;
    uint8_t THRESHOLD = 0;

    // // Configuration of L3GD20:
    sending_data[0] = CTRL_REG1_ADDR;
    sending_data[1] = CTRL_REG1_CONF;
    ack = WRITE_over_I2C(2, L3GD20_I2C_ADDR, sending_data);
    while (1){
        // Request X
        sending_data[0] = OUT_X_L;
        ack = WRITE_over_I2C(NBYTE, L3GD20_I2C_ADDR, sending_data);
        X_data[0] = READ_over_I2C(NBYTE, L3GD20_I2C_ADDR); // Read LSB
        sending_data[0] = OUT_X_H;
        ack = WRITE_over_I2C(NBYTE, L3GD20_I2C_ADDR, sending_data);
        X_data[1] = READ_over_I2C(NBYTE, L3GD20_I2C_ADDR); // Read MSB
        // Combine X MSB and LSB
        X_axis = (int16_t)((X_data[1] << 8) | X_data[0]);
        x_dir += X_axis;
        
        // Request Y
        sending_data[0] = OUT_Y_L;
        ack = WRITE_over_I2C(NBYTE, L3GD20_I2C_ADDR, sending_data);
        Y_data[0] = READ_over_I2C(NBYTE, L3GD20_I2C_ADDR); // Read LSB
        sending_data[0] = OUT_Y_H;
        ack = WRITE_over_I2C(NBYTE, L3GD20_I2C_ADDR, sending_data);
        Y_data[1] = READ_over_I2C(NBYTE, L3GD20_I2C_ADDR); // Read MSB
        // Combine Y MSB and LSB
        Y_axis = (int16_t)((Y_data[1] << 8) | Y_data[0]);
        y_dir += Y_axis;

        sprintf(str, "X: %x + %x \t= 0x%-8x \t= %d\t--->\tX: %d\r\n", X_data[1], X_data[0], X_axis, X_axis, x_dir);
        USART3_TransmitString(str);
        sprintf(str, "Y: %x + %x \t= 0x%-8x \t= %d\t--->\ty: %d\r\n\r\n", Y_data[1], Y_data[0], Y_axis, Y_axis, y_dir);
        USART3_TransmitString(str);
        // GPIO_PIN_6 ==> R LED (Y > 0)
        // GPIO_PIN_7 ==> B LED (Y < 0)
        if ((int16_t)y_dir > THRESHOLD) {
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 1);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 0);
        } else if ((int16_t)y_dir < THRESHOLD) {
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 0);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, 1);
        }
        // GPIO_PIN_8 ==> O LED (X > 0)
        // GPIO_PIN_9 ==> G LED (X < 0)
        if ((int16_t)x_dir > THRESHOLD) {
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 0);
        } else if ((int16_t)x_dir < THRESHOLD) {
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0);
            My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 1);
        }
        HAL_Delay(100);
    }
}
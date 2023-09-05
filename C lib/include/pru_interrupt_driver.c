#include "pru_interrupt_driver.h"
#include <pru_cfg.h>

// Base address of the GPIO registers
#define GPIO0 0x44E07000

// PRU0 interrupt number
#define PRU0_INTERRUPT 19

// GPIO register offsets
#define GPIO_OE_OFFSET 0x134
#define GPIO_DATAOUT_OFFSET 0x13C

// LED GPIO pin number (P8_XX)
#define LED_GPIO_PIN 7 // Example, replace with the actual pin number

// Function to initialize PRU interrupt handling
void pru_interrupt_init() {
    // Enable the PRU0 interrupt in the system interrupt controller
    CT_INTC.SECR0 |= (1 << PRU0_INTERRUPT);
}

// Function to enable external interrupt on a specific GPIO pin
void pru_interrupt_enable(uint8_t gpio_pin) {
    // Configure the GPIO pin as an input
    // Write to GPIO OE register to set the corresponding bit to 0

    // Enable interrupt generation on rising edge
    // Write to GPIO Rising Edge Detection Enable register

    // Enable the specific interrupt number in the PRU INTC register
    CT_INTC.PRU0_EN |= (1 << PRU0_INTERRUPT);
}

// Function to disable external interrupt on a specific GPIO pin
void pru_interrupt_disable(uint8_t gpio_pin) {
    // Disable interrupt generation on the specific GPIO pin
    // Write to GPIO Rising Edge Detection Enable register

    // Disable the specific interrupt number in the PRU INTC register
    CT_INTC.PRU0_EN &= ~(1 << PRU0_INTERRUPT);
}

// Define the Interrupt Service Routine (ISR)
void pru_interrupt_isr() {
    // Turn on the LED on the specified GPIO pin
    // Write to GPIO Data Output register to set the corresponding bit to 1
    ((volatile uint32_t *)(GPIO0 + GPIO_DATAOUT_OFFSET)) |= (1 << LED_GPIO_PIN);
}

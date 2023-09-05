#ifndef PRU_INTERRUPT_DRIVER_H
#define PRU_INTERRUPT_DRIVER_H

#include <stdint.h>

// Initialize PRU interrupt handling
void pru_interrupt_init();

// Enable external interrupt on a specific GPIO pin
void pru_interrupt_enable(uint8_t gpio_pin);

// Disable external interrupt on a specific GPIO pin
void pru_interrupt_disable(uint8_t gpio_pin);

// Define the ISR (Interrupt Service Routine)
void pru_interrupt_isr();

#endif // PRU_INTERRUPT_DRIVER_H

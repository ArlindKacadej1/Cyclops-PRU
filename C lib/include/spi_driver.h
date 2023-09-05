/**
 * @file spi_driver.h
 * @brief PRU SPI driver using memory-mapped I/O.
 */

#ifndef SPI_DRIVER_H
#define SPI_DRIVER_H

/**
 * @brief Initialize the SPI driver.
 * @return 0 on success, non-zero on failure.
 */
int spi_init(void);

/**
 * @brief Deinitialize the SPI driver.
 */
void spi_deinit(void);

/**
 * @brief Write a single byte over SPI.
 * @param data The byte to be written.
 * @return 0 on success, non-zero error code on failure.
 */
int spi_write_byte(unsigned char data);

/**
 * @brief Read a single byte over SPI.
 * @param data Pointer to store the received byte.
 * @return 0 on success, non-zero error code on failure.
 */
int spi_read_byte(unsigned char *data);

/**
 * @brief Write multiple bytes over SPI.
 * @param data Pointer to the data array to be written.
 * @param length Number of bytes to write.
 * @return 0 on success, non-zero error code on failure.
 */
int spi_write_bytes(const unsigned char *data, unsigned int length);

/**
 * @brief Read multiple bytes over SPI.
 * @param data Pointer to store the received data.
 * @param length Number of bytes to read.
 * @return 0 on success, non-zero error code on failure.
 */
int spi_read_bytes(unsigned char *data, unsigned int length);

#endif /* SPI_DRIVER_H */

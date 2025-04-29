/**
 * @file pn532.h
 * @author Generated with Claude
 * @brief PN532 NFC/RFID Reader driver for STM32
 */

#ifndef PN532_H
#define PN532_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>

// PN532 I2C address
#define PN532_I2C_ADDRESS        (0x24)

// PN532 Commands
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_SAMCONFIGURATION      (0x14)
#define PN532_COMMAND_INLISTPASSIVETARGET   (0x4A)
#define PN532_COMMAND_INDATAEXCHANGE        (0x40)

// Mifare Classic Commands
#define MIFARE_CMD_AUTH_A                   (0x60)
#define MIFARE_CMD_AUTH_B                   (0x61)
#define MIFARE_CMD_READ                     (0x30)
#define MIFARE_CMD_WRITE                    (0xA0)

// Other constants
#define PN532_PREAMBLE                      (0x00)
#define PN532_STARTCODE1                    (0x00)
#define PN532_STARTCODE2                    (0xFF)
#define PN532_POSTAMBLE                     (0x00)

#define PN532_HOSTTOPN532                   (0xD4)
#define PN532_PN532TOHOST                   (0xD5)

#define PN532_ACK_WAIT_TIME                 (30)  // ms
#define PN532_DEFAULT_TIMEOUT               (1000) // ms

#define MIFARE_UID_MAX_LENGTH               (10)
#define MIFARE_KEY_LENGTH                   (6)

typedef struct {
    I2C_HandleTypeDef *i2c;
    GPIO_TypeDef *irq_port;
    uint16_t irq_pin;
    uint8_t uid[MIFARE_UID_MAX_LENGTH];
    uint8_t uid_length;
} PN532;

/**
 * @brief Initialize PN532 module
 * @param pn532 Pointer to PN532 structure
 * @param hi2c Pointer to I2C handle
 * @param irq_port IRQ GPIO port
 * @param irq_pin IRQ GPIO pin
 * @return true if successful, false otherwise
 */
bool pn532_init(PN532 *pn532, I2C_HandleTypeDef *hi2c, GPIO_TypeDef *irq_port, uint16_t irq_pin);

/**
 * @brief Get the firmware version of PN532
 * @param pn532 Pointer to PN532 structure
 * @return Firmware version or 0 if failed
 */
uint32_t pn532_get_firmware_version(PN532 *pn532);

/**
 * @brief Configure SAM (Secure Access Module)
 * @param pn532 Pointer to PN532 structure
 * @return true if successful, false otherwise
 */
bool pn532_sam_config(PN532 *pn532);

/**
 * @brief Scan for RFID/NFC card
 * @param pn532 Pointer to PN532 structure
 * @return true if card found, false otherwise
 */
bool pn532_read_passive_target(PN532 *pn532);

/**
 * @brief Get UID of detected card
 * @param pn532 Pointer to PN532 structure
 * @param uid Pointer to store UID
 * @param length Pointer to store UID length
 * @return true if successful, false otherwise
 */
bool pn532_get_uid(PN532 *pn532, uint8_t *uid, uint8_t *length);

/**
 * @brief Authenticate a block with key A
 * @param pn532 Pointer to PN532 structure
 * @param block_number Block number
 * @param key Key (6 bytes)
 * @return true if authenticated, false otherwise
 */
bool pn532_mifare_auth(PN532 *pn532, uint8_t block_number, uint8_t *key, uint8_t auth_type);

/**
 * @brief Read data from a block
 * @param pn532 Pointer to PN532 structure
 * @param block_number Block number
 * @param data Pointer to store data (16 bytes)
 * @return true if successful, false otherwise
 */
bool pn532_mifare_read_block(PN532 *pn532, uint8_t block_number, uint8_t *data);

/**
 * @brief Write data to a block
 * @param pn532 Pointer to PN532 structure
 * @param block_number Block number
 * @param data Pointer to data to write (16 bytes)
 * @return true if successful, false otherwise
 */
bool pn532_mifare_write_block(PN532 *pn532, uint8_t block_number, uint8_t *data);

#endif /* PN532_H */
/**
 * @file pn532.c
 * @author Generated with Claude
 * @brief PN532 NFC/RFID Reader driver for STM32
 */

#include "pn532.h"
#include <string.h>

// Private function prototypes
static bool pn532_read_ack(PN532 *pn532);
static bool pn532_write_command(PN532 *pn532, uint8_t *command, uint8_t command_length);
static bool pn532_read_response(PN532 *pn532, uint8_t *response, uint8_t *response_length, uint16_t timeout);
static uint8_t pn532_calculate_checksum(uint8_t *data, uint8_t length);

bool pn532_init(PN532 *pn532, I2C_HandleTypeDef *hi2c, GPIO_TypeDef *irq_port, uint16_t irq_pin) {
    pn532->i2c = hi2c;
    pn532->irq_port = irq_port;
    pn532->irq_pin = irq_pin;
    
    // Reset UID data
    memset(pn532->uid, 0, MIFARE_UID_MAX_LENGTH);
    pn532->uid_length = 0;
    
    // Delay for PN532 initialization
    HAL_Delay(100);
    
    // Check if we can communicate with PN532
    uint32_t firmware_version = pn532_get_firmware_version(pn532);
    if (firmware_version == 0) {
        return false;
    }
    
    // Configure SAM
    return pn532_sam_config(pn532);
}

uint32_t pn532_get_firmware_version(PN532 *pn532) {
    uint8_t command[] = {
        PN532_COMMAND_GETFIRMWAREVERSION
    };
    
    if (!pn532_write_command(pn532, command, sizeof(command))) {
        return 0;
    }
    
    uint8_t response[12];
    uint8_t response_length = sizeof(response);
    
    if (!pn532_read_response(pn532, response, &response_length, PN532_DEFAULT_TIMEOUT)) {
        return 0;
    }
    
    // Check if the response is correct
    if (response_length < 6 || response[0] != PN532_PN532TOHOST || response[1] != PN532_COMMAND_GETFIRMWAREVERSION + 1) {
        return 0;
    }
    
    // Extract firmware version
    uint32_t firmware_version = ((uint32_t)response[2] << 24) | 
                               ((uint32_t)response[3] << 16) | 
                               ((uint32_t)response[4] << 8) | 
                               response[5];
    
    return firmware_version;
}

bool pn532_sam_config(PN532 *pn532) {
    uint8_t command[] = {
        PN532_COMMAND_SAMCONFIGURATION,
        0x01,  // Normal mode
        0x14,  // Timeout: 50ms * 20 = 1 second
        0x01   // Use IRQ pin
    };
    
    if (!pn532_write_command(pn532, command, sizeof(command))) {
        return false;
    }
    
    uint8_t response[8];
    uint8_t response_length = sizeof(response);
    
    if (!pn532_read_response(pn532, response, &response_length, PN532_DEFAULT_TIMEOUT)) {
        return false;
    }
    
    // Check if the response is correct
    if (response_length < 3 || response[0] != PN532_PN532TOHOST || response[1] != PN532_COMMAND_SAMCONFIGURATION + 1 || response[2] != 0x00) {
        return false;
    }
    
    return true;
}

bool pn532_read_passive_target(PN532 *pn532) {
    // Command to detect ISO14443A cards
    uint8_t command[] = {
        PN532_COMMAND_INLISTPASSIVETARGET,
        0x01,  // Max 1 card
        0x00   // ISO14443A (Mifare)
    };
    
    if (!pn532_write_command(pn532, command, sizeof(command))) {
        return false;
    }
    
    uint8_t response[25];
    uint8_t response_length = sizeof(response);
    
    if (!pn532_read_response(pn532, response, &response_length, PN532_DEFAULT_TIMEOUT)) {
        return false;
    }
    
    // Check if the response is correct and a card was found
    if (response_length < 5 || response[0] != PN532_PN532TOHOST || response[1] != PN532_COMMAND_INLISTPASSIVETARGET + 1 || response[2] != 0x01) {
        return false;
    }
    
    // Extract UID length and UID
    pn532->uid_length = response[7];
    if (pn532->uid_length > MIFARE_UID_MAX_LENGTH) {
        pn532->uid_length = MIFARE_UID_MAX_LENGTH;
    }
    
    memcpy(pn532->uid, &response[8], pn532->uid_length);
    
    return true;
}

bool pn532_get_uid(PN532 *pn532, uint8_t *uid, uint8_t *length) {
    if (pn532->uid_length == 0) {
        return false;
    }
    
    *length = pn532->uid_length;
    memcpy(uid, pn532->uid, pn532->uid_length);
    
    return true;
}

bool pn532_mifare_auth(PN532 *pn532, uint8_t block_number, uint8_t *key, uint8_t auth_type) {
    // Ensure we have a card
    if (pn532->uid_length == 0) {
        return false;
    }
    
    // Authentication command
    uint8_t command[13 + pn532->uid_length];
    command[0] = PN532_COMMAND_INDATAEXCHANGE;
    command[1] = 0x01;  // Card number 1
    command[2] = auth_type;  // MIFARE_CMD_AUTH_A or MIFARE_CMD_AUTH_B
    command[3] = block_number;
    
    // Copy authentication key (6 bytes)
    memcpy(&command[4], key, 6);
    
    // Copy UID
    memcpy(&command[10], pn532->uid, pn532->uid_length);
    
    if (!pn532_write_command(pn532, command, 10 + pn532->uid_length)) {
        return false;
    }
    
    uint8_t response[8];
    uint8_t response_length = sizeof(response);
    
    if (!pn532_read_response(pn532, response, &response_length, PN532_DEFAULT_TIMEOUT)) {
        return false;
    }
    
    // Check if authentication was successful
    if (response_length < 3 || response[0] != PN532_PN532TOHOST || response[1] != PN532_COMMAND_INDATAEXCHANGE + 1 || response[2] != 0x00) {
        return false;
    }
    
    return true;
}

bool pn532_mifare_read_block(PN532 *pn532, uint8_t block_number, uint8_t *data) {
    uint8_t command[] = {
        PN532_COMMAND_INDATAEXCHANGE,
        0x01,  // Card number 1
        MIFARE_CMD_READ,
        block_number
    };
    
    if (!pn532_write_command(pn532, command, sizeof(command))) {
        return false;
    }
    
    uint8_t response[26];  // 3 header bytes + 16 data bytes + more
    uint8_t response_length = sizeof(response);
    
    if (!pn532_read_response(pn532, response, &response_length, PN532_DEFAULT_TIMEOUT)) {
        return false;
    }
    
    // Check if read was successful
    if (response_length < 19 || response[0] != PN532_PN532TOHOST || response[1] != PN532_COMMAND_INDATAEXCHANGE + 1 || response[2] != 0x00) {
        return false;
    }
    
    // Copy 16 bytes of data
    memcpy(data, &response[3], 16);
    
    return true;
}

bool pn532_mifare_write_block(PN532 *pn532, uint8_t block_number, uint8_t *data) {
    uint8_t command[20] = {
        PN532_COMMAND_INDATAEXCHANGE,
        0x01,  // Card number 1
        MIFARE_CMD_WRITE,
        block_number
    };
    
    // Copy 16 bytes of data
    memcpy(&command[4], data, 16);
    
    if (!pn532_write_command(pn532, command, sizeof(command))) {
        return false;
    }
    
    uint8_t response[8];
    uint8_t response_length = sizeof(response);
    
    if (!pn532_read_response(pn532, response, &response_length, PN532_DEFAULT_TIMEOUT)) {
        return false;
    }
    
    // Check if write was successful
    if (response_length < 3 || response[0] != PN532_PN532TOHOST || response[1] != PN532_COMMAND_INDATAEXCHANGE + 1 || response[2] != 0x00) {
        return false;
    }
    
    return true;
}

static bool pn532_read_ack(PN532 *pn532) {
    uint8_t ack_buf[6];
    HAL_StatusTypeDef status;
    uint8_t retries = 5;
    
    // Expected ACK packet
    const uint8_t PN532_ACK[6] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
    
    // Wait for ACK
    do {
        HAL_Delay(5);
        status = HAL_I2C_Master_Receive(pn532->i2c, PN532_I2C_ADDRESS << 1, ack_buf, 6, PN532_ACK_WAIT_TIME);
        retries--;
    } while (status != HAL_OK && retries > 0);
    
    if (status != HAL_OK) {
        return false;
    }
    
    // Check if the ACK matches
    return (memcmp(ack_buf, PN532_ACK, 6) == 0);
}

static bool pn532_write_command(PN532 *pn532, uint8_t *command, uint8_t command_length) {
    // Construct the command frame
    // Frame format: Preamble (1) + Start codes (2) + Length (1) + LCS (1) + TFI (1) + Command data + DCS (1) + Postamble (1)
    // Total length = command_length + 7
    
    uint8_t length = command_length + 1; // Add TFI
    uint8_t frame[command_length + 9];
    uint8_t checksum = 0;
    uint8_t i;
    
    frame[0] = PN532_PREAMBLE;
    frame[1] = PN532_STARTCODE1;
    frame[2] = PN532_STARTCODE2;
    frame[3] = length;
    frame[4] = ~length + 1; // LCS: Length checksum
    frame[5] = PN532_HOSTTOPN532; // TFI: Frame Identifier (Host to PN532)
    
    // Calculate data checksum
    checksum += PN532_HOSTTOPN532;
    for (i = 0; i < command_length; i++) {
        frame[6 + i] = command[i];
        checksum += command[i];
    }
    
    frame[6 + command_length] = ~checksum + 1; // DCS: Data checksum
    frame[7 + command_length] = PN532_POSTAMBLE;
    
    // Send the command frame
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(pn532->i2c, PN532_I2C_ADDRESS << 1, frame, command_length + 8, PN532_DEFAULT_TIMEOUT);
    if (status != HAL_OK) {
        return false;
    }
    
    // Wait for ACK
    return pn532_read_ack(pn532);
}

static bool pn532_read_response(PN532 *pn532, uint8_t *response, uint8_t *response_length, uint16_t timeout) {
    // Read response header
    uint8_t header[7];
    uint32_t start_time = HAL_GetTick();
    HAL_StatusTypeDef status;
    
    // Wait for IRQ pin to go low or timeout
    if (pn532->irq_port != NULL) {
        while (HAL_GPIO_ReadPin(pn532->irq_port, pn532->irq_pin) == GPIO_PIN_SET) {
            if ((HAL_GetTick() - start_time) > timeout) {
                return false; // Timeout
            }
            HAL_Delay(5);
        }
    } else {
        HAL_Delay(50); // No IRQ pin, just wait a bit
    }
    
    // Read the response header
    status = HAL_I2C_Master_Receive(pn532->i2c, PN532_I2C_ADDRESS << 1, header, 7, timeout);
    if (status != HAL_OK) {
        return false;
    }
    
    // Check header format
    if (header[0] != PN532_PREAMBLE || header[1] != PN532_STARTCODE1 || header[2] != PN532_STARTCODE2) {
        return false;
    }
    
    // Extract length
    uint8_t length = header[3];
    
    // Verify length checksum
    if (header[4] != ((~length) & 0xFF) + 1) {
        return false;
    }
    
    // Make sure we have enough space in the buffer
    if (length > *response_length) {
        length = *response_length;
    }
    
    // Read the actual data
    status = HAL_I2C_Master_Receive(pn532->i2c, PN532_I2C_ADDRESS << 1, response, length, timeout);
    if (status != HAL_OK) {
        return false;
    }
    
    // Update actual response length
    *response_length = length;
    
    // Read the checksum and postamble
    uint8_t trailer[2];
    status = HAL_I2C_Master_Receive(pn532->i2c, PN532_I2C_ADDRESS << 1, trailer, 2, timeout);
    if (status != HAL_OK) {
        return false;
    }
    
    // Verify data checksum (simple implementation)
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum += response[i];
    }
    
    if (trailer[0] != ((~checksum) & 0xFF) + 1) {
        return false;
    }
    
    // Verify postamble
    if (trailer[1] != PN532_POSTAMBLE) {
        return false;
    }
    
    return true;
}

static uint8_t pn532_calculate_checksum(uint8_t *data, uint8_t length) {
    uint8_t sum = 0;
    for (uint8_t i = 0; i < length; i++) {
        sum += data[i];
    }
    return ~sum + 1;
}
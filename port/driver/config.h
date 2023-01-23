/*
 * This file is part of the MicroPython for Monocle:
 *      https://github.com/Itsbrilliantlabs/monocle-micropython
 *
 * Authored by: Josuah Demangeon <me@josuah.net>
 *
 * ISC Licence
 *
 * Copyright © 2022 Brilliant Labs Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Configuratin file for the drivers.
 * GPIO pins used, I2C addresses, peripheral instances in use...
 */

#include "nrf_gpio.h"

// Bluetooth params

#define BLE_DEVICE_NAME "Monocle"

// Pin mapping

#define ECX336CN_CS_N_PIN 6                   // P0.06 SPI Slave Select pin for the display.
#define ECX336CN_XCLR_PIN 15                  // P0.15
#define FLASH_CS_N_PIN 4                      // P0.04 SPI Slave Select pin for the flash.
#define FPGA_CS_N_PIN 8                       // P0.08 SPI Slave Select pin for the FPGA.
#define FPGA_INT_PIN 5                        // P0.05 FPGA Interrupt pin, to hold low in init.
#define FPGA_MODE1_PIN FPGA_CS_N_PIN          // Alias
#define FPGA_RECONFIG_N_PIN FPGA_INT_PIN      // Alias
#define PMIC_TOUCH_I2C_SCL_PIN 17             // P0.17 I2C clock GPIO pin.
#define PMIC_TOUCH_I2C_SDA_PIN 13             // P0.13 I2C data GPIO pin.
#define CAMERA_I2C_SCL_PIN 18                 // P0.18 I2C clock GPIO pin.
#define CAMERA_I2C_SDA_PIN 16                 // P0.16 I2C data GPIO pin.
#define IQS620_TOUCH_RDY_PIN 2                // P0.02 Interrupt pin.
#define MAX77654_ADC_PIN NRF_SAADC_INPUT_AIN1 // P0.02/AIN0 = VBATT_MEAS
#define MAX77654_IRQ_PIN 14                   // P0.14
#define OV5640_PWDN_PIN 1                     // P0.01
#define OV5640_RESETB_N_PIN 0                 // P0.00
#define RESET_L_PIN 21                        // P0.21
#define SPI2_MISO_PIN 10                      // P0.10 SPI Master In Slave Out pin.
#define SPI2_MOSI_PIN 9                       // P0.09 SPI Master Out Slave In pin.
#define SPI2_SCK_PIN 7                        // P0.07 SPI Clock pin

// TIMER

// 0 is reserved for SoftDevice
#define TIMER_INSTANCE 2
#define TIMER_MAX_HANDLERS 2

// I2C addresses
#define IQS620_ADDR 0x44
#define PMIC_ADDRESS 0x48
#define OV5640_ADDR 0x3C

// SPI

#define SPI_MAX_XFER_LEN 255

// OV5640

#define OV5640_WIDTH 1280
#define OV5640_HEIGHT 720

// ECX664CN

#define ECX336CN_WIDTH 640
#define ECX336CN_HEIGHT 400

// LIGJOJPEG

#define JOJPEG_WIDTH_MAX OV5640_WIDTH

// LIGGFX

#define GFX_OBJ_NUM

// Driver declaration

#define DRIVER(name)                    \
    {                                   \
        static bool ready = false;      \
        if (ready)                      \
            return;                     \
        else                            \
            ready = true;               \
        PRINTF("DRIVER(%s)\r\n", name); \
    }

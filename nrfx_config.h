/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Glenn Ruben Bakke
 * Copyright (c) 2018 Ayke van Laethem
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef NRFX_CONFIG_H
#define NRFX_CONFIG_H

#include "py/mpconfig.h"
#include "nrf.h"

// Port specific defines
#ifndef NRFX_LOG_ENABLED
#define NRFX_LOG_ENABLED 0
#endif

#define NRFX_LOG_UART_DISABLED 1


// NRFX configurations

//#define GPIO_COUNT 1

#define NRFX_GPIOTE_ENABLED 1
#define NRFX_GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS 1
#define NRFX_GPIOTE_DEFAULT_CONFIG_IRQ_PRIORITY 6

#define NRFX_TEMP_ENABLED 1
#define NRFX_TEMP_DEFAULT_CONFIG_IRQ_PRIORITY 7

#define NRFX_UART_ENABLED 1
#define NRFX_UART0_ENABLED 1
#define NRFX_UART1_ENABLED 1

#define NRFX_TWI_ENABLED 1
#define NRFX_TWI_DEFAULT_CONFIG_IRQ_PRIORITY 7
#define NRFX_TWI0_ENABLED 0
#define NRFX_TWI1_ENABLED 1

#define NRFX_SPI_ENABLED 0
#define NRFX_SPI0_ENABLED 0
#define NRFX_SPI1_ENABLED 0
#define NRFX_SPI2_ENABLED 0
#define NRFX_SPI_MISO_PULL_CFG 1

#define NRFX_SPIM_ENABLED 1
#define NRFX_SPIM0_ENABLED 1
#define NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY 7
#define NRFX_SPIM_MISO_PULL_CFG 1

#define NRFX_RTC_ENABLED (MICROPY_PY_MACHINE_RTCOUNTER)
#define NRFX_RTC0_ENABLED 1
#define NRFX_RTC1_ENABLED 1
#define NRFX_RTC2_ENABLED 1

#define NRFX_TIMER_ENABLED (MICROPY_PY_MACHINE_TIMER)
#define NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY 7
#define NRFX_TIMER0_ENABLED 1
#define NRFX_TIMER1_ENABLED (!MICROPY_PY_MACHINE_SOFT_PWM)
#define NRFX_TIMER2_ENABLED 1
#define NRFX_TIMER3_ENABLED 1
#define NRFX_TIMER4_ENABLED 1


#define NRFX_PWM_ENABLED MICROPY_PY_MACHINE_HW_PWM
#define NRFX_PWM0_ENABLED 1
#define NRFX_PWM1_ENABLED 1
#define NRFX_PWM2_ENABLED 1
#define NRFX_PWM3_ENABLED 0

#define NRFX_NVMC_ENABLED 1

// Peripheral Resource Sharing
#define NRFX_PRS_BOX_0_ENABLED (NRFX_TWI_ENABLED && NRFX_TWI0_ENABLED && NRFX_SPI_ENABLED && NRFX_SPI0_ENABLED)
#define NRFX_PRS_BOX_1_ENABLED (NRFX_TWI_ENABLED && NRFX_TWI1_ENABLED && NRFX_SPI_ENABLED && NRFX_SPI1_ENABLED)
#define NRFX_PRS_BOX_2_ENABLED (NRFX_TWI_ENABLED && NRFX_TWI1_ENABLED && NRFX_SPI_ENABLED && NRFX_SPI1_ENABLED)

#define NRFX_PRS_ENABLED (NRFX_PRS_BOX_0_ENABLED || NRFX_PRS_BOX_1_ENABLED || NRFX_PRS_BOX_2_ENABLED)

#define NRFX_SAADC_ENABLED (MICROPY_PY_MACHINE_ADC)

#endif

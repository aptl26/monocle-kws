/*
 * Copyright (c) 2021 Brilliant Labs Limited
 * Licensed under the MIT License
 */

#include <assert.h>
#include "monocle_fpga.h"
#include "monocle_ov5640.h"
#include "monocle_spi.h"
#include "monocle_config.h"
#include "nrfx_systick.h"
#include "nrfx_log.h"

#define LOG NRFX_LOG_WARNING

/**
 * Write a byte to the FPGA over SPI using a bridge protocol.
 * @param addr The address of the FPGA to write to.
 * @param byte The byte to write on that address.
 */
void fpga_write_byte(uint8_t addr, uint8_t byte)
{
    // check that it is a valid register to write to
    assert(FPGA_REGISTER_IS_WRITABLE(addr));

    // select FPGA on SPI bus and write to it.
    spi_set_cs_pin(SPIM0_FPGA_CS_PIN);
    spi_write_byte(addr, byte);
    LOG("fpga_write_byte(addr=0x%x, byte=0x%x).", addr, byte);
}

/**
 * Read a byte to the FPGA over SPI using a bridge protocol.
 * @param addr The address of the FPGA to read from.
 * @return The value read at that address.
 */
uint8_t fpga_read_byte(uint8_t addr)
{
    uint8_t byte;

    // check that it is a valid register
    assert(FPGA_REGISTER_EXISTS(addr));

    // select FPGA on SPI bus
    spi_set_cs_pin(SPIM0_FPGA_CS_PIN);

    byte = spi_read_byte(addr);
    LOG("fpga_read_byte(addr=0x%x) returned 0x%x.", addr, byte);    
    return byte;
}

/**
 * Write a multi-byte burst of byte to the FPGA over SPI.
 * The address is implicit.
 * @param buf The buffer containing the data.
 * @param len The size of that buffer.
 */
void fpga_write_burst(const uint8_t *buf, uint16_t len)
{
    // prepare FPGA to receive the byte stream
    fpga_write_byte(FPGA_WR_BURST_SIZE_LO, (len & 0x00FF) >> 0);
    fpga_write_byte(FPGA_WR_BURST_SIZE_HI, (len & 0xFF00) >> 8);

    // select FPGA on SPI bus
    spi_set_cs_pin(SPIM0_FPGA_CS_PIN); 
    spi_write_burst(FPGA_BURST_WR_DATA, buf, len);
}

/**
 * Read a burst of data from the FPGA.
 * The address is implicit.
 * @param data The buffer that will be written to.
 * @param len The number of bytes to read onto that buffer.
 */
void fpga_read_burst_ref(uint8_t *data_buf, uint16_t data_len)
{
     do {
        size_t chunk_len = MIN(SPI_MAX_BURST_LENGTH, data_len);
        uint8_t *chunk_buf;

        // prepare FPGA for burst read
        fpga_write_byte(FPGA_RD_BURST_SIZE_LO, (chunk_len & 0x00FF) >> 0);
        fpga_write_byte(FPGA_RD_BURST_SIZE_HI, (chunk_len & 0xFF00) >> 8);

        // select FPGA on SPI bus
        spi_set_cs_pin(SPIM0_FPGA_CS_PIN);

        chunk_buf = spi_read_burst(FPGA_BURST_RD_DATA, data_len);
        memcpy(data_buf, chunk_buf, chunk_len);

        data_buf += chunk_len;
        data_len -= chunk_len;
    } while (data_len > 0);
}

uint8_t *fpga_read_burst(uint16_t len)
{
    // prepare FPGA for burst read
    fpga_write_byte(FPGA_RD_BURST_SIZE_LO, (len & 0x00FF) >> 0);
    fpga_write_byte(FPGA_RD_BURST_SIZE_HI, (len & 0xFF00) >> 8);

    // select FPGA on SPI bus
    spi_set_cs_pin(SPIM0_FPGA_CS_PIN);

    return spi_read_burst(FPGA_BURST_RD_DATA, len);
}

uint32_t fpga_get_capture_size(void)
{
    assert(fpga_capture_done());
    return (fpga_read_byte(FPGA_CAPTURE_SIZE_0) << 24
          | fpga_read_byte(FPGA_CAPTURE_SIZE_1) << 16
          | fpga_read_byte(FPGA_CAPTURE_SIZE_2) << 8
          | fpga_read_byte(FPGA_CAPTURE_SIZE_3) << 0);
}

uint32_t fpga_get_bytes_read(void)
{
    return (fpga_read_byte(FPGA_CAPT_BYTE_COUNT_0) << 24
          | fpga_read_byte(FPGA_CAPT_BYTE_COUNT_1) << 16
          | fpga_read_byte(FPGA_CAPT_BYTE_COUNT_2) << 8
          | fpga_read_byte(FPGA_CAPT_BYTE_COUNT_3) << 0);
}

uint16_t fpga_get_checksum(void)
{
    return (fpga_read_byte(FPGA_CAPT_FRM_CHECKSUM_0) << 8
          | fpga_read_byte(FPGA_CAPT_FRM_CHECKSUM_1) << 0);
}

bool fpga_is_buffer_at_start(void)
{
    return (fpga_read_byte(FPGA_CAPTURE_STATUS) & FPGA_START_OF_CAPT);
}

bool fpga_is_buffer_read_done(void)
{
    return (fpga_read_byte(FPGA_CAPTURE_STATUS)
            & (FPGA_VIDEO_CAPT_DONE | FPGA_AUDIO_CAPT_DONE | FPGA_FRAME_CAPT_DONE));
    //return (fpga_read_byte(FPGA_CAPTURE_STATUS) & (FPGA_AUDIO_CAPT_DONE));
}

// testing functions
bool fpga_test_reset(void)
{
    // currently this register does nothing, can be used for test
    fpga_write_byte(FPGA_MEMORY_CONTROL, 0xBB);
    if (0xBB != fpga_read_byte(FPGA_MEMORY_CONTROL))
        return (false);
    // this will turn on XCLK
    fpga_write_byte(FPGA_CAMERA_CONTROL, 0x01);
    if (0x01 != fpga_read_byte(FPGA_CAMERA_CONTROL))
        return (false);

    fpga_soft_reset();

    // check registers all have default values
    if (fpga_read_byte(FPGA_SYSTEM_CONTROL) != FPGA_SYSTEM_CONTROL_DEFAULT)
        return (false);
    if (fpga_read_byte(FPGA_DISPLAY_CONTROL) != FPGA_DISPLAY_CONTROL_DEFAULT)
        return (false);
    if (fpga_read_byte(FPGA_MEMORY_CONTROL) != FPGA_MEMORY_CONTROL_DEFAULT)
        return (false);
    if (fpga_read_byte(FPGA_CAMERA_CONTROL) != FPGA_CAMERA_CONTROL_DEFAULT)
        return (false);
    // moved this to memory self-test, since bit FPGA_MEM_INIT_DONE will not be set if memory initialization fails
    //if (FPGA_SYSTEM_STATUS_DEFAULT != fpga_read_byte(FPGA_SYSTEM_STATUS))
    //    return (false);
    return (true);
}

bool fpga_ram_check(void)
{
    //first check that memory initialization succeeded
    // NOTE as of 2021-10-28, FPGA IP only supports one memory chip, so on MK11 only uses U10
    // run memory self-test
    // TODO: requires FPGA code support
    return (fpga_read_byte(FPGA_SYSTEM_STATUS) == FPGA_SYSTEM_STATUS_DEFAULT);
}

bool fpga_spi_exercise_register(uint8_t addr)
{
    // check that it is a valid register
    assert(FPGA_REGISTER_EXISTS(addr));

    // select FPGA on SPI bus
    spi_set_cs_pin(SPIM0_FPGA_CS_PIN);

    // run SPI exercise, return TRUE if successful
    return (spi_exercise_register(addr));
}

/**
 * Preparations for GPIO pins before to power-on the FPGA.
 */
void fpga_prepare(void)
{
    // Make sure the interrupt pin is not pulled low which could prevent
    // the FPGA from starting and cause damage to the flash.
    //
    // There is a weak pull-up on this line, and it can be used as
    // GPIO output for the FPGA, input for the MCU.
    nrf_gpio_pin_set(FPGA_RECONFIG_N_PIN);
    nrf_gpio_cfg_output(FPGA_RECONFIG_N_PIN);

    // MODE1 set low for AUTOBOOT from FPGA internal flash
    nrf_gpio_pin_clear(FPGA_MODE1_PIN);
    nrf_gpio_cfg_output(FPGA_MODE1_PIN);
}

/**
 * Initial configuration of the registers of the FPGA.
 */
void fpga_init(void)
{
    // Give the FPGA some time to boot.
    nrfx_systick_delay_ms(1000);

    // Set all registers to a known state.
    fpga_soft_reset();

    // Give the FPGA some further time.
    nrfx_systick_delay_ms(500);
}

void fpga_deinit(void)
{
    nrf_gpio_cfg_default(FPGA_MODE1_PIN);
    nrf_gpio_cfg_default(FPGA_RECONFIG_N_PIN);
}

void fpga_soft_reset(void)
{
    // reset FPGA
    fpga_write_byte(FPGA_SYSTEM_CONTROL, 0x01);

    // TODO: not sure if we need this, but just in case...
    nrfx_systick_delay_ms(185);

    // clear the reset (needed for some FPGA projects, like OLED unit test)
    fpga_write_byte(FPGA_SYSTEM_CONTROL, 0x00);

    // from testing, 2ms seems to be the minimum delay needed for all registers to return to expected values
    // reason is unclear
    // use 5ms for extra safety margin (used to work earlier)
    nrfx_systick_delay_ms(5);

    // NOTE: from 2021-02-19, 5ms is no longer enough
    // TODO: why? Seems to require 170ms delay now
    nrfx_systick_delay_ms(185);
}

bool fpga_xclk_on(void)
{
    fpga_write_byte(FPGA_CAMERA_CONTROL, FPGA_EN_XCLK);

    // TODO: is this still needed?
    nrfx_systick_delay_ms(300);

    return (fpga_read_byte(FPGA_CAMERA_CONTROL) == FPGA_EN_XCLK);
}

/**
 * Turn on the camera.
 * @return True if it has been effectively turned on.
 */
bool fpga_camera_on(void)
{
    // delay 4 frames to discard AWB adjustments (needed if camera was just powered up)
    nrfx_systick_delay_ms(4*(1000/OV5640_FPS) + 1);

    // enable camera interface (& keep XCLK enabled!)
    fpga_write_byte(FPGA_CAMERA_CONTROL, (FPGA_EN_XCLK | FPGA_EN_CAM));

    LOG("fpga_camera_on() waited 4 frames, sent FPGA_EN_XCLK, FPGA_EN_CAM");
    return (fpga_read_byte(FPGA_CAMERA_CONTROL) == (FPGA_EN_XCLK | FPGA_EN_CAM));
}

/**
 * Turn off the camera.
 * @return True if it has been effectively turned off.
 */
bool fpga_camera_off(void)
{
    // turn off camera interface (but keep XCLK enabled!)
    fpga_write_byte(FPGA_CAMERA_CONTROL, FPGA_EN_XCLK);

    // allow last frame to finish entering video buffer to avoid split screen
    nrfx_systick_delay_ms(1*(1000/OV5640_FPS) + 1);

    LOG("fpga_camera_off() sent FPGA_EN_XCLK, waited 1 frame");
    return (fpga_read_byte(FPGA_CAMERA_CONTROL) == FPGA_EN_XCLK);
}

/**
 * Turn on the microphone.
 * @return True if it has been effectively turned on.
 */
bool fpga_mic_on(void)
{
    fpga_write_byte(FPGA_MIC_CONTROL, FPGA_EN_MIC);
    return (fpga_read_byte(FPGA_MIC_CONTROL) == FPGA_EN_MIC);
}

/**
 * Turn off the microphone.
 * @return True if it has been effectively turned off.
 */
bool fpga_mic_off(void)
{
    fpga_write_byte(FPGA_MIC_CONTROL, 0x00);
    return (fpga_read_byte(FPGA_MIC_CONTROL) == 0x00);
}

/**
 * Enable the live streaming of the camera directly to the screen.
 */
void fpga_disp_live(void)
{
    fpga_write_byte(FPGA_DISPLAY_CONTROL, FPGA_DISP_CAM);
}

/**
 * Enable the busy indicator: gray screen.
 */
void fpga_disp_busy(void)
{
    fpga_write_byte(FPGA_DISPLAY_CONTROL, FPGA_DISP_BUSY);
}

/**
 * Enable the display of 8 vertical color bars to the screen.
 */
void fpga_disp_bars(void)
{
    fpga_write_byte(FPGA_DISPLAY_CONTROL, FPGA_DISP_BARS);
}

#ifndef FPGA_RELEASE_20210709 // TODO: Remove this?
/**
 * Enable the red-blue shift chrominance correction.
 */
void fpga_disp_rb_shift(bool on)
{
    uint8_t reg = fpga_read_byte(FPGA_DISPLAY_CONTROL);
    reg = on ? (reg | FPGA_EN_RB_SHIFT) : (reg & ~FPGA_EN_RB_SHIFT);
    fpga_write_byte(FPGA_DISPLAY_CONTROL, reg);
}
#endif

/**
 * Disable the red-blue shift chrominace correction.
 */
void fpga_disp_off(void)
{
    fpga_write_byte(FPGA_DISPLAY_CONTROL, FPGA_DISP_OFF);
}

/**
 * Resume live video feed and clear the checksum.
 */
void fpga_resume_live_video(void)
{
    //fpga_write_byte(FPGA_CAPTURE_CONTROL, FPGA_RESUME_FILL);
    fpga_write_byte(FPGA_CAPTURE_CONTROL, (FPGA_RESUME_FILL | FPGA_CLR_CHKSM));
}

/**
 * Enable the capture of a frame of the video.
 */
void fpga_image_capture(void)
{
    fpga_write_byte(FPGA_CAPTURE_CONTROL, (FPGA_CAPT_EN | FPGA_CAPT_FRM));
}

void fpga_video_capture(void)
{
#ifdef MIC_ON
    fpga_write_byte(FPGA_CAPTURE_CONTROL, (FPGA_CAPT_EN | FPGA_CAPT_VIDEO | FPGA_CAPT_AUDIO));
#else
    fpga_write_byte(FPGA_CAPTURE_CONTROL, (FPGA_CAPT_EN | FPGA_CAPT_VIDEO));
#endif
}

// first clear checksum, then set bit for audio data burst read
void fpga_prep_read_audio(void)
{
#ifdef MIC_ON
    // ensure CLR_CHECKSUM gets a rising edge
    fpga_write_byte(FPGA_CAPTURE_CONTROL, 0x00);

    // clear checksum (left from video transfer)
    fpga_write_byte(FPGA_CAPTURE_CONTROL, FPGA_CLR_CHKSM);

    // this also sets CLR_CHKSM back to 0
    fpga_write_byte(FPGA_CAPTURE_CONTROL, FPGA_RD_AUDIO);

    // this also sets CLR_CHKSM back to 0
    //fpga_write_byte(FPGA_CAPTURE_CONTROL, (FPGA_CAPT_EN | FPGA_CAPT_VIDEO | FPGA_CAPT_AUDIO));
#else
    // do nothing
#endif
}

void fpga_replay_rate(uint8_t repeat)
{
    // cannot be zero, max 5 bits
    assert(repeat > 0);
    assert(repeat <= FPGA_REP_RATE_MASK);

    fpga_write_byte(FPGA_REPLAY_RATE_CONTROL, repeat);
    //LOG("FPGA replay rate set to %d", repeat);
}

bool fpga_capture_done(void)
{
    return (fpga_read_byte(FPGA_CAPTURE_STATUS) & FPGA_CAPT_RD_VLD);
}

// Simple checksum calculation; matching the algorithm used on FPGA
// Sums a byte stream as 16-bit words (with first byte being least significant), adding carry back in, to return 16-bit result
// Precondition: len must be a multiple of 2 (bytes)
uint16_t fpga_calc_checksum(uint8_t *bytearray, uint32_t len)
{

    uint32_t checksum = 0;

    assert(bytearray != NULL);
    assert(len == 0);
    assert((len % 2) == 0);
    for (uint32_t i = 0; i < len; i = i + 2)
        checksum = fpga_checksum_add(checksum, ((bytearray[i + 1] << 8) + bytearray[i]));
    return ((uint16_t)checksum);
}

uint16_t fpga_checksum_add(uint16_t checksum1, uint16_t checksum2)
{
    uint32_t checksum = 0;
    uint32_t carry = 0;

    // add carry (which if it exists must be 1) back in
    checksum = checksum1 + checksum2;

    if (checksum > 0x0000FFFF)
    {
        carry = checksum & 0xFFFF0000;
        carry = carry >> 16;

        // this should always be true, if so we can simplify the above
        assert(carry == 1);

        checksum = (checksum & 0x0000FFFF) + carry;
        assert(checksum <= 0x0000FFFF);
    }
    return ((uint16_t)checksum);
}

// valid input zoom levels: 1, 2, 4, 8
void fpga_set_zoom(uint8_t level)
{
    uint8_t zoom_bits = 0;
    //NOTE For zoom to work, it should always be true that XCLK is on, EN_CAM is on
    // TODO: make en_luma_cor independently settable; for now always on
    switch(level)
    {
        case 1:
            zoom_bits = 0 << FPGA_ZOOM_SHIFT;
            fpga_write_byte(FPGA_CAMERA_CONTROL, FPGA_EN_XCLK | FPGA_EN_CAM | zoom_bits);
            break;
        case 2:
        case 4:
        case 8:
            zoom_bits = (level >> 2) << FPGA_ZOOM_SHIFT;
            fpga_write_byte(FPGA_CAMERA_CONTROL, FPGA_EN_XCLK | FPGA_EN_CAM | zoom_bits | FPGA_EN_ZOOM | FPGA_EN_LUMA_COR);
            break;
        default:
            assert(!"FPGA Zoom invalid zoom level.");
    }
}

/**
 * Turn the luma correction of the camera on or off asking the FPGA over SPI.
 * Convenient for testing.
 * @param turn_on Whether to activate or disable the luma correction.
 */
void fpga_set_luma(bool turn_on)
{
    uint8_t reg =0;
    bool luma_on = false;

    // get current zoom & luma status
    reg = fpga_read_byte(FPGA_CAMERA_CONTROL);

    // only valid when zoom is active
    if (!(reg & FPGA_EN_ZOOM))
        return;

    // already correctly set, nothing to do
    luma_on = (reg & FPGA_EN_LUMA_COR);
    if ((luma_on && turn_on) || (!luma_on && !turn_on))
        return;

    if (turn_on) {
        reg = reg | FPGA_EN_LUMA_COR;
        LOG("FPGA turn luma correction on.");
    } else {
        reg = reg & ~FPGA_EN_LUMA_COR;
        LOG("FPGA turn luma correction off.");
    }
    fpga_write_byte(FPGA_CAMERA_CONTROL, reg);
}

/**
 * Set the display mode of the screen controlled by FPGA over SPI.
 * @param mode 0=off, 1=disp_cam, 2=disp_busy, 3=disp_bars.
 */
// TODO: Convert mode to enum.
void fpga_set_display(uint8_t mode)
{
    switch(mode) {
        case 0:
            fpga_disp_off();
            LOG("FPGA display mode = off.");
            break;
        case 1:
            fpga_disp_live();
            LOG("FPGA display mode = video.");
            break;
        case 2:
            fpga_disp_busy();
            LOG("FPGA display mode = busy.");
            break;
        case 3:
            fpga_disp_bars();
            LOG("FPGA display mode = color bars.");
            break;
        default:
            assert(!"FPGA display mode invalid.");
    }
}

/**
 * Read the version register of the FPGA.
 * @param major Major revision number.
 * @param minor Minor revision number.
 */
void fpga_get_version(uint8_t *major, uint8_t *minor)
{
    *major = fpga_read_byte(FPGA_VERSION_MAJOR);
    *minor = fpga_read_byte(FPGA_VERSION_MINOR);
    LOG("%d.%d", *major, *minor);
}

/**
 * Inform the FPGA to discard the oldest capture buffer.
 * For now, the FPGA only supports one buffer, and it is discarded when a new capture is made.
 */
void fpga_discard_buffer(void)
{
    // TODO:
}

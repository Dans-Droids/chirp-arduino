/**-----------------------------------------------------------------------------
 *
 *  Example of using the Chirp SDK with an ESP32 and the SPH0645 microphone
 *
 *  @file receive.ino
 *
 *  @brief After creating a developer account on https://developers.chirp.io,
 *  get your key, secret and config string from your account using the "arduino"
 *  protocol, and set them in this file (in APP_KEY, APP_SECRET, APP_CONFIG).
 *
 *  The example will print to the terminal any chirp messages it receives.
 *
 *  Copyright © 2011-2018, Asio Ltd.
 *  All rights reserved.
 *
 *----------------------------------------------------------------------------*/
#include <driver/i2s.h>

#include <chirp_connect_errors.h>
#include <chirp_connect_callbacks.h>
#include <chirp_connect.h>
#include <chirp_connect_states.h>
#include <chirp_sdk_defines.h>

#include "credentials.h"

#define I2SI_DATA         32     // I2S data on GPIO32
#define I2SI_BCK          14     // I2S clk on GPIO14
#define I2SI_LRCL         15     // I2S select on GPIO15

#define BUFFER_SIZE       1024
#define MIC_CALIBRATION   13625

#define APP_KEY           "CHIRP_APP_KEY_HERE"
#define APP_SECRET        "CHIRP_APP_SECRET_HERE"
#define APP_CONFIG        "CHIRP_APP_ARDUINO_CONFIG_HERE"

/**
 * Convert I2S data.
 * Data is 18 bit signed, MSBit first, two's complement.
 * The calibration value is determined using the Serial
 * Plotter to centre the audio about zero.
 */
#define convert(sample) (((int32_t)(sample) >> 14) + MIC_CALIBRATION)

// Global variables -------------------------------------------
chirp_connect_t *connect = NULL;

// Function definitions ---------------------------------------
void setupChirp();
void setupAudio(int sample_rate);
void chirpErrorHandler(chirp_connect_error_code_t code);

// Function declarations --------------------------------------
void
onStateChangedCallback(void *connect, chirp_connect_state_t previous, chirp_connect_state_t current)
{
  Serial.printf("State changed from %d to %d\n", previous, current);
}

void
onReceivingCallback(void *connect, uint8_t *payload, size_t length, uint8_t channel)
{
  Serial.println("Receiving data...");
}

void
onReceivedCallback(void *connect, uint8_t *payload, size_t length, uint8_t channel)
{
  if (payload) {
    char *hexString = chirp_connect_as_string((chirp_connect_t *)connect, payload, length);
    Serial.printf("Received %s\n", hexString);
  } else{
    Serial.println("Decode failed.");
  }
}

void
setup()
{
  Serial.begin(115200);

  setupChirp();
  uint32_t sample_rate = chirp_connect_get_input_sample_rate(connect);

  setupAudio((int)sample_rate);
}

void
loop()
{
  esp_err_t audioError;
  chirp_connect_error_code_t chirpError;

  size_t bytesLength = 0;
  float buffer[BUFFER_SIZE] = {0};
  int32_t ibuffer[BUFFER_SIZE] = {0};

  audioError = i2s_read(I2S_NUM_0, ibuffer, BUFFER_SIZE * 4, &bytesLength, portMAX_DELAY);
  if (bytesLength) {
    for (int i = 0; i < bytesLength / 4; i++) {
      buffer[i] = (float)convert(ibuffer[i]);
    }
    chirpError = chirp_connect_process_input(connect, buffer, bytesLength / 4);
    if (chirpError != CHIRP_CONNECT_OK)
      chirpErrorHandler(chirpError);
  }
}

/**
 * Initialise Chirp and start running.
 */
void
setupChirp()
{
  connect = new_chirp_connect(APP_KEY, APP_SECRET);
  if (connect == NULL) {
    Serial.println("Chirp initialisation failed.");
    return;
  }

  chirp_connect_error_code_t err = chirp_connect_set_config(connect, APP_CONFIG);
  if (err != CHIRP_CONNECT_OK)
    chirpErrorHandler(err);

  chirp_connect_callback_set_t callbacks = {0};
  callbacks.on_sending = NULL;
  callbacks.on_sent = NULL;
  callbacks.on_state_changed = onStateChangedCallback;
  callbacks.on_receiving = onReceivingCallback;
  callbacks.on_received = onReceivedCallback;
  err = chirp_connect_set_callbacks(connect, callbacks);
  if (err != CHIRP_CONNECT_OK)
    chirpErrorHandler(err);

  err = chirp_connect_set_callback_ptr(connect, connect);
  if (err != CHIRP_CONNECT_OK)
    chirpErrorHandler(err);

  err = chirp_connect_start(connect);
  if (err != CHIRP_CONNECT_OK)
    chirpErrorHandler(err);

  Serial.println("Chirp Connect initialised.");
}

/**
 * Set up I2S audio for SPH0645 microphone
 */
void
setupAudio(int sample_rate)
{
  esp_err_t err;
  Serial.println("Initialising audio driver..");

  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = sample_rate,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 4,
      .dma_buf_len = 128,
      .use_apll = true
  };

  const i2s_pin_config_t pin_config = {
      .bck_io_num = I2SI_BCK,
      .ws_io_num = I2SI_LRCL,
      .data_out_num = I2S_PIN_NO_CHANGE,
      .data_in_num = I2SI_DATA
  };

  err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("Failed installing driver: %d\n", err);
    while (true);
  }

  err = i2s_set_pin(I2S_NUM_0, &pin_config);
  if (err != ESP_OK) {
    Serial.printf("Failed setting pin: %d\n", err);
    while (true);
  }

  err = i2s_set_sample_rates(I2S_NUM_0, sample_rate);
  if (err != ESP_OK) {
    Serial.printf("Failed to set sample rates: %d\n", err);
    while (true);
  }

  Serial.println("Audio driver initalised.");
}

void chirpErrorHandler(chirp_connect_error_code_t code)
{
  if (code != CHIRP_CONNECT_OK)
  {
    const char *error_string = chirp_connect_error_code_to_string(code);
    Serial.printf("Chirp error handler : %s\n", error_string);
    chirp_connect_free((void *) error_string);
    while(true) {
      delay(1000);
      Serial.print('.');
    }
  }
}

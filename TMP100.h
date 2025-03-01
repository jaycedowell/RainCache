#ifndef _TMP100_H
#define _TMP100_H

#include "Arduino.h"
#include <Adafruit_I2CDevice.h>

#define TMP100_I2CADDR_DEFAULT 0x48
#define TMP100_READ_REG 0x00
#define TMP100_CONFIG_REG 0x01
#define TMP100_CONV_DELAY_9BIT 40
#define TMP100_CONV_DELAY_10BIT 80
#define TMP100_CONV_DELAY_11BIT 160
#define TMP100_CONV_DELAY_12BIT 320


class TMP100 {
  protected:
    int _delay = 0;
    TwoWire *i2c_dev = NULL; ///< Pointer to I2C bus interface

  public:
    TMP100();
    ~TMP100();

    bool begin(TwoWire *wire = &Wire);
    bool isDeviceReady(void);

    bool setResolution(int nbit);
    int getResolution(void);

    float getTemp(void);
};

#endif

#include "Arduino.h"
#include "TMP100.h"

TMP100::TMP100(void) {}
TMP100::~TMP100(void) {}

bool TMP100::begin(TwoWire *wire) {
  if( i2c_dev ) {
    delete i2c_dev;
  }

  i2c_dev = wire;
  i2c_dev->beginTransmission(TMP100_I2CADDR_DEFAULT);
  byte error = i2c_dev->endTransmission();

  if( error != 0 ) {
    return false;
  }

  return true;
}

bool TMP100::isDeviceReady() {
  if( !i2c_dev || _delay == 0 ) {
    return false;
  }

  i2c_dev->beginTransmission(TMP100_I2CADDR_DEFAULT);
  byte error = i2c_dev->endTransmission();
  return (error == 0);
}

bool TMP100::setResolution(int nbit) {
  if( !i2c_dev || nbit < 9 || nbit > 12 ) {
    return false;
  }

  switch(nbit) {
    case 9:  _delay = TMP100_CONV_DELAY_9BIT; break;
    case 10: _delay = TMP100_CONV_DELAY_10BIT; break;
    case 11: _delay = TMP100_CONV_DELAY_11BIT; break;
    default: _delay = TMP100_CONV_DELAY_12BIT;
  }
  
  i2c_dev->beginTransmission(TMP100_I2CADDR_DEFAULT);
  i2c_dev->write(TMP100_CONFIG_REG);
  i2c_dev->write(((unsigned char) (nbit - 9)) << 5);
  byte error = i2c_dev->endTransmission();

  return (error == 0);
}

int TMP100::getResolution() {
  if( !i2c_dev || _delay == 0 ) {
    return 0;
  }

  switch(_delay) {
    case TMP100_CONV_DELAY_9BIT: return 9;
    case TMP100_CONV_DELAY_10BIT: return 10;
    case TMP100_CONV_DELAY_11BIT: return 11;
    case TMP100_CONV_DELAY_12BIT: return 12;
    default: return 0;
  }

  return 0;
}

float TMP100::getTemp() {
  if( !i2c_dev || _delay == 0 ) {
    return -99.0;
  }
  
  i2c_dev->beginTransmission(TMP100_I2CADDR_DEFAULT);
  i2c_dev->write(TMP100_READ_REG);
  byte error = i2c_dev->endTransmission();

  if( error != 0 ) {
    return -99.0;
  }

  unsigned long startTime = millis();
  while( i2c_dev->requestFrom(TMP100_I2CADDR_DEFAULT, 2) != 2 ) {
    if( (millis() - startTime) > (_delay + 10) ) {
      return -99.0;
    }
    delay(5);
  }

  int16_t buffer = 0;
  buffer |= (i2c_dev->read() << 8);
  buffer |= i2c_dev->read();
  float temp = buffer / 16.0 * 0.0625;

  return temp;
}

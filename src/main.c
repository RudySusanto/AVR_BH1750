#include <Arduino.h>
#include <avr/io.h>
#include <stdio.h>       // for printf()
#include <util/delay.h>  // for _delay_ms()
#include "serial.h" 
#include "i2c.h" 

#define BH1750_ADDRESS  (0x23)

uint8_t init_bh1750() {
  uint8_t result;
  // Start I2C transmission for write
  result = i2c_start_transmission( (BH1750_ADDRESS << 1) | TW_WRITE);
  if ( result ) {
    i2c_end_transmission();
    printf("BH1750 initialization failed!\n");
    return 1; // error
  }
  i2c_write( 0x01 ); // Send POWER_ON command
  // End I2C transmission
  i2c_end_transmission();
  _delay_ms(150);
  printf("BH1750 initialization OK!\n");
  return 0;
}

uint8_t bh1750_read( uint16_t *raw_value ) {
  uint8_t result;
  // Start I2C transmission for write
  result = i2c_start_transmission( (BH1750_ADDRESS << 1) | TW_WRITE);
  if ( result ) {
    i2c_end_transmission();
    printf("BH1750: I2C operation failed...!\n");
    return 1; // error
  }
  // Send command to initiate single measurement with 1.0x resolution
  i2c_write( 0x20 );
  // End I2C transmission
  i2c_end_transmission();
  _delay_ms(120);

  // Start I2C transmission for read
  result = i2c_start_transmission( (BH1750_ADDRESS << 1) | TW_READ );
  if ( result ) {
    printf("BH1750: I2C operation failed...!\n");
    i2c_end_transmission();
    return 2; // error
  }
  // Read two bytes of light data
  uint8_t buf[2] = {0x00,0x00};
  i2c_read(&buf[0], 1 /*ACK*/);
  i2c_read(&buf[1], 0 /*NACK*/);
  // End I2C transmission
  i2c_end_transmission();
  *raw_value = (buf[0] << 8) | buf[1];
  return 0;
}

int main(void) {
  // Initialize I2C
  init_i2c(400000);
  // Initialize the UART and set baudrate to 9600.
  init_uart(9600);
  // Initialize BH1750
  init_bh1750();
  while (1) {
    uint16_t value;
    uint8_t result;
    result = bh1750_read(&value);
    if (!result) {
      float lux = value/1.2f;
      printf("Lux:%u\n", (uint16_t)lux );
    } else {
      printf("Lux:---- (err=%d)\n", result );
    }
    _delay_ms(500);
  }
  return 0;
}
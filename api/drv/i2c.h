#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>


typedef uint8_t i2c_dev_addr_t;
typedef uint8_t i2c_ptr_t;

void i2c_mstr_init(void);

void i2c_mstr_write(i2c_dev_addr_t slave_addr, i2c_ptr_t ptr, void *data, size_t len);
void i2c_mstr_read(i2c_dev_addr_t slave_addr, i2c_ptr_t ptr, void *data, size_t len);

#endif /* I2C_H_ */


#ifndef BME280_H_
#define BME280_H_

#define BME280_ADDR      0x76 // device address is 111011x

#define BME280_CALIB00   0x88 // calibration data - 25 bytes at 0x88...0xA1
#define BME280_CALIB26   0xE1 // calibration data - 7 bytes at 0xE1...0xE7

#define BME280_HUM_LSB    0xFE
#define BME280_HUM_MSB    0xFD
#define BME280_TEMP_XLSB  0xFC
#define BME280_TEMP_LSB   0xFB
#define BME280_TEMP_MSB   0xFA
#define BME280_PRESS_XLSB 0xF9
#define BME280_PRESS_LSB  0xF8
#define BME280_PRESS_MSB  0xF7
#define BME280_CONFIG     0xF5
#define BME280_CTRL_MEAS  0xF4
#define BME280_STATUS     0xF3
#define BME280_CTRL_HUM   0xF2
#define BME280_RESET      0xE0
#define BME280_ID         0xD0

// sensor configuration

#define BME280_OSRS_H    1  // humidity oversampling x 1
#define BME280_OSRS_T    1  // temperature oversampling x 1
#define BME280_OSRS_P    1  // pressure oversampling x 1
#define BME280_MODE      3  // forced mode: 1, normal mode: 3
#define BME280_T_SB      5  // standby in normal mode 1000ms 
#define BME280_FILTER    0  // filter off 
#define BME280_SPI3W_EN  0  // 3-wire SPI disable

#define BME280_CTRL_HUM_VAL   (BME280_OSRS_H)
#define BME280_CTRL_MEAS_VAL  (BME280_OSRS_T << 5) | (BME280_OSRS_P << 2) | BME280_MODE
#define BME280_CONFIG_VAL     (BME280_T_SB << 5) | (BME280_FILTER << 2) | BME280_SPI3W_EN

#endif /* BME280_H_ */


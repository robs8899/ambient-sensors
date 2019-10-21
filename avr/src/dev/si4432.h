#define SI4432_NSEL_PIN                    0 // NSEL port pin number for spi_cs() / spi_ds(): 0=PB2, 1=PB1
#define SI4432_FIXP_SIZE                  58 // fixed packet size (58);

#define SI4432_REG_DEV_TYPE             0x00 // Device Type Code
#define SI4432_REG_DEV_VERSION          0x01 // Version Code
#define SI4432_REG_DEV_STATUS           0x02 // Device Status

#define SI4432_REG_STATUS_1             0x03 // Interrupt/Status 1

// bits for SI4432_REG_STATUS_1
#define SI4432_STATUS_1_IFFERR          0x80 // FIFO Underflow/Overflow Error
#define SI4432_STATUS_1_ITXFFAFULL      0x40 // TX FIFO Almost Full
#define SI4432_STATUS_1_IXTFFAEM        0x20 // TX FIFO Almost Empty
#define SI4432_STATUS_1_IRXFFAFULL      0x10 // RX FIFO Almost Full
#define SI4432_STATUS_1_IEXT            0x08 // External Interrupt
#define SI4432_STATUS_1_IPKSENT         0x04 // Packet Sent Interrupt
#define SI4432_STATUS_1_IPKVALID        0x02 // Valid Packet Received
#define SI4432_STATUS_1_ICRERROR        0x01 // CRC Error

#define SI4432_STATUS_1_IGNORE          0x00 // ignore all Status 1 flags in ezr_poll()

#define SI4432_REG_STATUS_2             0x04 // Interrupt/Status 2

// bits for SI4432_REG_STATUS_2
#define SI4432_STATUS_2_ISWDET          0x80 // Sync Word Detected
#define SI4432_STATUS_2_IPREAVAL        0x40 // Valid Preamble Detected
#define SI4432_STATUS_2_IPREAINVAL      0x20 // Invalid Preamble Detected
#define SI4432_STATUS_2_IRSSI           0x10 // RSSI level exceeds threshold
#define SI4432_STATUS_2_IWUT            0x08 // Wake-Up-Time expiration
#define SI4432_STATUS_2_ILBD            0x04 // Low Battery Detect
#define SI4432_STATUS_2_ICHIPRDY        0x02 // Chip Ready (XTAL)
#define SI4432_STATUS_2_IPOR            0x01 // Power-on-Reset (POR)

#define SI4432_STATUS_2_IGNORE          0x00 // ignore all Status 2 flags in ezr_poll()

#define SI4432_REG_INT_1                0x05 // Interrupt Enable 1

// bits for SI4432_REG_INT_1
#define SI4432_INT_1_ENFFERR            0x80 // Enable FIFO Underflow/Overflow
#define SI4432_INT_1_ENTXFFAFULL        0x40 // Enable TX FIFO Almost Full
#define SI4432_INT_1_ENTXFFAEM          0x20 // Enable TX FIFO Almost Empty
#define SI4432_INT_1_ENRXFFAFULL        0x10 // Enable RX FIFO Almost Full
#define SI4432_INT_1_ENEXT              0x08 // Enable External Interrupt
#define SI4432_INT_1_ENPKSENT           0x04 // Enable Packet Sent
#define SI4432_INT_1_ENPKVALID          0x02 // Enable Valid Packet Received
#define SI4432_INT_1_ENCRCERROR         0x01 // Enable CRC Error

#define SI4432_REG_INT_2                0x06 // Interrupt Enable 2

// bits for SI4432_REG_INT_2
#define SI4432_INT_2_ENSWDET            0x80 // Enable Sync Word Detected
#define SI4432_INT_2_ENPREAVAL          0x40 // Enable Valid Preamble Detected
#define SI4432_INT_2_ENPREAINVAL        0x20 // Enable Invalid Preamble Detected
#define SI4432_INT_2_ENRSSI             0x10 // Enable RSSI Threshold for CCA
#define SI4432_INT_2_ENWUT              0x08 // Enable Wake-Up Timer
#define SI4432_INT_2_ENLBD              0x04 // Enable Low Battery Detect
#define SI4432_INT_2_ENCHIPRDY          0x02 // Enable Chip Ready (XTAL)
#define SI4432_INT_2_ENPOR              0x01 // Enable POR

#define SI4432_REG_OPMODE_CTRL_1        0x07 // Operating Mode and Function Control Register 1

// bits for SI4432_REG_OPMODE_CTRL1
#define SI4432_OPMODE_CTRL_1_SWRES      0x80 // Software Register Reset Bit
#define SI4432_OPMODE_CTRL_1_ENLBD      0x40 // Enable Low Battery Detect
#define SI4432_OPMODE_CTRL_1_ENWT       0x20 // Enable Wake-Up-Timer
#define SI4432_OPMODE_CTRL_1_X32KSEL    0x10 // 32,768 kHz Crystal Oscillator Select
#define SI4432_OPMODE_CTRL_1_TXON       0x08 // TX on in Manual Transmit Mode
#define SI4432_OPMODE_CTRL_1_RXON       0x04 // RX on in Manual Receiver Mode
#define SI4432_OPMODE_CTRL_1_PLLON      0x02 // TUNE Mode (PLL is ON)
#define SI4432_OPMODE_CTRL_1_XTON       0x01 // READY Mode (Xtal is ON)

#define SI4432_REG_OPMODE_CTRL_2        0x08 // Operating Mode and Function Control Register 2

// bits for SI4432_REG_OPMODE_CTRL_2
#define SI4432_OPMODE_CTRL_2_RXMPK      0x10 // RX Multi Packet
#define SI4432_OPMODE_CTRL_2_AUTOTX     0x08 // Automatic Transmission
#define SI4432_OPMODE_CTRL_2_ENLDM      0x04 // Enable Low Duty Cycle Mode
#define SI4432_OPMODE_CTRL_2_FFCLRRX    0x02 // RX FIFO Reset/Clear
#define SI4432_OPMODE_CTRL_2_FFCLRTX    0x01 // TX FIFO Reset/Clear

#define SI4432_REG_RSSI_VALUE           0x26 // Received Signal Strength Indicator
#define SI4432_REG_RSSI_CCI             0x27 // RSSI Threshold for Clear Channel Indicator
#define SI4432_REG_ANT_DIV_1            0x28 // Antenna Diversity 1
#define SI4432_REG_ANT_DIV_2            0x29 // Antenna Diversity 2
#define SI4432_REG_AFC_LIMITER          0x2A // AFC Limiter
#define SI4432_REG_AFC_CORRECT          0x2B // AFC Correction (MSBs)
#define SI4432_REG_OOK_COUNTER_1        0x2C // OOK Counter Value 1
#define SI4432_REG_OOK_COUNTER_2        0x2D // OOK Counter Value 2

#define SI4432_REG_SLICER_PEAK_HLDR     0x2E // Slicer Peak Holder
#define SI4432_REG_DATA_ACCESS_CTRL     0x30 // Data Access Control

#define SI4432_AFC_LSB                  0x0C // AFC correction bits [7:6] in REG_OOK_COUNTER_1

#define SI4432_REG_AGC_OVERRIDE         0x69 // AGC Control Register
#define SI4432_AGC_LNA_GAIN_BIT         0x10 // LAN Gain: 0=5dB / 1=25 dB
#define SI4432_AGC_PGA_GAIN_MSK         0x0F // PGA Gain mask: 3dB steps

#define SI4432_REG_MDMODE_CTRL_1        0x70 // Modulation Mode Control 1
#define SI4432_REG_MDMODE_CTRL_2        0x71 // Modulation Mode Control 2

// values for Modulation source: dtmod[5:4]
#define SI4432_MDMODE_CTRL_2_GPIO       0x00 // Direct Mode via the GPIO pin
#define SI4432_MDMODE_CTRL_2_SDI        0x10 // Direct Mode via the SDI pin
#define SI4432_MDMODE_CTRL_2_FIFO       0x20 // FIFO Mode
#define SI4432_MDMODE_CTRL_2_PN9        0x30 // PN9 (internally generated)

// values for Modulation Type: modtyp[1:0]
#define SI4432_MDMODE_CTRL_2_CARRIER    0x00 // Unmodulated carrier
#define SI4432_MDMODE_CTRL_2_OOK        0x01 // OOK
#define SI4432_MDMODE_CTRL_2_FSK        0x02 // FSK
#define SI4432_MDMODE_CTRL_2_GFSK       0x03 // GFSK

#define SI4432_REG_FIFO                 0x7F // FIFO read/write access

/* register/value pair of the init sequence */
typedef struct EZR_REG_VAL {
  uint8_t reg;
  uint8_t val;
} ezr_reg_val;

void ezr_init(int port, int nirq, int sdn);

void ezr_startup();
void ezr_shutdown();
void ezr_configure();

uint8_t ezr_get(uint8_t reg);
uint8_t ezr_set(uint8_t reg, uint8_t val);
void ezr_rx(void *data, int len);
void ezr_tx(void *data, int len);

int ezr_poll(uint8_t s1, uint8_t s2, int max);




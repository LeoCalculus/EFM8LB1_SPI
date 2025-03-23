#define SYSCLK 72000000L    // Internal oscillator frequency in Hz
#define F_SCK_MAX 2000000L  // Max SCK freq (Hz)

// Pins used by the SPI interface
// You might need to adjust with crossbar
#define SPI_SCK  P0_0
#define SPI_MISO P0_1
#define SPI_MOSI P0_2
#define SPI_SS   P0_3    // Chip select 

char _c51_external_startup(void);
unsigned char SPITransfer(unsigned char tx_data);
unsigned char ReadRegister(unsigned char reg_addr);
void WriteRegister(unsigned char reg_addr, unsigned char reg_value);

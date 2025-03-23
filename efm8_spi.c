#include <stdio.h>
#include <stdlib.h>
#include <EFM8LB1.h> // this lib was provided by Jesus Calvino Fraga's CrossIDE

// Credit: Jesus Calvino Fraga's example code
char _c51_external_startup(void)
{
    // Disable Watchdog with key sequence
    SFRPAGE = 0x00;
    WDTCN = 0xDE; // First key
    WDTCN = 0xAD; // Second key
  
    VDM0CN = 0x80;      // Enable VDD monitor
    RSTSRC = 0x02|0x04; // Enable reset on missing clock detector and VDD

    // Configure clock for 72MHz
    SFRPAGE = 0x10;
    PFE0CN = 0x20;     // SYSCLK < 75 MHz
    SFRPAGE = 0x00;
    
    // Before setting clock to 72 MHz, must transition to 24.5 MHz first
    CLKSEL = 0x00;
    CLKSEL = 0x00;
    while ((CLKSEL & 0x80) == 0);
    CLKSEL = 0x03;
    CLKSEL = 0x03;
    while ((CLKSEL & 0x80) == 0);

    // Configure I/O ports for SPI and UART
    P0MDOUT = 0b_0000_1101; // SCK, MOSI, and P0.3 are push-pull, others open-drain
    P1MDOUT = 0b_0000_0000; // P1 all open-drain
    XBR0 = 0b_0000_0011;    // SPI0E=1, URT0E=1
    XBR1 = 0b_0000_0000;
    XBR2 = 0b_0100_0000;    // Enable crossbar and weak pull-ups

    // Configure UART0
    SCON0 = 0x10;
    TH1 = 0x100-((SYSCLK/BAUDRATE)/(12L*2L));
    TL1 = TH1;          // Init Timer1
    TMOD &= ~0xf0;      // TMOD: timer 1 in 8-bit auto-reload
    TMOD |= 0x20;       
    TR1 = 1;            // START Timer1
    TI = 1;             // Indicate TX0 ready

    // Configure SPI: Notice, you might need to adjust it according to data sheet
    SPI0CKR = (SYSCLK/(2*F_SCK_MAX))-1;
    SPI0CFG = 0b_1110_0000; // this is mode 3, you need to determine the sampling etc. in datasheet
    SPI0CN0 = 0b_0000_0001; // SPI enabled and in three wire mode
    
    return 0;
}

unsigned char SPITransfer(unsigned char tx_data)
{
    SPI0DAT = tx_data;  // Send data
    while (!SPIF);      // Wait for transfer to complete
    SPIF = 0;           // Clear SPI interrupt flag
    return SPI0DAT;     // Return received data
}

unsigned char ReadRegister(unsigned char reg_addr)
{
    unsigned char value;
    
    // For SPI read operation, set MSB of address to 1
    reg_addr = reg_addr | 0x80;
    
    SPI_SS = 0;                // Select the device
    SPITransfer(reg_addr);     // Send register address
    value = SPITransfer(0x00); // Read value (send dummy byte) dummy byte is a placeholder byte sent by the master when it wants to receive data
    SPI_SS = 1;                // Deselect the device
    
    return value;
}

void WriteRegister(unsigned char reg_addr, unsigned char reg_value)
{
    // For SPI write operation, MSB should be 0
    reg_addr = reg_addr & 0x7F;
    
    SPI_SS = 0;                // Select device
    SPITransfer(reg_addr);     // Send register address
    SPITransfer(reg_value);    // Send value
    SPI_SS = 1;                // Deselect device
}


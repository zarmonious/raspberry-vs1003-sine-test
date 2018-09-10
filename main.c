/**
 * A sine test for VS1003 on RaspberryPi 3 (262Hz)
 * Link to the documentation of VS1003 www.vlsi.fi/fileadmin/datasheets/vs1003.pdf 
 */

#include <stdio.h>
#include <stdint.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>

#define SCI_INSTRUCTION_WRITE 0x02
#define SCI_INSTRUCTION_READ  0x03




void writeSCI(uint8_t address, uint16_t data)
{
    unsigned char buf[4] = {SCI_INSTRUCTION_WRITE, address, data >> 8, data & 0xFF};	// temp array of 4 chars
    (void)wiringPiSPIDataRW(0, buf , 4);										    	// paraments defined in the lib, require (int channel, unsigned char *data, int len)
}

uint16_t readSCI(uint8_t address)
{
    uint16_t result = 0x0000;

    unsigned char buf[4] = {SCI_INSTRUCTION_READ, address, 0x00, 0x00};
    (void)wiringPiSPIDataRW(0, buf , 4);
    result = buf[2]<<8 | buf[3];

    return result;
}

void writeSDItest(void)
{
    unsigned char buf[8] = {0x53, 0xEF, 0x6E, 0x21, 0,0,0,0};		// temp array of 4 chars bcs we have to send 8 bytes (4bytes and 4 zero bytes)
    (void)wiringPiSPIDataRW(1, buf , 8);

    delay (5000);

    unsigned char buf2[8] = {0x45, 0x78, 0x69, 0x74, 0,0,0,0};		// a sequence to disable sine
    (void)wiringPiSPIDataRW(1, buf2 , 8);
}

int main(void)
{
    int x = 0;
    (void)wiringPiSetup();
    (void)wiringPiSPISetup(0, 787504);								// channel 0 is SCI (Serial Command Interface)
    (void)wiringPiSPISetup(1, 787504);								// channel 1 is SDI (Serial Data Interface)
    pinMode(1, INPUT);												// 1 is for GPIO 18 (DREQ)
    pinMode(2, OUTPUT);												// 2 is for GPIO 27 (XRST)


    digitalWrite(2, LOW);											// Start hard reset
    delay(20);
    digitalWrite(2, HIGH);											// End hard reset
    delay(20);

    uint16_t currentMode = readSCI(0x00);							// address of SCI Registers (SCI_MODE)
    currentMode = currentMode | 1 << 5;								// changed 6th (SM_TESTS) bit to 1

    writeSCI(0x00, currentMode);									// changed 6th bit in SCI_MODE to 1 w/o modifying others
    delay(20);
    writeSDItest();

    return 0;
}

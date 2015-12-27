#define SCK  7
#define MOSI 5
#define SS0  3

void spiWrite2Bytes(char high, char low, unsigned int SS);
void setBit( int bit);
void clearBit( int bit);

volatile register uint32_t __R30;

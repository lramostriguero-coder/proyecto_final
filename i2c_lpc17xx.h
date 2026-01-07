extern void I2Cdelay(void);
extern void pulso_SCL(void);
extern void I2CSendByte(unsigned char byte);
extern void I2CSendAddr(unsigned char addr, unsigned char rw);
extern unsigned char I2CGetByte(unsigned char ACK);
extern void I2CSendStop(void);

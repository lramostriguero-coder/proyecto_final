// Librería de funciones del sensor TCS34725 de Adafruit para Arduino adaptada:
// https://github.com/adafruit/Adafruit_TCS34725/blob/master/examples/tcs34725/tcs34725.ino

#include <LPC17xx.H>
#include <stdint.h>
#include <TCS34725.h>
#include <i2c_lpc17xx.h>

// Definiciones para mejorar la legibilidad de la librería I2C
#define I2C_WRITE 0
#define I2C_READ  1
#define I2C_ACK   0
#define I2C_NACK  1

uint8_t _tcs34725IntegrationTime;
uint8_t  _tcs34725Gain;

void delay_ms(int ms) {
   int i,j;
   for (j=0; j < ms; j++ )
      for (i=0; i < 20000; i++);
}


//----------------------------------------------------------------------------
// Funcion que escribe un valor de 8 bits en un registro
void writeReg(uint8_t reg, uint8_t value)
{
	// 1. Generar START y enviar dirección del esclavo + Bit Escritura (0)
    // Tu función I2CSendAddr ya hace el Start internamente.
    I2CSendAddr(TCS34725_ADDRESS, I2C_WRITE);
    
    // 2. Enviar el registro destino. Importante: OR con 0x80 (Command Bit) 
    I2CSendByte(reg | TCS34725_COMMAND_BIT);
    
    // 3. Enviar el dato
    I2CSendByte(value);
    
    // 4. Generar STOP
    I2CSendStop();
}
//----------------------------------------------------------------------------
// Funcion que escribe un valor de 16 bits en dos registros de 8 bits consecutivos. 
// Primero el byte bajo y luego el byte alto 
void writeReg16Bit(uint8_t reg, uint16_t value)
{
	// En I2C, primero enviamos el byte bajo y luego el alto
    writeReg(reg, (uint8_t)(value & 0xFF));       // Byte bajo
    writeReg(reg + 1, (uint8_t)(value >> 8));     // Byte alto			
}
//----------------------------------------------------------------------------
uint8_t readReg(uint8_t reg)
// Funcion que lee un valor de 8 bits de un registro
{
  uint8_t value;
    
    // 1. Escritura para seleccionar el registro (Dummy Write)
    I2CSendAddr(TCS34725_ADDRESS, I2C_WRITE);
    
    // 2. Enviar dirección del registro con Command Bit 
    I2CSendByte(reg | TCS34725_COMMAND_BIT);
    
    // 3. Repeated START + Dirección de Lectura
    // Nota: Al llamar a I2CSendAddr de nuevo sin un Stop previo, 
    // se comporta como un Repeated Start en tu implementación bit-banging.
    I2CSendAddr(TCS34725_ADDRESS, I2C_READ);
    
    // 4. Leer el dato y enviar NACK (1) porque es el último (y único) byte
    value = I2CGetByte(I2C_NACK);
    
    // 5. Generar STOP
    I2CSendStop();
    
    return value;					
}
//----------------------------------------------------------------------------
uint16_t readReg16Bit(uint8_t reg)
// Funcion que lee un valor de 16 bits de dos registros de 8 bits consegutivos. 
// Primero el byte bajo y luego el byte alto 
{
  uint16_t value;
    uint8_t low, high;

    // Opción Robusta: Leer byte a byte usando la función anterior.
    // Esto es un poco más lento pero muy seguro.
    low = readReg(reg);
    high = readReg(reg + 1);
    
    // Componer el valor de 16 bits
    value = ((uint16_t)high << 8) | low;
    
    return value;
}
//----------------------------------------------------------------------------


void TCS34725_init(){
	/* Power on --> default values (int time = 2.4ms, gain = 1x) */

  /* Initialise with specific int time and gain values */
  /* Set default integration time and gain */
  TCS34725_setIntegrationTime(TCS34725_INTEGRATIONTIME_50MS);
  TCS34725_setGain(TCS34725_GAIN_4X);
}

//----------------------------------------------------------------------------
/*!
 *  @brief  Sets the integration time for the TC34725
 *  @param  it
 *          Integration Time
 */
void TCS34725_setIntegrationTime(uint8_t it) {
  /* Update the timing register */
  writeReg(TCS34725_ATIME, it);
	_tcs34725IntegrationTime=it;
}

//----------------------------------------------------------------------------
/*!
 *  @brief  Enables the device
 */
void TCS34725_enable(void) {
  writeReg(TCS34725_ENABLE, TCS34725_ENABLE_PON);
  delay_ms(5);
  writeReg(TCS34725_ENABLE, TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN);
  /* Set a delay for the integration time.
    This is only necessary in the case where enabling and then
    immediately trying to read values back. This is because setting
    AEN triggers an automatic integration, so if a read RGBC is
    performed too quickly, the data is not yet valid and all 0's are
    returned */
  /* 12/5 = 2.4, add 1 to account for integer truncation */
  delay_ms((256 - _tcs34725IntegrationTime) * 12 / 5 + 1);
}

//----------------------------------------------------------------------------
/*!
 *  @brief  Disables the device (putting it in lower power sleep mode)
 */
void TCS34725_disable(void) {
  /* Turn the device off to save power */
  uint8_t reg = 0;
  reg = readReg(TCS34725_ENABLE);
  writeReg(TCS34725_ENABLE, reg & ~(TCS34725_ENABLE_PON | TCS34725_ENABLE_AEN));
}

//----------------------------------------------------------------------------
/*!
 *  @brief  Adjusts the gain on the TCS34725
 *  @param  gain
 *          Gain (sensitivity to light)
 */
void TCS34725_setGain(tcs34725Gain_t gain) {
  /* Update the timing register */
  writeReg(TCS34725_CONTROL, gain);
	_tcs34725Gain=gain;
 }

 //----------------------------------------------------------------------------
/*!
 *  @brief  Reads the raw red, green, blue and clear channel values
 *  @param  *r
 *          Red value
 *  @param  *g
 *          Green value
 *  @param  *b
 *          Blue value
 *  @param  *c
 *          Clear channel value
 */
void TCS34725_getRawData(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c) {

  *r = readReg16Bit(TCS34725_RDATAL);
  *g = readReg16Bit(TCS34725_GDATAL);
  *b = readReg16Bit(TCS34725_BDATAL);
	*c = readReg16Bit(TCS34725_CDATAL);

  /* Set a delay for the integration time */
  /* 12/5 = 2.4, add 1 to account for integer truncation */
  delay_ms((256 - _tcs34725IntegrationTime) * 12 / 5 + 1);
}


//-----------------------------------------------------------------------------
/*!
 *  @brief  Reads the raw red, green, blue and clear channel values in
 *          one-shot mode (e.g., wakes from sleep, takes measurement, enters
 *          sleep)
 *  @param  *r
 *          Red value
 *  @param  *g
 *          Green value
 *  @param  *b
 *          Blue value
 *  @param  *c
 *          Clear channel value
 */
void TCS34725_getRawDataOneShot(uint16_t *r, uint16_t *g, uint16_t *b, uint16_t *c) {
  TCS34725_enable();
  TCS34725_getRawData(r, g, b, c);
  TCS34725_disable();
}



//------------------------------------------------------------------------------------
/*!
 *  @brief  Read the RGB color detected by the sensor.
 *  @param  *r
 *          Red value normalized to 0-255
 *  @param  *g
 *          Green value normalized to 0-255
 *  @param  *b
 *          Blue value normalized to 0-255
 */
void TCS34725_getRGB(float *r, float *g, float *b) {
 
	uint16_t red, green, blue, clear;
	uint32_t sum = clear;
  TCS34725_getRawData(&red, &green, &blue, &clear);
  
  // Avoid divide by zero errors ... if clear = 0 return black
  if (clear == 0) {
    *r = *g = *b = 0;
    return;
  }
  *r = (float)red / sum * 255.0;
  *g = (float)green / sum * 255.0;
  *b = (float)blue / sum * 255.0;
}


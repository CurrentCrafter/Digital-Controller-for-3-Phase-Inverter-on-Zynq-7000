#ifndef SPI_H
#define SPI_H

//#include <stdint.h>
//#include "xil_types.h"
#include "xspips.h"



//SPI definitions
#define SPI_DEVICE_ID   XPAR_XSPIPS_0_DEVICE_ID  // Passe an, falls ein anderer SPI verwendet wird
#define DAC_SPI_SELECT  0x00  // Falls ein Multiplexer vorhanden ist
#define CHANNEL_1 	0x08
#define	CHANNEL_2	0x09
#define	CHANNEL_3	0x0A
#define	CHANNEL_4	0x0B

extern XSpiPs SpiInstance;

int Spi_Init(XSpiPs *SpiInstancePtr, u16 SpiDeviceId);
void Spi_Write_Dac(XSpiPs *SpiInstancePtr, u8 reg, u16 data);
void Spi_Trigger_on_Error();
void Spi_Trigger();
void oszi_write(u8 channel,float vmax, float value);
void oszi_write_all();




#endif // SPI_H


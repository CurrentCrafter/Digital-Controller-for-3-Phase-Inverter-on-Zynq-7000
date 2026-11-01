#include "MASTER.h"


XSpiPs SpiInstance;

// Function Name: SpiInit()
// Initalisation of the SPI Protocoll which is nessasary for the communication with the ADC's on the logic board.
int Spi_Init(XSpiPs *SpiInstancePtr, u16 SpiDeviceId) {
    XSpiPs_Config *ConfigPtr;
    int Status;

    ConfigPtr = XSpiPs_LookupConfig(SpiDeviceId);
    if (ConfigPtr == NULL) {
        xil_printf("SPI Config nicht gefunden!\n");
        return XST_FAILURE;
    }

    Status = XSpiPs_CfgInitialize(SpiInstancePtr, ConfigPtr, ConfigPtr->BaseAddress);
    if (Status != XST_SUCCESS) {
        xil_printf("SPI Initialisierung fehlgeschlagen!\n");
        return XST_FAILURE;
    }

    XSpiPs_SetOptions(SpiInstancePtr, XSPIPS_MASTER_OPTION | XSPIPS_CLK_PHASE_1_OPTION);
    XSpiPs_SetClkPrescaler(SpiInstancePtr, XSPIPS_CLK_PRESCALE_4);
    XSpiPs_Enable(SpiInstancePtr);

    XSpiPs_SetSlaveSelect(SpiInstancePtr, DAC_SPI_SELECT);

    Spi_Write_Dac(&SpiInstance, 0x00, 0x0000);

    return XST_SUCCESS;
}

// Function Name: SpiWriteDac()
// Writes a value to the ADCs using the SPI protocol.
// The 'u8_register' parameter specifies the target ADC channel ( 1, 2, 3 or 4),
// and 'data' contains the value to be written. Note: The max value ist 4090
void Spi_Write_Dac(XSpiPs *SpiInstancePtr, u8 reg, u16 data) {
    u8 TxBuffer[3];
    data = (data << 4);
    TxBuffer[0] = reg & 0xFF;
    TxBuffer[1] = (data >> 8) & 0xFF;
    TxBuffer[2] = data & 0xFF;

    XSpiPs_PolledTransfer(SpiInstancePtr, TxBuffer, NULL, 3);
}

//Function Name: Spi_Trigger_on_Error()
//Triggers the 4th channel of the DAC to signal an error state.
void Spi_Trigger_on_Error()
{
    if(Error_Code > 2)
    {
        Spi_Write_Dac(&SpiInstance, CHANNEL_4, 4090);
    }
    else
    {
        Spi_Write_Dac(&SpiInstance, CHANNEL_4, 2046);
    }
}

//Function Name: Spi_Trigger()
//Triggers the 4th channel of the DAC to ease a single shot
//Sends a high potential, if trigger is 1
//Sends a low potential if trigger is 0
//Setup on oszi: Single shot and trigger on rising edge
void Spi_Trigger()
{
    if(oszi.trigger == 0)
    {
        Spi_Write_Dac(&SpiInstance, CHANNEL_4, 4090);
    }
    if(oszi.trigger == 1)
    {
        Spi_Write_Dac(&SpiInstance, CHANNEL_4, 2046);
    }
}

// Function Name: oszi_write()
// Writes a value to the oscilloscope channels of the DAC.
// The 'channel' parameter specifies the target channel (1, 2, 3 or 4),
// and 'value' contains the value to be written.
// vmax is the maximum value that corresponds to full scale (100%).^
// Range on oszi: [-vmax; vmax]
// The function maps the input value to the DAC range based on the specified maximum value.
void oszi_write(u8 channel,float vmax, float value) {
    u8 reg = 0;
    u16 output_value = 0;
    float frac = 0;

    if (vmax <= 0.0f) return;

    if (channel == 1 || channel == 2 || channel == 3 || channel == 4) {

    	switch(channel){
    	case 1:
    		reg = CHANNEL_1;
    		break;
    	case 2:
    		reg = CHANNEL_2;
    		break;
    	case 3:
    		reg = CHANNEL_3;
    		break;
    	case 4:
    		reg = CHANNEL_4;
    		break;
    	default:
    		return;
    	}

         if (value >= 0.0f) {
            frac = value / vmax;          // [0, ...]
            if (frac > 1.0f) frac = 1.0f; // Begrenzung auf +100%
            // Positiver Bereich: MID bis MAXV
            output_value = MID + (u16)roundf(frac * (MAXV - MID));
        } else {
            frac = (-value) / vmax;       // [0, ...]
            if (frac > 1.0f) frac = 1.0f; // Begrenzung auf -100%
            // Negativer Bereich: MINV bis MID-1
            output_value = MID - (u16)roundf(frac * (MID - MINV));
        }
    // Clamp zur Sicherheit
    if (output_value < MINV) output_value = MINV;
    if (output_value > MAXV) output_value = MAXV;

    Spi_Write_Dac(&SpiInstance, reg, output_value);
    }
    else {
        return; // Ungültiger Kanal
    }
}

//writes all oszi channels
//if trigger is armed (= 1); the 4th channel is needed for the output
void oszi_write_all(){
	if(oszi.trigger_armed != 1){
		oszi_write(1,oszi.ch1_vmax, oszi.ch1);
		oszi_write(2,oszi.ch2_vmax, oszi.ch2);
		oszi_write(3,oszi.ch3_vmax, oszi.ch3);
		oszi_write(4,oszi.ch4_vmax, oszi.ch4);
	}else{
		oszi_write(1,oszi.ch1_vmax, oszi.ch1);
		oszi_write(2,oszi.ch2_vmax, oszi.ch2);
		oszi_write(3,oszi.ch3_vmax, oszi.ch3);
		Spi_Trigger();
	}
}

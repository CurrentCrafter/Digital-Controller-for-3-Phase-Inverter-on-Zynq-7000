#include "MASTER.h"
#include "ERROR.h"

// Function Name: Init_Errorhandling()
// Starting of the errorhandling.
int Init_Errorhandling() {
	u8 bitstream = 0;
	u8 Acknowledge_Error_var = 0;
	Modulator_Enable = 0;
	Errorhandling_nEnable = 0;

	Error_Code = 0;
	Error_handle_Max_Val();

	bitstream = Errorhandling_nEnable | (Modulator_Enable << 1) | (Acknowledge_Error_var << 2);
	XGpio_DiscreteWrite(&Enable_ErrCode_Temp, CHANNEL_ENABLE, bitstream);

	return XST_SUCCESS;
}

// Function: Enable_Errorhandling()
// Enabels the Errorhandling in the PL
void Enable_Errorhandling() {
	u32 bitstream = 0;
	Errorhandling_nEnable = 0;
	bitstream = Errorhandling_nEnable | (Modulator_Enable << 1) | (0 << 2);
	XGpio_DiscreteWrite(&Enable_ErrCode_Temp, CHANNEL_ENABLE, bitstream);
}

// Function: Diable_Errorhandling()
// Diables the Errorhandling in the PL
void Disable_Errorhandling() {
	u32 bitstream;
	Errorhandling_nEnable = 1;
	bitstream = Errorhandling_nEnable | (Modulator_Enable << 1) | (0 << 2);
	XGpio_DiscreteWrite(&Enable_ErrCode_Temp, CHANNEL_ENABLE, bitstream);
}

// Function: Acknowledge_Error()
// If an error occurs, the user must acknowledge the error via UART before restarting the modulator.
// This prevents the modulator from starting automatically.
void Acknowledge_Error() {
	u32 bitstream = 0;
	u8 Acknowledge_Error_var = 1;
	Disable_Modulator();
	bitstream = Errorhandling_nEnable | (Modulator_Enable << 1) | (Acknowledge_Error_var << 2);
	XGpio_DiscreteWrite(&Enable_ErrCode_Temp, CHANNEL_ENABLE, bitstream);

	Acknowledge_Error_var= 0;
	bitstream = Errorhandling_nEnable | (Modulator_Enable << 1) | (Acknowledge_Error_var << 2);
	XGpio_DiscreteWrite(&Enable_ErrCode_Temp, CHANNEL_ENABLE, bitstream);
	Read_Error_Code();
}

// Function: Error_handle_Max_Val()
// Converts the maximum measured floating-point values of currents and voltages
// into scaled 16-bit integer values for error handling purposes using specific calibration factors.
//Sends these scaled values to the FPGA via GPIO to set the maximum allowable limits.
void Error_handle_Max_Val(){

	u32 maskDC = 0x00;
	u32 maskGrid = 0x00;
	u32 maskConv = 0x00;
	u8 maskTemp = (u8) MAX_TEMP;

	maskDC = ((u16) (MAX_DC_CURRENT * PROP_I_DC));
	maskDC = (maskDC<<16) | ((u16) (MAX_DC_VOLTAGE * PROP_U_DC));
	maskGrid = ((u16) (MAX_GRID_CURRENT * PROP_I_AC));
	maskGrid = (maskGrid<<16) | ((u16) (MAX_GRID_VOLTAGE * PROP_UGRID_L2));
	maskConv = ((u16) (MAX_CONV_CURRENT  * PROP_I_AC));
	maskConv = (maskConv<<16) | ((u16) (MAX_CONV_VOLTAGE * PROP_UCF_L1));
	XGpio_DiscreteWrite(&Max_DC_Grid, CHANNEL_MAX_DC, maskDC);
	XGpio_DiscreteWrite(&Max_DC_Grid, CHANNEL_MAX_GRID, maskGrid);
	XGpio_DiscreteWrite(&Max_Conv_Temp, CHANNEL_MAX_CONV, maskConv);
	XGpio_DiscreteWrite(&Max_Conv_Temp, CHANNEL_MAX_TEMP, maskTemp);
}



// Function: Read_Status()
// Reads the current status of the error handling
void Read_Error_Code()
{
	u16 tmp = XGpio_DiscreteRead(&Enable_ErrCode_Temp, CHANNEL_ERRORCODE_TEMP);
	Error_Code = (u8) (tmp & 0b11111);
}

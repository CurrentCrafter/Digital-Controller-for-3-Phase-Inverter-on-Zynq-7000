#include "MASTER.h"
#include "xil_mmu.h"
#include "xil_cache.h"

#define AMOUNT_OF_BYTES 48

void Init_Measurement() {
    // Set the associated memory cacheable
    Xil_SetTlbAttributes(XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR, 0x15de6);
    Measurement_Enable = 1;
    XGpio_DiscreteWrite(&Duty_Cycles_Enable_Meas, CHANNEL_DUTY_CYCLE_PH3_ENABLE_MEAS, (Measurement_Enable << 16));
}


void Read_all_Measurments() {
	Xil_DCacheInvalidateRange(XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR, AMOUNT_OF_BYTES);
	uint64_t* bram_data_ptr = (uint64_t*)XPAR_AXI_BRAM_CTRL_0_S_AXI_BASEADDR;
	uint64_t bram_data_64[AMOUNT_OF_BYTES / 8];

	for (int i = 0; i < (AMOUNT_OF_BYTES / 8); i++) {
		bram_data_64[i] = bram_data_ptr[i];
	}

	u16 bram_data_16[AMOUNT_OF_BYTES / 2];
	int idx = 0;
	for (int i = 0; i < (AMOUNT_OF_BYTES / 8); i++) {
		bram_data_16[idx++] = (bram_data_64[i] >>  0) & 0xFFFF;
	    bram_data_16[idx++] = (bram_data_64[i] >> 16) & 0xFFFF;
	    bram_data_16[idx++] = (bram_data_64[i] >> 32) & 0xFFFF;
	    bram_data_16[idx++] = (bram_data_64[i] >> 48) & 0xFFFF;
	}

	int16_t tmp_16 = (int16_t) bram_data_16[0];
	Measurments.I_Lf_1 = ((float)tmp_16) / PROP_I_AC;

	tmp_16 = (int16_t) bram_data_16[1];
	Measurments.I_Lf_2 = ((float)tmp_16) / PROP_I_AC;

	tmp_16 = (int16_t) bram_data_16[2];
	Measurments.I_Lf_3 = ((float)tmp_16) / PROP_I_AC;

	tmp_16 = (int16_t) bram_data_16[3];
	Measurments.I_g_1 = ((float)tmp_16) / PROP_I_AC;

	tmp_16 = (int16_t) bram_data_16[4];
	Measurments.I_g_2 = ((float)tmp_16) / PROP_I_AC;

	tmp_16 = (int16_t) bram_data_16[5];
	Measurments.I_g_3 = ((float)tmp_16) / PROP_I_AC;

	tmp_16 = (int16_t) bram_data_16[6];
	Measurments.I_DC = ((float) tmp_16) / PROP_I_DC;

	tmp_16 = (int16_t) bram_data_16[13];
	Measurments.U_DC = ((float)tmp_16) / PROP_U_DC;

	tmp_16 = (int16_t) bram_data_16[7];
	Measurments.U_Cf_1 = (((float)tmp_16) / PROP_UCF_L1);//-(Measurments.U_DC/2);

	tmp_16 = (int16_t) bram_data_16[8];
	Measurments.U_Cf_2 = (((float)tmp_16) / PROP_UCF_L2);//-(Measurments.U_DC/2);

	tmp_16 = (int16_t) bram_data_16[9];
	Measurments.U_Cf_3 = (((float)tmp_16) / PROP_UCF_L3);//-(Measurments.U_DC/2);

	tmp_16 = (int16_t) bram_data_16[10];
	Measurments.U_g_1 = ((float)tmp_16) / PROP_UGRID_L1;

	tmp_16 = (int16_t) bram_data_16[11];
	Measurments.U_g_2 = ((float)tmp_16) / PROP_UGRID_L1;

	tmp_16 = (int16_t) bram_data_16[12];
	Measurments.U_g_3 = ((float)tmp_16) / PROP_UGRID_L1;


	//SOGI Parameter
	tmp_16 = (int16_t) bram_data_16[14];
	sogi_data.d = ((float) tmp_16) / PROP_UGRID_L1;

	tmp_16 = (int16_t) bram_data_16[15];
	sogi_data.q = ((float) tmp_16) / PROP_UGRID_L1;

	u32 sign = 0;
	u32 abs = 0;
	const u32 zweihoch23 = 0x800000;
	const u32 zweihoch28 = 0x10000000;


	int32_t tmp_32 = (int32_t) ((bram_data_16[17] << 16) | bram_data_16[16]);
	sogi_data.sin_norm = (double) ((s32) tmp_32) / 1073741824;

	tmp_32 = (int32_t) ((bram_data_16[19] << 16) | bram_data_16[18]);
	sogi_data.cos_norm = (double) ((s32) tmp_32) / 1073741824;

	u32 tmp_u32 = (u32) ((bram_data_16[21] << 16) | bram_data_16[20]);
	sogi_data.w_est = (double) (tmp_u32) / zweihoch23;

	tmp_32 = (int32_t) ((bram_data_16[23] << 16) | bram_data_16[22]);
	//theta
	sign = (tmp_32 & SIGN_MASK) >> 31;
	abs = tmp_32 & ABS_MASK;
	sogi_data.theta = (sign == 1) ? -(double) abs : (double) abs;
	sogi_data.theta /= zweihoch28;

	sogi_data.locked = XGpioPs_ReadPin(&EMIO_GPIO, EMIO_PIN_SOGI_LOCKED);
//	SOGI_locked = sogi_data.locked;
}


// Function: Read_Temperature()
// Reads the current temperature value from the sensor and stores it in the Errorhandling struct.
// Additionally, it accumulates the temperature values in sumTemp to enable smoothing,
// since temperature data is sent to the GUI via UART only at irregular intervals.
void Read_Temperature()
{
	u16 tmp = XGpio_DiscreteRead(&Enable_ErrCode_Temp, CHANNEL_ERRORCODE_TEMP);
	Measurments.Temperature = (u8) (tmp >> 5);
}


void Read_Slow_Changing_Meas(){
	u16 tmp = XGpio_DiscreteRead(&Enable_ErrCode_Temp, CHANNEL_ERRORCODE_TEMP);
	Error_Code = (u8) (tmp & 0b11111);
	Measurments.Temperature = (u8) (tmp >> 5);
}

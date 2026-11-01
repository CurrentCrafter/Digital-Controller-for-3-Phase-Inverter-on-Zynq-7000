#include "MASTER.h"
#include "DQ.h"



//struct debugging_values Debug_values[2000];
//int index_debug;

// Function: debug_values()
// Called only during the last period before the converter is disabled.
// Stores all of the dq Values over the time of one periode (2000 values)
// This values can be send over UART later for debugging purposes
void debug_values(){
	if(index_debug >= 1999){
		//index_debug = 0;
		//do nothing
	}
	else{
		Debug_values[index_debug].I_DC = Measurments.I_DC;
		Debug_values[index_debug].I_Lf_1 = Measurments.I_Lf_1;
		Debug_values[index_debug].I_Lf_2 = Measurments.I_Lf_2;
		Debug_values[index_debug].I_Lf_3 = Measurments.I_Lf_3;
		Debug_values[index_debug].I_g_1 = Measurments.I_g_1;
		Debug_values[index_debug].I_g_2 = Measurments.I_g_2;
		Debug_values[index_debug].I_g_3 = Measurments.I_g_3;

		Debug_values[index_debug].U_DC = Measurments.U_DC;
		Debug_values[index_debug].U_Cf_1 = Measurments.U_Cf_1;
		Debug_values[index_debug].U_Cf_2 = Measurments.U_Cf_2;
		Debug_values[index_debug].U_Cf_3 = Measurments.U_Cf_3;
		Debug_values[index_debug].U_g_1 = Measurments.U_g_1;
		Debug_values[index_debug].U_g_2 = Measurments.U_g_2;
		Debug_values[index_debug].U_g_3 = Measurments.U_g_3;

		Debug_values[index_debug].I_d_Conv = dq_values.I_d_Conv;
		Debug_values[index_debug].I_q_Conv = dq_values.I_q_Conv;
		Debug_values[index_debug].I_d_Grid = dq_values.I_d_Grid;
		Debug_values[index_debug].I_q_Grid = dq_values.I_q_Grid;
		Debug_values[index_debug].U_d_Conv = dq_values.U_d_Conv;
		Debug_values[index_debug].U_q_Conv = dq_values.U_q_Conv;
		Debug_values[index_debug].U_d_Grid = dq_values.U_d_Grid;
		Debug_values[index_debug].U_q_Grid = dq_values.U_q_Grid;

		index_debug++;
	}
}

void debug_sogi(){
	if(index_sogi >= 1999){
		//index_debug = 0;
		//do nothing
	}
	else{
		Debug_sogi[index_sogi].sin_norm = sogi_data.sin_norm;
		Debug_sogi[index_sogi].cos_norm = sogi_data.cos_norm;
		Debug_sogi[index_sogi].d = sogi_data.d;
		Debug_sogi[index_sogi].q = sogi_data.q;
		Debug_sogi[index_sogi].w_est = sogi_data.w_est;
		Debug_sogi[index_sogi].theta = sogi_data.theta;
		Debug_sogi[index_sogi].locked = sogi_data.locked;

		index_sogi++;
	}
}

// Function Name: testOszi()
// tests all of the registers and displays a 50 HZ sine wave on all of the displays
// NOTE: Please consider the runtime, when writing all 4 channels simutaniuly, it can be, that the function gets called to slow, resulting in an output thats shifted from 50 Hz
void testOszi()
{
	int num_samples = 100;

	for (int i = 0; i < num_samples; i++) {
	                double angle = (2 * M_PI * i) / num_samples;
	                u16 dac_value = (u16)((MAX_DAC_VALUE / 2) * (1 + sin(angle)) + 0.5);

	                Spi_Write_Dac(&SpiInstance, CHANNEL_1, dac_value);
	                Spi_Write_Dac(&SpiInstance, CHANNEL_2, dac_value);
	                Spi_Write_Dac(&SpiInstance, CHANNEL_3, dac_value);
	                Spi_Write_Dac(&SpiInstance, CHANNEL_4, dac_value);

	                usleep(100); // Frequenz einstellen
	            }
}

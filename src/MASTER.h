#ifndef MASTER_H
#define MASTER_H

///+++++++++++++++++++++++++++++++++++++
///Global used includes
///+++++++++++++++++++++++++++++++++++++
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
#include "xparameters.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xil_types.h"
#include "sleep.h"
#include "xgpio.h"
#include "xgpiops.h"
#include "xuartps.h"
#include "DEFINE.h"
#include "GUI.h"
#include "UART.h"
#include "ERROR.h"
#include "SPI.h"
#include "MEAS.h"
#include "DEBUG.h"
#include "DQ.h"
#include "HARDWARE.h"


///+++++++++++++++++++++++++++++++++++++
///Function Prototypes
///+++++++++++++++++++++++++++++++++++++
void isr0(void*);
int main();
///+++++++++++++++++++++++++++++++++++++
///Global Defines (GPIO below)
///+++++++++++++++++++++++++++++++++++++
#define MAX_DC_VOLTAGE 65
#define MAX_DC_CURRENT 6

#define MAX_GRID_VOLTAGE 40
#define MAX_GRID_CURRENT 25

#define MAX_CONV_VOLTAGE 65
#define MAX_CONV_CURRENT 10

#define MAX_TEMP 70

// PI-Parameter (müssen eingestellt werden!)
#define Kp_i 0.1f
#define Ki_i 10.0f


///+++++++++++++++++++++++++++++++++++++
///Global defined Variables
///+++++++++++++++++++++++++++++++++++++

//isr/main
//extern double nor;
//extern float phaseIncrement;

extern float Ref_ILg_d;
extern float Ref_ILg_q;
extern float Int_err_ILg_d;
extern float Int_err_ILg_q;
extern float Prop_ILg_d;
extern float Prop_ILg_q;
extern float Error_ILg_d;
//extern const float Ts; // 48*2 kHz Regelung
extern float U0;
extern float Ud;
extern float Uq;

extern u8 SOGI_locked;

extern float cos_theta;
extern float sin_theta;

extern float I_g_alpha;
extern float I_g_beta;

extern float I_g_d;
extern float I_g_q;

//Debug
extern int index_debug;
extern int index_sogi;

//ERROR
extern u8 Modulator_Enable;
extern u8 Measurement_Enable;
extern u8 Errorhandling_nEnable;
extern u8 debug;
extern u8 Error_Code;

//GUI
extern u8 Message_Received;
extern char lastStatus[UART_MESSAGE_SIZE_MAX];
extern char lastfeat1[UART_MESSAGE_SIZE_MAX];
extern char lastfeat2[UART_MESSAGE_SIZE_MAX];
extern char lastfeatvars[UART_MESSAGE_SIZE_MAX];
extern char lastLocked[UART_MESSAGE_SIZE_MAX];
extern u16 i;

extern u8 feature1;
extern u8 feature2;
extern int featureVars;

//MEAS
//extern int counter_temp;

//Hardware
#define SIGN_MASK 0x80000000U
#define ABS_MASK  0x7FFFFFFFU


///+++++++++++++++++++++++++++++++++++++
///Global defined Structs
///+++++++++++++++++++++++++++++++++++++

//MEAS
//name: Measurments
//function: Structure for storing the Sinc3 filter output converted to float values.
struct Measurment{
	float I_DC;
	float I_Lf_1;
	float I_Lf_2;
	float I_Lf_3;
	float I_g_1;
	float I_g_2;
	float I_g_3;

	float U_DC;
	float U_Cf_1;
	float U_Cf_2;
	float U_Cf_3;
	float U_g_1;
	float U_g_2;
	float U_g_3;
	u8 Temperature;
}; //Measurments
extern struct Measurment Measurments;

//Debug
//Struct mit den letzten 2000 Messwerten f r das debug Messen einer Periode aus der UART heraus
struct debugging_values{
	float I_DC;
	float I_Lf_1;
	float I_Lf_2;
	float I_Lf_3;
	float I_g_1;
	float I_g_2;
	float I_g_3;

	float U_DC;
	float U_Cf_1;
	float U_Cf_2;
	float U_Cf_3;
	float U_g_1;
	float U_g_2;
	float U_g_3;

	float I_d_Conv;
	float I_q_Conv;
	float U_d_Grid;
	float U_q_Grid;
	float I_d_Grid;
	float I_q_Grid;
	float U_d_Conv;
	float U_q_Conv;
};
extern struct debugging_values Debug_values[2000]; //Struct extern als Array nutzbar

//Debug
//Struct mit den letzten 2000 Messwerten f r das debug Messen einer Periode aus der UART heraus
struct debug_sogi{
	double sin_norm; //Werte > 2hoch24 werden zu stark gerundet
	double cos_norm;
	float d;
	float q;
	double w_est;
	double theta;
	u8 locked;
};
extern struct debug_sogi Debug_sogi[2000]; //Struct extern als Array nutzbar


//Sogi
//name: Measurments
//function: Structure for storing the Sinc3 filter output converted to float values.
struct sogi{
	double sin_norm;
	double cos_norm;
	float d;
	float q;
	double w_est;
	double theta;
	u8 locked;
};
extern struct sogi sogi_data;

//SPI

//name: oszi
//function: Structure for storing the values that should be written on the oszi
//If the trigger should be used, trigger_armed == 1, otherwise it has to be 0
struct oszidata{
	float ch1;
	float ch2;
	float ch3;
	float ch4;
	float ch1_vmax;
	float ch2_vmax;
	float ch3_vmax;
	float ch4_vmax;
	u8 trigger;
	u8 trigger_armed;
};
extern struct oszidata oszi;

///+++++++++++++++++++++++++++++++++++++
///GPIO
///+++++++++++++++++++++++++++++++++++++


//definitions GPIO Channels
#define CHANNEL_nLADC 1
#define CHANNEL_ERRORCODE_TEMP 1
#define CHANNEL_ENABLE 2
#define CHANNEL_DUTY_CYCLE_PH12 1
#define CHANNEL_DUTY_CYCLE_PH3_ENABLE_MEAS 2
#define CHANNEL_DUTY_CYCLE_FAN 2
#define CHANNEL_RELAYS 1
#define CHANNEL_TEMP 1

///////////////////////////////////////////////////////////////////////////////////
//#define CHANNEL_U_DC_GRID_PH1 1
//#define CHANNEL_U_GRID_PH2_3 2
//
//#define CHANNEL_I_GRID_PH1_2 1
//#define CHANNEL_I_DC_GRID_PH3 2
//
//#define CHANNEL_U_CONV_PH1_2 1
//#define CHANNEL_U_CONV_PH3 2
//
//#define CHANNEL_I_CONV_PH1_2 1
//#define CHANNEL_I_CONV_PH3 2
///////////////////////////////////////////////////////////////////////////////////

#define CHANNEL_MAX_DC 1
#define CHANNEL_MAX_GRID 2

#define CHANNEL_MAX_CONV 1
#define CHANNEL_MAX_TEMP 2

#define CHANNEL_FREQUENCY 1
#define CHANNEL_DELAY_1 2

#define CHANNEL_DELAY_CONV 1
#define CHANNEL_DELAY_GRID 2

#define SOGI_in 1

///////////////////////////////////////////////////////////////////////////////////
//#define SOGI_out_sin_norm 1
//#define SOGI_out_cos_norm 2

//#define SOGI_out_dq 1
//#define SOGI_out_theta 2
//#define SOGI_out_w_est 2
///////////////////////////////////////////////////////////////////////////////////

#define CHANNEL_SOGI_OUT_LOCKED 1


#define EMIO_PIN_SOGI_LOCKED 54
#define DEBUG_PIN_1 55
#define DEBUG_PIN_2 56
#define DEBUG_PIN_3 57

//GPIO Pointer

extern XGpio nLDAC; //AXI GPIO 0 CH1: nLDAC; CH2: duty_cycle_fan, freq_fan
extern XGpio Enable_ErrCode_Temp;
extern XGpio Duty_Cycles_Enable_Meas; //AXI GPIO 6 (2x 16 Bit)
extern XGpio Relays; // AXI GPIO 7 CH1: Duty Cycle PH3, CH2: Relais, obelix, enable driver, enable driver supply

extern XGpio Max_DC_Grid; // AXI GPIO 9 CH1: UDC_Max, IDC_Max; CH2: U_Grid_Max, I_Grid_Max
extern XGpio Max_Conv_Temp;

extern XGpio Frequency; // AXI GPIO 11: CH1: deadtime_data_a (modulator), f_data (ADCs-> sync sampling), CH2: delay_phase_2 (ADCs), delay_phase_3
extern XGpio delay;// AXI GPIO 12: CH1: ; CH2: delay_phase_1,

extern XGpio SOGI_in_q_limit_enable;

extern XGpioPs EMIO_GPIO;



#endif // MASTER_H

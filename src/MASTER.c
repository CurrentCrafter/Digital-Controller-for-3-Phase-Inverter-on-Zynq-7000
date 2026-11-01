#include "MASTER.h"


///+++++++++++++++++++++++++++++++++++++
///Global Variables and Structs
///+++++++++++++++++++++++++++++++++++++

//isr / main----------------------------

//Großbuchstaben für bessere Leserlichkeit
float Ref_ILg_d = 1.0f; // 1 A Wirkstrom
float Ref_ILg_q = 0.0f; // kein Blindstrom

float Int_err_ILg_d = 0.0f;
float Int_err_ILg_q = 0.0f;
float Prop_ILg_d = 0.0f;
float Prop_ILg_q = 0.0f;
float Error_ILg_d = 0.0f;

float Ud = 0.0f;
float Uq = 0.0f;

u8 SOGI_locked = 0;

float cos_theta = 0;
float sin_theta = 0;

float I_g_alpha = 0;
float I_g_beta = 0;

float I_g_d = 0;
float I_g_q = 0;
float U0 = 0;




//Debug---------------------------------
int index_debug = 0;
int index_sogi = 0;
struct debugging_values Debug_values[2000];
struct debug_sogi Debug_sogi[2000];

//ERROR--------------------------------
u8 Errorhandling_nEnable = 0;
u8 Modulator_Enable = 0;
u8 Measurement_Enable = 0;
u8 debug = 0x00;
u8 Error_Code = 0;

//GUI----------------------------------
u8 Message_Received = 0;
char lastStatus[UART_MESSAGE_SIZE_MAX];
char lastfeat1[UART_MESSAGE_SIZE_MAX];
char lastfeat2[UART_MESSAGE_SIZE_MAX];
char lastfeatvars[UART_MESSAGE_SIZE_MAX];
char lastLocked[UART_MESSAGE_SIZE_MAX];
u16 i=0;

u8 feature1 = 0;
u8 feature2 = 0;
int featureVars = 0;

//DQ-----------------------------------
struct dq_values dq_values;

//MEAS---------------------------------
int counter_temp = 0;
struct Measurment Measurments;

//SOGI---------------------------------
struct sogi sogi_data;

//SPI
struct oszidata oszi;

//GPIO---------------------------------
//Gpio instances
XGpio nLDAC;
XGpio Enable_ErrCode_Temp;
XGpio Duty_Cycles_Enable_Meas;
XGpio Relays;

XGpio Grid_UDC_Voltages;
XGpio Conv_Voltages;
XGpio Grid_IDC_Current;
XGpio Conv_Current;

XGpio Max_DC_Grid;
XGpio Max_Conv_Temp;

XGpio Frequency;
XGpio delay;

XGpio SOGI_in_q_limit_enable;
XGpio SOGI_out_sin_cos;
XGpio SOGI_out_dq_theta;
XGpio SOGI_out_locked_w;

XGpioPs EMIO_GPIO;


/************Main Programm****************/
int gui_send_counter = 0;
int main() {
	//DO NOT REMOVE!
    Init_Hardware();
    //***********************************************************************************************************************
    //***********************************************************************************************************************
    //***********************************************************************************************************************
    //YOUR CODE HERE:
	oszi.trigger_armed = 0;


    //UNTIL HERE
	//***********************************************************************************************************************
	//***********************************************************************************************************************
	//***********************************************************************************************************************
    while (1) {
    	//XGpioPs_WritePin(&EMIO_GPIO, DEBUG_PIN_2, 1);
    	/************************
    	 * DO NOT CHANGE FROM	*
    	 ************************/
    	//Slow changing vars
    	Read_Slow_Changing_Meas();

    	UART_Process_Received_Message();

    	if(Message_Received) {
    		UART_Write_Receive_Buffer();
    		Message_Received = 0;
    	}
    	Sogi_is_locked();
    	GUI_Send_State();
        oszi_write_all();
    	/****************************
    	 * DO NOT CHANGE UNTIL HERE	*
    	 ****************************/
        //***********************************************************************************************************************
        //***********************************************************************************************************************
        //***********************************************************************************************************************
    	//YOUR CODE HERE:






        //UNTIL HERE
        //***********************************************************************************************************************
        //***********************************************************************************************************************
        //***********************************************************************************************************************

    	//XGpioPs_WritePin(&EMIO_GPIO, DEBUG_PIN_2, 0);
    }
    return XST_SUCCESS;
}

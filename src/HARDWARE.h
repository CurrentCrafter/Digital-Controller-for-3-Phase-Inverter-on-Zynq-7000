#ifndef HARDWARE_H
#define HARDWARE_H

#include "xscugic.h"

//interrupt
#define INTC_DEVICE_ID XPAR_SCUGIC_0_DEVICE_ID
#define INTC_INTERRUPT_ID_0 XPS_FPGA0_INT_ID

extern XScuGic intr_ctl;
extern XScuGic_Config *intr_cfg; //Interrupt
extern XScuGic intr_ctl;
extern XScuGic Interrupt_Controller;
extern const u16 UARTINTID;
extern u8 relais_Mask;


//*** Function Header */
int Init_Hardware();
int Init_Interrupt();
void Init_Modulator();
void Enable_Modulator();
void Disable_Modulator();
void Write_Delay_DeltaSig();
void fan_temp_control();
void Init_GPIO();
void relais_grid_open();
void relais_grid_close();
void relais_ntc_open();
void relais_ntc_close();
void Enable_Driver();
void Sogi_Enable();
void Sogi_Disable();
//void Sogi_read_Data();
//void Sogi_read_Data_sincos_dq_locked();
//void Sogi_read_Data_sincos_dq();
void Sogi_read_Data_locked();
void Sogi_is_locked();
void write_dutycycle(float duty_1, float duty_2, float duty_3);


#endif /* HARDWARE_H */

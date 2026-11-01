#ifndef DEFINE_H
#define DEFINE_H


///+++++++++++++++++++++++++++++++++++++
///Global defined Values
///+++++++++++++++++++++++++++++++++++++

//ISR

#define SQRT3_OVER_2 0.866025
#define ONE_OVER_SQRT3 0.57735020504// (1/1.732051)
#define TWO_THIRDS 0.666666666
#define ONE_THIRD 0.33333333333
#define Ts 0.000010416667 //(1 /96000) // 48*2 kHz Regelung


//in ns*10
#define DELAY_CONV_PHASE1_DELTA_SIGMA 30
#define DELAY_CONV_PHASE2_DELTA_SIGMA 30
#define DELAY_CONV_PHASE3_DELTA_SIGMA 30

#define DELAY_GRID_PHASE1_DELTA_SIGMA 30
#define DELAY_GRID_PHASE2_DELTA_SIGMA 30
#define DELAY_GRID_PHASE3_DELTA_SIGMA 30

//Sogi
#define Q_LIMIT 160


//FAN Control
#define FREQ_CARRIER_FAN 20000
#define PWM_FAN 1 //0.2 up to 1

//MEAS
//Propfactor Delta Sigma
//HAS TO BE FLOAT
#define PROP_I_DC 1049.4
#define PROP_I_AC 793.366

//OLD PROP FACTORS (FOR 400V AC & 800V DC)
//#define PROP_UCF_L1 30.59
//#define PROP_UCF_L2 30.47
//#define PROP_UCF_L3 30.55

//#define PROP_UGRID_L1 65.29
//#define PROP_UGRID_L2 65.30
//#define PROP_UGRID_L3 65.10

//#define PROP_U_DC 30.49

#define PROP_U_DC 304.375

#define PROP_UCF_L1 302.5
#define PROP_UCF_L2 302.2625
#define PROP_UCF_L3 303.512

#define PROP_UGRID_L1 641.45
#define PROP_UGRID_L2 639.75
#define PROP_UGRID_L3 638.875

//SPI / DAC
#define MAX_DAC_VALUE 0x0FFF

#define MID 2061
#define MINV 50
#define MAXV 4065

//HARDWARE
#define FPGA_CLOCK 100	// In MHz
#define DEADTIME 10 // in Cnt Wert aktuell 200ns

//dq
#define SINE_FREQ 50.0
#define MAX_CARRIER 1042
#define MAX_DUTY_CYCLE_VALUE MAX_CARRIER 



#endif //DEFINE_H

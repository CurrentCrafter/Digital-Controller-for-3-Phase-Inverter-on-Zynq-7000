#include "MASTER.h"

//LOCAL VARIABLES MAY BE DEFINED HERE:
	float duty_1 = 0;
	float duty_2 = 0;
	float duty_3 = 0;


	const float Kp = 2.0f * M_PI * 48000 *350e-6/10;
	const float Ki = 2.0f * M_PI * 48000 * Kp/100;

	float u_1 = 0;
	float u_2 = 0;
	float u_3 = 0;
	float U_0 = 0;
	float i_d_soll = 0;
	float i_q_soll = 0;
	float i_d = 0;
	float i_q = 0;
	float i_0 = 0;
	float i_alpha = 0;
	float i_beta = 0;
	float m = 0.0f;
	float theta  = 0.0f;
	float twooverthree = 2.0f/3.0f;
	float sqrtthreeover2 = sqrt(3)/2.0f;
	float ink_theta = 0;
	float error_d = 0.0;
	float error_q = 0.0;
	float prop_d = 0.0;
	float prop_q = 0.0;
	float inte_d = 0.0;
	float inte_q = 0.0;
	float u_d = 0.0;
	float u_q = 0.0;
	float u_m_0 = 0.0;
	float u_zero = 0.0;
	float u_alpha = 0.0;
	float u_beta = 0.0;


//UNTIL HERE (Note: Define and initialize global vars in the Master.c / as extern in Master.h;
//Defines should be done in the DEFINE.h)

// Function: isr0
// Interrupt service routine for calculating the new duty cycles for each phase.
// This function is called every half switching period (96 kHz) and computes the duty cycle values for each phase
void isr0(void *CallbackRef) {
	Read_all_Measurments();


	if (Modulator_Enable && SOGI_locked){
//***********************************************************************************************************************
//***********************************************************************************************************************
//***********************************************************************************************************************
//YOUR CODE HERE
	oszi.ch1 = feature1;
	oszi.ch2 = feature2;
	oszi.ch3 = m;
	oszi.ch4 = featureVars;
	oszi.ch1_vmax = 2;
	oszi.ch2_vmax = 2;
	oszi.ch3_vmax = 10;
	oszi.ch4_vmax = 20;
	
	if(m<=10.0f){
		m+=0.01f;
	}
	if(m>10.0f){
		m=0;
	}



//UNTIL HERE
//***********************************************************************************************************************
//***********************************************************************************************************************
//***********************************************************************************************************************
	}
	 else {
		Disable_Modulator();
		duty_1 = 0;
		duty_2 = 0;
		duty_3 = 0;
		oszi.ch1 = 0;
		oszi.ch2 = 0;
		oszi.ch3 = 0;
		oszi.ch4 = 0;
	}

	write_dutycycle(duty_1, duty_2, duty_3);
}

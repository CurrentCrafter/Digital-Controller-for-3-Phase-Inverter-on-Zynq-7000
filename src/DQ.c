#include "DQ.h"
#include "MASTER.h"

//Function: dq_calc()
// Calculates all dq transformations in a round robin manner to save computation time in the interrupt service routine.
void dq_calc() {
	static u32 Counter_Value = 0;

		switch(Counter_Value) {
			case 0:
				dq_Transformation_I_Conv();
				Counter_Value++;
				break;
			case 1:
				dq_Transformation_U_Conv();
				Counter_Value++;
				break;
			case 2:
				dq_Transformation_I_Grid();
				Counter_Value++;
				break;
			case 3:
				dq_Transformation_U_Grid();
				Counter_Value++;
				break;
			default:
				Counter_Value = 0;
		}
	}

// Function: dq_invers_Transformation()
// gerneral Invers dq Transformation (dq -> abc).
void dq_invers_Transformation(u16* output, u16 d, u16 q){
    output[0] = (uint16_t)((d * cos(sogi_data.theta) - q * sin(sogi_data.theta) + MAX_DUTY_CYCLE_VALUE) / 2);
    output[1] = (uint16_t)((d * cos(sogi_data.theta - DQSTD) - q * sin(sogi_data.theta - DQSTD) + MAX_DUTY_CYCLE_VALUE) / 2);
    output[2] = (uint16_t)((d * cos(sogi_data.theta + DQSTD) - q * sin(sogi_data.theta + DQSTD) + MAX_DUTY_CYCLE_VALUE) / 2);
}

// Function: dq_Transformation_I_Conv()
// Inverse dq transformation (abc_I_conv -> dq_I_conv).
// The values are stored in the corresponding structs (Measurments, dq_values).
void dq_Transformation_I_Conv(){
	dq_values.I_d_Conv = 2.0 / 3.0 * (
	    cos(sogi_data.theta) * Measurments.I_Lf_1 +
	    cos(sogi_data.theta - DQSTD) * Measurments.I_Lf_2 +
	    cos(sogi_data.theta - DQSTD43) * Measurments.I_Lf_3
	);

	dq_values.I_q_Conv = 2.0 / 3.0 * (
	    -sin(sogi_data.theta) * Measurments.I_Lf_1 -
	    sin(sogi_data.theta - DQSTD) * Measurments.I_Lf_2 -
	    sin(sogi_data.theta - DQSTD43) * Measurments.I_Lf_3
	);
}

// Function: dq_Transformation_U_Conv()
// Inverse dq transformation (abc_U_conv -> dq_U_conv).
// The values are stored in the corresponding structs (Measurments, dq_values).
void dq_Transformation_U_Conv(){
	dq_values.U_d_Conv = 2.0 / 3.0 * (
	    cos(sogi_data.theta) * Measurments.U_Cf_1 +
	    cos(sogi_data.theta - DQSTD) * Measurments.U_Cf_2 +
	    cos(sogi_data.theta - DQSTD43) * Measurments.U_Cf_3
	);

	dq_values.U_q_Conv = 2.0 / 3.0 * (
	    -sin(sogi_data.theta) * Measurments.U_Cf_1 -
	    sin(sogi_data.theta - DQSTD) * Measurments.U_Cf_2 -
	    sin(sogi_data.theta - DQSTD43) * Measurments.U_Cf_3
	);
}

// Function: dq_Transformation_I_Grid()
// Inverse dq transformation (abc_U_grid -> dq_I_grid).
// The values are stored in the corresponding structs (Measurments, dq_values).
void dq_Transformation_I_Grid(){
	dq_values.I_d_Grid = 2.0 / 3.0 * (
		cos(sogi_data.theta) * Measurments.I_g_1 +
		cos(sogi_data.theta - DQSTD) * Measurments.I_g_2 +
		cos(sogi_data.theta - DQSTD43) * Measurments.I_g_3
	);

	dq_values.I_q_Grid = 2.0 / 3.0 * (
		    -sin(sogi_data.theta) * Measurments.I_g_1 -
		    sin(sogi_data.theta - DQSTD) * Measurments.I_g_2 -
		    sin(sogi_data.theta - DQSTD43) * Measurments.I_g_3
	);
}

// Function: dq_Transformation_I_Grid()
// Inverse dq transformation (abc_U_grid -> dq_U_grid).
// The values are stored in the corresponding structs (Measurments, dq_values).
void dq_Transformation_U_Grid(){
	dq_values.U_d_Grid = 2.0 / 3.0 * (
		    cos(sogi_data.theta) * Measurments.U_g_1 +
		    cos(sogi_data.theta - DQSTD) * Measurments.U_g_2 +
		    cos(sogi_data.theta - DQSTD43) * Measurments.U_g_3
		);

	dq_values.U_q_Grid = 2.0 / 3.0 * (
		    -sin(sogi_data.theta) * Measurments.U_g_1 -
		    sin(sogi_data.theta - DQSTD) * Measurments.U_g_2 -
		    sin(sogi_data.theta - DQSTD43) * Measurments.U_g_3
		);

}

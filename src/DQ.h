#ifndef DQ_H
#define DQ_H

#include "xil_types.h"

#define DQSTD 2.0/3.0 * M_PI
#define DQSTD43 4.0/3.0 * M_PI


// Name: dq_values
// Function: Stores all of the dq_values
struct dq_values{
	float I_d_Conv;
	float I_q_Conv;
	float U_d_Grid;
	float U_q_Grid;
	float I_d_Grid;
	float I_q_Grid;
	float U_d_Conv;
	float U_q_Conv;
}; //dq_values
extern struct dq_values dq_values;

void dq_calc();
void dq_invers_Transformation(u16* output, u16 d, u16 q);
void dq_Transformation_I_Conv();
void dq_Transformation_U_Conv();
void dq_Transformation_I_Grid();
void dq_Transformation_U_Grid();


#endif // DQ_H


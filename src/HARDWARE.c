#include "MASTER.h"

//Global used Vars
XScuGic_Config *intr_cfg; //Interrupt
XScuGic intr_ctl;
XScuGic Interrupt_Controller;

//L ftercontrol
u32 CARRIER_MAX_FAN = 2000000;

u8 relais_Mask = 0x00; //Initial all off
//Bit 0,2,4 = NTC Phase 1,2,3
//Bit 1,3,5 = Grid Phase 1,2,3

///+++++++++++++++++++++++++++++++++++++
/// Init Functions
///+++++++++++++++++++++++++++++++++++++

// Function_name: Init_Hardware()
// function for calling all of the initalisation fuctions (GPIO. SPI, UART, Errorhandling)
int Init_Hardware() {
	int status;
	Init_GPIO();
	Init_Errorhandling();
	status = Init_UART();
	status = Spi_Init(&SpiInstance, SPI_DEVICE_ID);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	//Hochfahrroutine
	Write_Delay_DeltaSig();
	Init_Modulator();
	Init_Measurement();
	Enable_Driver();
	Enable_Errorhandling(); // First enable Errorhandling THEN modulator
	sleep(2);
	Acknowledge_Error(); //Hochfahrroutige sogrt aufgrund von Timingproblemen für falsche Errormeldung

	//NUR FÜR PRAKTIKUM:
    XGpio_DiscreteWrite(&nLDAC, CHANNEL_DUTY_CYCLE_FAN, 0);
    Sogi_Enable();
	oszi.trigger_armed = 0;
    oszi.ch1_vmax = 1;
    oszi.ch2_vmax = 1;
    oszi.ch3_vmax = 1;
    oszi.ch4_vmax = 1;

	return XST_SUCCESS;
}

// Function_name: Init_Interrupt()
// The first interrupt is triggered after the last Sinc3 filter completes its measurement.
// It is called every half switching period and initiates the ISR for calculating the new DQ values.
int Init_Interrupt() {
	s32 status;
	u8 prio, trigger;
	u32 IntrMask = 0;

	XScuGic_Config *IntcConfig; /* Config for interrupt controller */

	/* Initialize the interrupt controller driver */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	status = XScuGic_CfgInitialize(&Interrupt_Controller, IntcConfig,
				       IntcConfig->CpuBaseAddress);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Connect the interrupt controller interrupt handler to the
	 * hardware interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
				     (Xil_ExceptionHandler) XScuGic_InterruptHandler,
					 &Interrupt_Controller);
    /*
     * Connect a device driver handler that will be called when an interrupt for the device occurs,
     * the device driver handler performs the specific interrupt processing for the device
	*/
    status = XScuGic_Connect((&Interrupt_Controller), UART_INT_ID, (Xil_ExceptionHandler) XUartPs_InterruptHandler,  (void *) (&UART_Driver));
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    XScuGic_Enable((&Interrupt_Controller), UART_INT_ID);

	XUartPs_SetHandler(&UART_Driver, (XUartPs_Handler) UART_Interrupt_Handler, &UART_Driver);

	IntrMask = XUARTPS_IXR_TOUT;

	XUartPs_SetInterruptMask(&UART_Driver, IntrMask);

	XUartPs_SetRecvTimeout(&UART_Driver, 8);

	XUartPs_SetOperMode(&UART_Driver, XUARTPS_OPER_MODE_NORMAL);

	XScuGic_GetPriorityTriggerType((&Interrupt_Controller), UART_INT_ID, &prio, &trigger);
	prio = 0x8;
	XScuGic_SetPriorityTriggerType((&Interrupt_Controller), UART_INT_ID, prio, trigger);


    status = XScuGic_Connect((&Interrupt_Controller), INTC_INTERRUPT_ID_0, (Xil_ExceptionHandler) isr0, (void *) &Interrupt_Controller);
    if (status != XST_SUCCESS) {
        return XST_FAILURE;
    }

	XScuGic_GetPriorityTriggerType((&Interrupt_Controller), INTC_INTERRUPT_ID_0, &prio, &trigger);
	trigger = 0x3;
	prio = 0x0;
	XScuGic_SetPriorityTriggerType((&Interrupt_Controller), INTC_INTERRUPT_ID_0, prio, trigger);

    XScuGic_Enable((&Interrupt_Controller), INTC_INTERRUPT_ID_0);

	Xil_ExceptionEnable();

	return XST_SUCCESS;
}


//Function_name: Init_Modulator)(
//Initializes all Modulator functions
// Calculates the frequency and deadtime values for the FPGA modulator
// and writes them combined into a 32-bit mask to the corresponding GPIO channel.
// - Frequency is calculated based on the FPGA clock and carrier frequency.
// - Deadtime is a predefined constant.
void Init_Modulator() {

	u32 Mask = 0x00;
	u16 Carrier_MAX = (u16) MAX_CARRIER;

	Mask = DEADTIME;
	Mask = (Mask << 16) | Carrier_MAX;

	XGpio_DiscreteWrite(&Frequency, CHANNEL_FREQUENCY, Mask);

	//Tastgrad initial auf 0 setzen.
	XGpio_DiscreteWrite(&Duty_Cycles_Enable_Meas, CHANNEL_DUTY_CYCLE_PH12, 0);
	XGpio_DiscreteWrite(&Duty_Cycles_Enable_Meas, CHANNEL_DUTY_CYCLE_PH3_ENABLE_MEAS, 0);
}

//Function_name: enable_Sogi()
//Enables the Sogi
//Also sets q_limit
void Sogi_Enable() {
	u32 Mask = ((u32) Q_LIMIT) << 1; // 17 Bit maske: 1:16 = q_limit ; 0 = enable_clk
	//eintrages des q_limit wertes und //konvertierung in Intwert

	Mask |= 1; //enable auf 16

	XGpio_DiscreteWrite(&SOGI_in_q_limit_enable, SOGI_in, Mask);
	sogi_data.locked = 0; //Sets locked to 0 to be sure it truly locked
}

//Function_name: disable_Sogi()
//Disables the Sogi
void Sogi_Disable() {
	u32 Mask = ((u32) Q_LIMIT ) << 1; // 17 Bit maske: 1:16 = q_limit ; 0 = enable_clk
	//eintrages des q_limit wertes und //konvertierung in Intwert
	Mask |= 0; //disable auf 16
	XGpio_DiscreteWrite(&SOGI_in_q_limit_enable, SOGI_in, Mask);
}

//Function_name: Sogi_is_locked()
//Checks if Sogi is locked
//Returns 1 if truly locked ; returns 0 if not
void Sogi_is_locked() {
	if(sogi_data.locked == 1 && (Error_Code<=2)){
		SOGI_locked = 1;
	}
	if(Error_Code>2){
		SOGI_locked = 0;
	}

}

// Function_name: Init_GPIO()
// Function for the Initalisation of all the GPIO Pins
void Init_GPIO() {
	XGpio_Config *GPIO_NLADC_CONFIG;
	XGpio_Config *GPIO_ENABLE_CONFIG;
	XGpio_Config *GPIO_DUTY_CYCLES_ENABLE_MEAS_CONFIG;
	XGpio_Config *GPIO_RELAYS_CONFIG;
	XGpio_Config *GPIO_MAX_DC_GRID;
	XGpio_Config *GPIO_MAX_CONV_TEMP;
	XGpio_Config *GPIO_Frequency;
	XGpio_Config *GPIO_DELAY;
	XGpio_Config *GPIO_SOGI_IN;

	// Hier muss XGpio statt XGpioPs verwendet werden!
	GPIO_NLADC_CONFIG = XGpio_LookupConfig(XPAR_AXI_GPIO_0_DEVICE_ID);

	GPIO_ENABLE_CONFIG = XGpio_LookupConfig(XPAR_AXI_GPIO_3_DEVICE_ID);

	GPIO_DUTY_CYCLES_ENABLE_MEAS_CONFIG = XGpio_LookupConfig(XPAR_AXI_GPIO_6_DEVICE_ID);

	GPIO_RELAYS_CONFIG = XGpio_LookupConfig(XPAR_AXI_GPIO_7_DEVICE_ID);

	GPIO_MAX_DC_GRID = XGpio_LookupConfig(XPAR_AXI_GPIO_9_DEVICE_ID);

	GPIO_MAX_CONV_TEMP = XGpio_LookupConfig(XPAR_AXI_GPIO_10_DEVICE_ID);

	GPIO_Frequency = XGpio_LookupConfig(XPAR_AXI_GPIO_11_DEVICE_ID);

	GPIO_DELAY = XGpio_LookupConfig(XPAR_AXI_GPIO_12_DEVICE_ID);

	GPIO_SOGI_IN = XGpio_LookupConfig(XPAR_AXI_GPIO_13_DEVICE_ID);

	XGpio_CfgInitialize(&nLDAC, GPIO_NLADC_CONFIG, GPIO_NLADC_CONFIG->BaseAddress);

	XGpio_CfgInitialize(&Enable_ErrCode_Temp, GPIO_ENABLE_CONFIG, GPIO_ENABLE_CONFIG->BaseAddress);

	XGpio_CfgInitialize(&Duty_Cycles_Enable_Meas, GPIO_DUTY_CYCLES_ENABLE_MEAS_CONFIG, GPIO_DUTY_CYCLES_ENABLE_MEAS_CONFIG->BaseAddress);

	XGpio_CfgInitialize(&Relays, GPIO_RELAYS_CONFIG, GPIO_RELAYS_CONFIG->BaseAddress);

	XGpio_CfgInitialize(&Max_DC_Grid, GPIO_MAX_DC_GRID,	GPIO_MAX_DC_GRID->BaseAddress);

	XGpio_CfgInitialize(&Max_Conv_Temp, GPIO_MAX_CONV_TEMP, GPIO_MAX_CONV_TEMP->BaseAddress);

	XGpio_CfgInitialize(&Frequency, GPIO_Frequency, GPIO_Frequency->BaseAddress);

	XGpio_CfgInitialize(&delay, GPIO_DELAY, GPIO_DELAY->BaseAddress);

	XGpio_CfgInitialize(&SOGI_in_q_limit_enable, GPIO_SOGI_IN, GPIO_SOGI_IN->BaseAddress);

	// Setze den Pin als Ausgang (0 = Output im VHDL)
	XGpio_SetDataDirection(&nLDAC, CHANNEL_nLADC, 0);
	XGpio_SetDataDirection(&nLDAC, CHANNEL_DUTY_CYCLE_FAN, 0);

	XGpio_SetDataDirection(&Enable_ErrCode_Temp, CHANNEL_ERRORCODE_TEMP, 1);
	XGpio_SetDataDirection(&Enable_ErrCode_Temp, CHANNEL_ENABLE, 0);

	XGpio_SetDataDirection(&Duty_Cycles_Enable_Meas, CHANNEL_DUTY_CYCLE_PH12, 0);
	XGpio_SetDataDirection(&Duty_Cycles_Enable_Meas, CHANNEL_DUTY_CYCLE_PH3_ENABLE_MEAS, 0);

	//XGpio_SetDataDirection(&Relays,	CHANNEL_DUTY_CYCLE_PH3_RELAYS, 0);
	XGpio_SetDataDirection(&Relays, CHANNEL_RELAYS, 0);

	XGpio_SetDataDirection(&Max_DC_Grid, CHANNEL_MAX_DC, 0);
	XGpio_SetDataDirection(&Max_DC_Grid, CHANNEL_MAX_GRID, 0);

	XGpio_SetDataDirection(&Max_Conv_Temp, CHANNEL_MAX_CONV, 0);
	XGpio_SetDataDirection(&Max_Conv_Temp, CHANNEL_MAX_TEMP, 0);

	XGpio_SetDataDirection(&Frequency, CHANNEL_FREQUENCY, 0);

	XGpio_SetDataDirection(&delay, CHANNEL_DELAY_CONV, 0);
	XGpio_SetDataDirection(&delay, CHANNEL_DELAY_GRID, 0);

	XGpio_SetDataDirection(&SOGI_in_q_limit_enable, SOGI_in, 0);


	XGpioPs_Config *ConfigPtr;

	ConfigPtr = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);
	XGpioPs_CfgInitialize(&EMIO_GPIO, ConfigPtr, ConfigPtr->BaseAddr);

	XGpioPs_SetDirectionPin(&EMIO_GPIO, EMIO_PIN_SOGI_LOCKED, 0);
	XGpioPs_SetDirectionPin(&EMIO_GPIO, DEBUG_PIN_1, 1);
	XGpioPs_SetDirectionPin(&EMIO_GPIO, DEBUG_PIN_2, 1);
	XGpioPs_SetDirectionPin(&EMIO_GPIO, DEBUG_PIN_3, 1);
}

///+++++++++++++++++++++++++++++++++++++
/// Modulator Control
///+++++++++++++++++++++++++++++++++++++

// Function: Enable_Modulator()
// Enables the modulator on the PL to generate the switching parameters.
// Note: The modulator is only enabled if the error code equals 2 (no error).
// If the error code is any other value, the modulator will not start.
void Enable_Modulator() {
	u32 bitstream;
	u8 Acknowledge_Error_var = 0;
	Modulator_Enable = 1;
	bitstream = Errorhandling_nEnable | (Modulator_Enable << 1)	| (Acknowledge_Error_var << 2);
	XGpio_DiscreteWrite(&Enable_ErrCode_Temp, CHANNEL_ENABLE, bitstream);
}

// Function: Disable_Modulator()
// Disables the modulator on the PL to generate the switching parameters.
void Disable_Modulator() {
	u32 bitstream;
	u8 Acknowledge_Error_var = 0;
	Modulator_Enable = 0;
	bitstream = Errorhandling_nEnable | (Modulator_Enable << 1)	| (Acknowledge_Error_var << 2);
	XGpio_DiscreteWrite(&Enable_ErrCode_Temp, CHANNEL_ENABLE, bitstream);
}

//writes the dutycycle for the sixpack
//input [0:1]
void write_dutycycle(float duty_1, float duty_2, float duty_3){
	uint16_t duty[3];
	// Sicherheitscheck
	if (duty_1 > 1.0f)
		duty_1 = 1.0f;
	else if (duty_1 < 0.0f)
		duty_1 = 0.0f;

	if (duty_2 > 1.0f)
		duty_2 = 1.0f;
	else if (duty_2 < 0.0f)
		duty_2 = 0.0f;

	if (duty_3 > 1.0f)
		duty_3 = 1.0f;
	else if (duty_3 < 0.0f)
		duty_3 = 0.0f;

	duty[0] = (uint16_t) (duty_1 * MAX_DUTY_CYCLE_VALUE);
	duty[1] = (uint16_t) (duty_2 * MAX_DUTY_CYCLE_VALUE);
	duty[2] = (uint16_t) (duty_3 * MAX_DUTY_CYCLE_VALUE);

	u32 tmp = (duty[1] << 16) | duty[0];
	XGpio_DiscreteWrite(&Duty_Cycles_Enable_Meas, CHANNEL_DUTY_CYCLE_PH12, tmp);
	tmp = (Measurement_Enable << 16) | duty[2];
	XGpio_DiscreteWrite(&Duty_Cycles_Enable_Meas, CHANNEL_DUTY_CYCLE_PH3_ENABLE_MEAS, tmp);
}

///+++++++++++++++++++++++++++++++++++++
/// FPGA Configuration
///+++++++++++++++++++++++++++++++++++++

// Function: Write_Delay()
// Packs and writes delay values for the sinc3 filters of the grid and converter measurments of all three phases into GPIO channels.
// The delay values for grid and converter are combined into 32-bit masks, each containing two 16-bit delays.
// These masks are written to specific GPIO channels corresponding to different delay registers.
void Write_Delay_DeltaSig() {
	u32 Mask_Conv = 0x00;
	u32 Mask_Grid = 0x00;

	u8 delay_conv_1 = (u8) DELAY_CONV_PHASE1_DELTA_SIGMA;
	u8 delay_conv_2 = (u8) DELAY_CONV_PHASE2_DELTA_SIGMA;
	u8 delay_conv_3 = (u8) DELAY_CONV_PHASE3_DELTA_SIGMA;

	u8 delay_grid_1 = (u8) DELAY_GRID_PHASE1_DELTA_SIGMA;
	u8 delay_grid_2 = (u8) DELAY_GRID_PHASE2_DELTA_SIGMA;
	u8 delay_grid_3 = (u8) DELAY_GRID_PHASE3_DELTA_SIGMA;

	Mask_Conv = (delay_conv_3 << 14) | (delay_conv_2 << 7) | delay_conv_1;

	Mask_Grid = (delay_grid_3 << 14) | (delay_grid_2 << 7) | delay_grid_1;


	XGpio_DiscreteWrite(&delay, CHANNEL_DELAY_CONV, Mask_Conv);
	XGpio_DiscreteWrite(&delay, CHANNEL_DELAY_GRID, Mask_Grid);

}

///+++++++++++++++++++++++++++++++++++++
/// Hardware Interaction
///+++++++++++++++++++++++++++++++++++++

//Function: fan_temp_control()
//Controls the cooling fan speed based on the temperature reading from the FPGA.
//The fan speed is adjusted in discrete steps according to predefined temperature ranges,
void fan_temp_control() {
	u32 Mask = 0x00;
	//u16 Pwm_Fan = (u16) (FAN_FREQUENCY * PWM_FAN); // for default use only
	u16 Pwm_Fan = 0;
	u8 temp = Measurments.Temperature;

	switch (temp) {
	case 0 ... 30:
		Pwm_Fan = (u16) (CARRIER_MAX_FAN * 0); //0% duty cycle
		break;
	case 31 ... 40:
		Pwm_Fan = (u16) (CARRIER_MAX_FAN * 0.5); //50% duty cycle
		break;
	case 41 ... 50:
		Pwm_Fan = (u16) (CARRIER_MAX_FAN * 0.8); //80% duty cycle
		break;
	case 51 ... 255:
		Pwm_Fan = CARRIER_MAX_FAN; //100% duty cycle
		break;
	default:
		Pwm_Fan = CARRIER_MAX_FAN; //safety 100% duty cycle
		break;
	}

	Mask = CARRIER_MAX_FAN;
	Mask = (Mask << 16) | Pwm_Fan;
	XGpio_DiscreteWrite(&nLDAC, CHANNEL_DUTY_CYCLE_FAN, Mask);
}

//++++++ Relais Control ++++++

//Grid == Channel 2
void relais_grid_open() {
	relais_Mask = relais_Mask | (1 << 1); //PH1 Grid auf 1 setzen
	relais_Mask = relais_Mask | (1 << 3); //PH2 Grid auf 1 setzen
	relais_Mask = relais_Mask | (1 << 5); //PH3 Grid auf 1 setzen

	XGpio_DiscreteWrite(&Relays, CHANNEL_RELAYS, relais_Mask);
}

void relais_grid_close() {
	relais_Mask = relais_Mask | (0 << 1); //PH1 Grid auf 0 setzen
	relais_Mask = relais_Mask | (0 << 3); //PH2 Grid auf 0 setzen
	relais_Mask = relais_Mask | (0 << 5); //PH3 Grid auf 0 setzen

	XGpio_DiscreteWrite(&Relays, CHANNEL_RELAYS, relais_Mask);
}

//NTC == Channel 1
void relais_ntc_open() {
	relais_Mask = relais_Mask | (1 << 0); //PH1 NTC auf 1 setzen
	relais_Mask = relais_Mask | (1 << 2); //PH2 NTC auf 1 setzen
	relais_Mask = relais_Mask | (1 << 4); //PH3 NTC auf 1 setzen

	XGpio_DiscreteWrite(&Relays, CHANNEL_RELAYS, relais_Mask);

}

void relais_ntc_close() {

	relais_Mask = relais_Mask | (0 << 0); //PH1 NTC auf 0 setzen
	relais_Mask = relais_Mask | (0 << 2); //PH2 NTC auf 0 setzen
	relais_Mask = relais_Mask | (0 << 4); //PH3 NTC auf 0 setzen

	XGpio_DiscreteWrite(&Relays, CHANNEL_RELAYS, relais_Mask);
}

//Function: enable_Driver()
//Enables the gate driver by setting the appropriate bits in the relay mask and writing it to the GPIO.
void Enable_Driver() {
	u16 Mask = 0b0011000000; //Treibersupply und Treiber enable
	XGpio_DiscreteWrite(&Relays, CHANNEL_RELAYS, Mask);
}

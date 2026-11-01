//#include "GUI.h"
#include "MASTER.h"
#include <stdlib.h>

int counter = 0;

// Function: gui_Find_Command()
// This function compares the first characters of 'str' with the entire
// string 'str2'. It returns 1 (true) if 'str' starts with 'str2', otherwise 0 (false).
u8 GUI_Find_Command(const char *str, const char *str2) {
	return strncmp(str, str2, strlen(str2)) == 0;
}

// Function: GUI_UART_Flush()
// Sends all bytes from the receive buffer via UART as long as the UART transmitter is empty.
// It repeatedly pops bytes from the buffer and sends them until the buffer is empty or UART is busy.
//void GUI_UART_Flush() {
//	uint8_t b;
//
//	while (XUartPs_IsTransmitEmpty(&UART_Driver) && rb_pop(&b)) {
//		XUartPs_SendByte(UART_Driver.Config.BaseAddress, b);
//	}
//}

//*****************************************************************************/
//Functions for sending Data to the GUI
//*****************************************************************************/

//Function: GUI_Send_State()
//Sends the current error code state to the GUI if it has changed.
//
void GUI_Send_State() {
    char buf[UART_MESSAGE_SIZE_MAX];
    int len = sprintf(buf, "<VALUE_ERROR_CODE=%d>", Error_Code);

    if (memcmp(lastStatus, buf, len) != 0){
	if ((len) <= rb_free_space()) {
		// Puffer passt - Daten einf gen
		for (int i = 0; i < len; ++i) {
			rb_push((uint8_t) buf[i]);
		}
		strcpy(lastStatus, buf);
	}else{
		//ERRORMESSAGE
	}
    }

	// Nur wenn UART gerade sendet, nichts tun
	// Wenn UART leer ist, sende das 1. Byte an
	if (XUartPs_IsTransmitEmpty(&UART_Driver)) {
		uint8_t b;
		if (rb_pop(&b)) {
			XUartPs_SendByte(UART_Driver.Config.BaseAddress, b);
//	             Nun wird der UART-Interrupt-Handler weitersenden
		}
	}


	//Send feature1 and feature2
	GUI_Send_feat1();
	GUI_Send_feat2();
	GUI_send_vars();
	GUI_Send_locked();

}


void GUI_Send_locked() {
    char buf[UART_MESSAGE_SIZE_MAX];
    int len = sprintf(buf, "<SOGI_LOCKED=%d>", SOGI_locked);

    if (memcmp(lastLocked, buf, len) != 0){
	if ((len) <= rb_free_space()) {
		// Puffer passt - Daten einf gen
		for (int i = 0; i < len; ++i) {
			rb_push((uint8_t) buf[i]);
		}
		strcpy(lastLocked, buf);
	}
    }


	// Nur wenn UART gerade sendet, nichts tun
	// Wenn UART leer ist, sende das 1. Byte an
	if (XUartPs_IsTransmitEmpty(&UART_Driver)) {
		uint8_t b;
		if (rb_pop(&b)) {
			XUartPs_SendByte(UART_Driver.Config.BaseAddress, b);
//	             Nun wird der UART-Interrupt-Handler weitersenden
		}
	}
}

void GUI_Send_feat1(){
	char buf[UART_MESSAGE_SIZE_MAX];
	int len = sprintf(buf, "<feature1=%d>", feature1);

	    if (memcmp(lastfeat1, buf, len) != 0){
		if ((len) <= rb_free_space()) {
			// Puffer passt - Daten einf gen
			for (int i = 0; i < len; ++i) {
				rb_push((uint8_t) buf[i]);
			}
			strcpy(lastfeat1, buf);
	    }
	    }
		// Nur wenn UART gerade sendet, nichts tun
		// Wenn UART leer ist, sende das 1. Byte an
		if (XUartPs_IsTransmitEmpty(&UART_Driver)) {
			uint8_t b;
			if (rb_pop(&b)) {
				XUartPs_SendByte(UART_Driver.Config.BaseAddress, b);
	//	             Nun wird der UART-Interrupt-Handler weitersenden
			}
		}
}

void GUI_Send_feat2(){
	char buf[UART_MESSAGE_SIZE_MAX];
	int len = sprintf(buf, "<feature2=%d>", feature2);

	    if (memcmp(lastfeat2, buf, len) != 0){
		if ((len) <= rb_free_space()) {
			// Puffer passt - Daten einf gen
			for (int i = 0; i < len; ++i) {
				rb_push((uint8_t) buf[i]);
			}
			strcpy(lastfeat2, buf);
	    }
	    }
		// Nur wenn UART gerade sendet, nichts tun
		// Wenn UART leer ist, sende das 1. Byte an
		if (XUartPs_IsTransmitEmpty(&UART_Driver)) {
			uint8_t b;
			if (rb_pop(&b)) {
				XUartPs_SendByte(UART_Driver.Config.BaseAddress, b);
	//	             Nun wird der UART-Interrupt-Handler weitersenden
			}
		}
}

void GUI_send_vars(){
	char buf[UART_MESSAGE_SIZE_MAX];
		int len = sprintf(buf, "<variable=%d>", featureVars);

		    if (memcmp(lastfeatvars, buf, len) != 0){
			if ((len) <= rb_free_space()) {
				// Puffer passt - Daten einf gen
				for (int i = 0; i < len; ++i) {
					rb_push((uint8_t) buf[i]);
				}
				strcpy(lastfeatvars, buf);
		    }
		    }
			// Nur wenn UART gerade sendet, nichts tun
			// Wenn UART leer ist, sende das 1. Byte an
			if (XUartPs_IsTransmitEmpty(&UART_Driver)) {
				uint8_t b;
				if (rb_pop(&b)) {
					XUartPs_SendByte(UART_Driver.Config.BaseAddress, b);
		//	             Nun wird der UART-Interrupt-Handler weitersenden
				}
			}

}

//Function: GUI_Send_Last_I_Values()
//Sends the last recorded current values to the GUI for a given index.
void GUI_Send_Last_I_Values(uint16_t index) {
	char buf[UART_MESSAGE_SIZE_MAX];
	int len = sprintf(buf, "%.3f;%.3f;%.3f;%.3f;%.3f;%.3f;%.3f;",
			Debug_values[index].I_DC, Debug_values[index].I_Lf_1,
			Debug_values[index].I_Lf_2, Debug_values[index].I_Lf_3,
			Debug_values[index].I_g_1, Debug_values[index].I_g_2,
			Debug_values[index].I_g_3);

	for (int i = 0; i < len; ++i) {
		rb_push((uint8_t) buf[i]);
	}

	uint8_t b;
	if (XUartPs_IsTransmitEmpty(&UART_Driver) && rb_pop(&b)) {
		XUartPs_SendByte(UART_Driver.Config.BaseAddress, b);
	}
}

//Function: GUI_Send_Last_U_Values()
//Sends the last recorded voltage values to the GUI for a given index.
void GUI_Send_Last_U_Values(uint16_t index) {
	char buf[UART_MESSAGE_SIZE_MAX];
	int len = sprintf(buf, ";%.3f;%.3f;%.3f;%.3f;%.3f;%.3f;%.3f",
			Debug_values[index].U_DC, Debug_values[index].U_Cf_1,
			Debug_values[index].U_Cf_2, Debug_values[index].U_Cf_3,
			Debug_values[index].U_g_1, Debug_values[index].U_g_2,
			Debug_values[index].U_g_3);

	for (int i = 0; i < len; ++i) {
		rb_push((uint8_t) buf[i]);
	}

	uint8_t b;
	if (XUartPs_IsTransmitEmpty(&UART_Driver) && rb_pop(&b)) {
		XUartPs_SendByte(UART_Driver.Config.BaseAddress, b);
	}
}

//Function: GUI_Send_Last_DQ_Values()
//Sends the last recorded DQ transformation values to the GUI for a given index.
void GUI_Send_Last_DQ_Values(uint16_t index) {
	char buf[UART_MESSAGE_SIZE_MAX];
	int len = sprintf(buf, ";;%.3f;%.3f;%.3f;%.3f;%.3f;%.3f;%i",
			Debug_values[index].I_d_Conv, Debug_values[index].I_q_Conv,
			Debug_values[index].I_d_Grid, Debug_values[index].I_q_Grid,
			Debug_values[index].U_d_Conv, Debug_values[index].U_q_Conv, index);

	for (int i = 0; i < len; ++i) {
		rb_push((uint8_t) buf[i]);
	}

	uint8_t b;
	if (XUartPs_IsTransmitEmpty(&UART_Driver) && rb_pop(&b)) {
		XUartPs_SendByte(UART_Driver.Config.BaseAddress, b);
	}
}

//Eventuell Mittelung der Temp einbauen
//Function: GUI_Send_Temperature()
//Sends the current temperature measurement to the GUI.
void GUI_Send_Temperature() {
	char buf[UART_MESSAGE_SIZE_MAX];
	int len = sprintf(buf, "<VALUE_TEMP=%d>", Measurments.Temperature); // urspr nglich: "<VALUE_TEMP=%f>"
	for (int i = 0; i < len; ++i) {
		rb_push((uint8_t) buf[i]);
	}

	uint8_t b;
	if (XUartPs_IsTransmitEmpty(&UART_Driver) && rb_pop(&b)) {
		XUartPs_SendByte(UART_Driver.Config.BaseAddress, b);
	}
}

//Function: GUI_Send_Sogi()
//Sends the sogi Values to the GUI/UART.
void GUI_Send_Sogi() {
	char message1[UART_MESSAGE_SIZE_MAX];
	char message2[UART_MESSAGE_SIZE_MAX];
	int len1;
	int len2;

	len1 = snprintf(message1, UART_MESSAGE_SIZE_MAX, "%.3f;%.3f;%.3f;%.3f", //sprintf bur with overflow protection
			sogi_data.sin_norm, sogi_data.cos_norm, sogi_data.d, sogi_data.q
			//sogi_data.w_est,
			//sogi_data.theta,
			//(unsigned int)sogi_data.locked
			);

	len2 = snprintf(message2, UART_MESSAGE_SIZE_MAX, ";%.3f;%.3f;%u\r",
			sogi_data.w_est, sogi_data.theta, (unsigned int) sogi_data.locked);

	if ((len1+len2) <= rb_free_space()) {
		// Puffer passt - Daten einf gen
		for (int i = 0; i < len1; ++i) {
			rb_push((uint8_t) message1[i]);
		}
		for (int i = 0; i < len2; ++i) {
			rb_push((uint8_t) message2[i]);
		}
	}else{
		//ERRORMESSAGE
	}

	// Nur wenn UART gerade sendet, nichts tun
	// Wenn UART leer ist, sende das 1. Byte an
	if (XUartPs_IsTransmitEmpty(&UART_Driver)) {
		uint8_t b;
		if (rb_pop(&b)) {
			XUartPs_SendByte(UART_Driver.Config.BaseAddress, b);
//	             Nun wird der UART-Interrupt-Handler weitersenden
		}
	}
}


//Function: GUI_Master()
//Example Function for future Use
//Sends Values via Uart
void GUI_Master() {
	char message[UART_MESSAGE_SIZE_MAX];
	int len;

	// Beispiel 1: Sogi-Daten senden
	len = snprintf(message, UART_MESSAGE_SIZE_MAX, "Test;%.3f;%.3f;%.3f;%.3f",
			sogi_data.sin_norm, sogi_data.cos_norm, sogi_data.d, sogi_data.q);

	// Pr fe, ob gen gend Platz im TX-Puffer ist
	if (len <= rb_free_space()) {
		// Puffer passt - Daten einf gen
		for (int i = 0; i < len; ++i) {
			rb_push((uint8_t) message[i]);
		}
	}else{
		//ERRORMESSAGE
		return; // oder entscheiden: R ckgabe, Retry sp ter, etc.
	}

}

// Function: GUI_Execute_Command()
// Parses and executes commands received via UART.
// Supported commands start with "SET" and include:
// - ENABLE_ERRORHANDLING: Enables or disables error handling based on the value (0x01 = enable).
// - ENABLE_MODULATOR: Enables or disables the modulator (0x01 = enable).
// - MODULATION_INDEX: Sets the modulation index within valid range (0 to 0x412).
// - ACKNOWLEDGE_ERROR: User acknowledges error to allow modulator restart.
// - EN_MOD: Abbreviated command to enable/disable modulator (1 = enable, 0 = disable).
// - ACKN_ERR: Abbreviated command to acknowledge error (no value needed).
//
// After processing the command, the function updates the GUI by calling
// GUI_Recived_Data() and GUI_Send_State().
void GUI_Execute_Command(u8* Command) {

    if (GUI_Find_Command((const char*) Command, "SET")) {
        if (GUI_Find_Command((const char*) Command + strlen("SET_"),
                "EN_MOD")) {
            // New abbreviated command for enabling/disabling modulator
            u16 Test = atoi((const char*) Command + strlen("SET_EN_MOD = "));
            debug = (u8) Test;
            if (Test == 1) {
                Enable_Modulator();
            } else if (Test == 0) {
                index_debug = 1;
                Disable_Modulator();
            }
        } else if (GUI_Find_Command((const char*) Command + strlen("SET_"),"ACKN_ERR")) {
            Acknowledge_Error();
        }
    }
    if (GUI_Find_Command((const char*) Command, "feature")) {
           if (GUI_Find_Command((const char*) Command + strlen("feature"),
                   "1")) {
               // New abbreviated command for enabling/disabling modulator
               u16 Test = atoi((const char*) Command + strlen("feature1="));
               debug = (u8) Test;
               if (Test == 1) {
            	   feature1 = 1;
               } else if (Test == 0) {
            	   feature1 = 0;
               }
           } else if (GUI_Find_Command((const char*) Command + strlen("feature"),
                   "2")) {
               u16 Test = atoi((const char*) Command + strlen("feature2="));
               debug = (u8) Test;
               if (Test == 1) {
            	   feature2 = 1;
               } else if (Test == 0) {
            	   feature2 = 0;
               }
           }
       }
    if (GUI_Find_Command((const char*) Command, "variable=")) {
    	 if (GUI_Find_Command((const char*) Command + strlen("variable="),
    	                   "+1")) {
    	               featureVars ++;
    	           }
    	 else if (GUI_Find_Command((const char*) Command + strlen("variable="),
    	                   "-1")) {
    	        	   featureVars --;
    	           }
    }


    //GUI_Recived_Data();
    //GUI_Send_State();
}

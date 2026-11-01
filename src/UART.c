
#include "MASTER.h"
#include "UART.h"
#include <string.h>

XUartPs UART_Driver;
struct uart_ringbuffer uart_tx_rb = { .head = 0, .tail = 0 }; //Definition
struct uart_buffer uart_rx_buf = { .idx = 0 };

// Function_name: Init_UART()
// Function for the Initialization of UART
int Init_UART()
{
    int status;
    XUartPs_Config *UART_Config;

    // Initialize UART driver
    UART_Config = XUartPs_LookupConfig(UART_DEVICE_ID);
    if (!UART_Config) return XST_FAILURE;

    status = XUartPs_CfgInitialize(&UART_Driver, UART_Config, UART_Config->BaseAddress);
    if (status != XST_SUCCESS) return XST_FAILURE;

    status = XUartPs_SelfTest(&UART_Driver);
    if (status != XST_SUCCESS) return XST_FAILURE;

    XUartPs_SetBaudRate(&UART_Driver, UART_BAUDRATE);

    // Interrupt setup
    status = Init_Interrupt();
    if (status != XST_SUCCESS) return XST_FAILURE;

    for (int i = 0; i < UART_RX_BUFFER_SIZE; i++) {
    	uart_rx_buf.buf[i] = '0';
    }
    // Enable RX-timeout and TX-empty interrupts
    //XUartPs_SetInterruptMask(&UART_Driver, XUARTPS_IXR_RXFULL | XUARTPS_IXR_TOUT);
    //XUartPs_SetRecvTimeout(&UART_Driver, 10);


    return XST_SUCCESS;
}

// Function Name: UART_Interrupt_Handler()
// Handles UART protocol interrupts. Valid received commands must begin with '<' and end with '>'.
// If the format is invalid, the reception is aborted.
// Returns uart_rx_buf.buf if a complete command (ending with '>') is received, otherwise NULL.
void UART_Interrupt_Handler(void *CallBackRef, u32 Event, unsigned int EventData) {
	Message_Received = 1;
}

// Function: UART_Process_Received_Command()
// Checks the UART RX buffer for a complete command (ending with '>').
// Extracts the command, calls GUI_Execute_Command to process it, and cleans up the buffer.
// This function should be called periodically (e.g., in the main loop) to handle received commands.
void UART_Write_Receive_Buffer() {
//	u8 byte = 0;
	while (XUartPs_IsReceiveData(UART_Driver.Config.BaseAddress)) {
		uart_rx_buf.buf[uart_rx_buf.idx] = XUartPs_RecvByte(UART_Driver.Config.BaseAddress);

        if (uart_rx_buf.idx < UART_RX_BUFFER_SIZE - 1) {
			uart_rx_buf.idx++;
		} else {
			uart_rx_buf.idx = 0;
		}
    }
}


void UART_Process_Received_Message() {
	int start_idx = -1, end_idx = 0, idx = uart_rx_buf.idx;

	// Search for '<' and '>' as the command start and end delimiter
    for (int i = 0; i < UART_RX_BUFFER_SIZE; ++i) {
    	//Message-Start found
    	if (uart_rx_buf.buf[idx] == '<') {
        	//Start of command
        	start_idx = idx;

		//Entire Message found
        } else if (uart_rx_buf.buf[idx] == '>' && start_idx >= 0) {
        	end_idx = idx;

        	// Complete command found (from start_idx to end of buffer)
        	if (start_idx < end_idx) {
				char command[UART_MESSAGE_SIZE_MAX];
				int len = end_idx - start_idx - 1; //Length without < and >

				if (len < UART_MESSAGE_SIZE_MAX) {
					memcpy(command, uart_rx_buf.buf + start_idx + 1, len);
					command[len] = '\0';  // Null-terminate for safety
					// Process command
					GUI_Execute_Command((u8*)command);
				}

				// Remove Command from the end of the buffer
				for (int idx = start_idx; idx <= end_idx; idx++) {
					uart_rx_buf.buf[idx] = '0';
				}
			// Complete command found (from start_idx to end of buffer + from 0 to end_idx)
        	} else {
				char command[UART_MESSAGE_SIZE_MAX];
				int len_first_half = UART_RX_BUFFER_SIZE - start_idx - 1; //Length without <
				int len_second_half = end_idx; //Length without >

				if (len_first_half + len_second_half < UART_MESSAGE_SIZE_MAX) {
					memcpy(command, uart_rx_buf.buf + start_idx + 1, len_first_half);
					memcpy(command + len_first_half, uart_rx_buf.buf, len_second_half);
					command[len_first_half + len_second_half] = '\0';  // Null-terminate for safety
					// Process the command
					GUI_Execute_Command((u8*)command);
				}
				// Remove Command from the end of the buffer
				for (int idx = start_idx; idx < UART_RX_BUFFER_SIZE; idx++) {
					uart_rx_buf.buf[idx] = '0';
				}
				// Remove Command from the end of the buffer
				for (int idx = 0; idx <= end_idx; idx++) {
					uart_rx_buf.buf[idx] = '0';
				}
        	}

            break;  // Process only one command per call to avoid blocking
        }

    	if (idx < UART_RX_BUFFER_SIZE - 1) {
    		idx++;
    	} else {
    		idx = 0;
    	}
    }
}

// Gibt die Anzahl der *freien* Bytes zum Schreiben zurück
size_t rb_free_space() {
    // Buffer ist voll, wenn (tail + 1) % size == head → dann bleibt 1 Byte ungenutzt
    if (uart_tx_rb.head == uart_tx_rb.tail) {
        // Buffer ist leer → komplett frei bis auf 1 Byte Guard
        return UART_TX_BUFFER_SIZE - 1;
    } else if (uart_tx_rb.head < uart_tx_rb.tail) {
        // head ---- tail --> freier Raum am Anfang
        return uart_tx_rb.head + UART_TX_BUFFER_SIZE - uart_tx_rb.tail - 1;
    } else {
        // tail ---- head --> freier Raum zwischen tail und head
        return uart_tx_rb.head - uart_tx_rb.tail - 1;
    }
}

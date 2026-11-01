#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include "MASTER.h"
#include "xuartps.h"

#define UART_DEVICE_ID XPAR_XUARTPS_0_DEVICE_ID
#define UART_INT_ID XPAR_XUARTPS_1_INTR
#define UART_MESSAGE_SIZE_MAX 50
#define UART_BAUDRATE 115200
#define UART_TX_BUFFER_SIZE 128 //Has to be power of 2
#define UART_RX_BUFFER_SIZE 128 //Has to be power of 2

extern XUartPs UART_Driver;

//Hardwarenaher Ringpuffer
struct uart_ringbuffer{
    uint8_t buf[UART_TX_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
};


struct uart_buffer{
    uint8_t buf[UART_RX_BUFFER_SIZE];
    uint16_t idx;
};
extern struct uart_ringbuffer uart_tx_rb; //Delaration
extern struct uart_buffer uart_rx_buf;

int Init_UART(void);
void UART_Interrupt_Handler(void *CallBackRef, u32 Event, unsigned int EventData);
void UART_Write_Receive_Buffer();
void UART_Process_Received_Message();
size_t rb_free_space();


/**
 * Prüft, ob der Ringpuffer leer ist.
 * Gibt true zurück, wenn keine Daten im Puffer sind.
 */
static inline bool rb_is_empty(void) {
	return uart_tx_rb.head == uart_tx_rb.tail;
}

/**
 * Prüft, ob der Ringpuffer voll ist.
 * Gibt true zurück, wenn kein weiteres Zeichen eingefügt werden kann.
 */
static inline bool rb_is_full(void) {
	return ((uart_tx_rb.head + 1) & (UART_TX_BUFFER_SIZE - 1)) == uart_tx_rb.tail;
}

/**
 * Fügt ein Zeichen in den Ringpuffer ein.
 * Gibt true zurück, wenn das Einfügen erfolgreich war, false wenn der Puffer voll ist.
 */
static inline bool rb_push(uint8_t c) {
	uint16_t next = (uart_tx_rb.head + 1) & (UART_TX_BUFFER_SIZE - 1);
	if (next == uart_tx_rb.tail) return false;
	uart_tx_rb.buf[uart_tx_rb.head] = c;
	uart_tx_rb.head = next;
	return true;
}

/**
 * Liest ein Zeichen aus dem Ringpuffer und entfernt es.
 * Gibt true zurück, wenn das Auslesen erfolgreich war, false wenn der Puffer leer ist.
 */
static inline bool rb_pop(uint8_t *c) {
	if (uart_tx_rb.head == uart_tx_rb.tail) return false;
	*c = uart_tx_rb.buf[uart_tx_rb.tail];
	uart_tx_rb.tail = (uart_tx_rb.tail + 1) & (UART_TX_BUFFER_SIZE - 1);
	return true;
}


#endif // UART_H

//TO BE REMOVED
// Ring buffer API
/*
extern bool rb_is_empty(void);
extern bool rb_is_full(void);
extern bool rb_push(uint8_t c);
extern bool rb_pop(uint8_t *c);
*/

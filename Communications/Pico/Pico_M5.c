/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "hardware/irq.h"
#include "hardware/uart.h"
#include "pico/stdlib.h"
#include "string.h"

/// \tag::uart_advanced[]

#define UART_ID uart1
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 4
#define UART_RX_PIN 5

// Communications variables
char fullString[255] = "{";
int previousIntegerData[5];
int currentIntegerData[5];
static char *datapointNames[7] = {"speed",         "turning",
                                  "distance",
                                  "humpHeight", "coordinates" ,  "barcode", "nav_dir"};
// Storing other submodules' data
char *barcodeReading[50]={"barcode reading"};
char *nav_dir[20] = {".mmmJE++>3L=+>7L^>>`"};
int endpointCoord = 0;
int varyData = 0;
int sent_nav_dir =0;

// Function prototypes
void getIntDatapoints(void);
void comms(void);
int getSpeed(void);
int getTurning(void);
int getDistance(void);
int getHumpHeight(void);
int getCoordinates(void);

/* RX interrupt handler for Pico to read from M5 to serial */
void on_uart_rx() {
  int i = 0;
  while (uart_is_readable(UART_ID)) {
    printf("%c", uart_getc(UART_ID));
    if (i == 0) {
      endpointCoord = (uart_getc(UART_ID) - '0') * 10; // convert numeric reading from ASCII to int
      i++;
    } else {
      endpointCoord += (uart_getc(UART_ID) - '0'); // convert numeric reading from ASCII to int
    }
  }
  printf("\n");
}

int main() {
  stdio_init_all();
  // Set up our UART with a basic baud rate.
  uart_init(UART_ID, BAUD_RATE);

  // Set the TX and RX pins by using the function select on the GPIO
  // Set datasheet for more information on function select
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  // Set UART flow control CTS/RTS, we don't want these, so turn them off
  uart_set_hw_flow(UART_ID, false, false);

  // Set our data format
  uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

  // Turn off FIFO's - we want to do this character by character
  uart_set_fifo_enabled(UART_ID, false);

  // Set up a RX interrupt
  // We need to set up the handler first
  // Select correct interrupt for the UART we are using
  int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

  // And set up and enable the interrupt handlers
  irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
  irq_set_enabled(UART_IRQ, true);

  // Now enable the UART to send interrupts - RX only
  uart_set_irq_enables(UART_ID, true, false);

  // OK, all set up.
  // Lets send a basic string out, and then run a loop and wait for RX
  // interrupts The handler will count them, but also reflect the incoming data
  // back with a slight change!
  uart_puts(UART_ID, "\nCommunications started\n");

  while (1) {
    // other sub modules
    comms();
  }
}

/* Comms submodule code */
void comms(void) {
  int datapoints = 0;  // number of datapoints in the message to be sent
  int i = 0;           // for-loop counter

  // Read barcode data
  if (barcodeReading != "") {
    strcat(fullString, "\"");
    strcat(fullString, datapointNames[5]);
    strcat(fullString, "\": \"");
    strcat(fullString, barcodeReading);
    strcat(fullString, "\"");
    datapoints++;
    memset(barcodeReading, '\0', strlen(barcodeReading));
  }

  getIntDatapoints(); // Get integer data points (distance, speed, turning, hump height, current coordinates)

  /* Package each integer data point into the message */
  for (i = 0; i < 5; i++) {
    if(currentIntegerData[i] != previousIntegerData[i]) // send only changed datapoints
    {
      if (datapoints != 0) {
        strcat(fullString, ", ");
      }
      strcat(fullString, "\"");
      strcat(fullString, datapointNames[i]);
      strcat(fullString, "\": \"");
      char tempData[8];
      sprintf(tempData, "%d", currentIntegerData[i]);
      strcat(fullString, tempData);
      strcat(fullString, "\"");
      previousIntegerData[i] = currentIntegerData[i]; // store current value as prev value for the next round
      datapoints++;  // increment number of datapoints within the message
    }
  }
  
  /* Send string containing available directions to travel from each node only once*/
  if(nav_dir != ""){
    if(sent_nav_dir == 0){
      if (datapoints != 0) {
      strcat(fullString, ", ");
      }
      strcat(fullString, "\"");
      strcat(fullString, datapointNames[6]);
      strcat(fullString, "\": \"");
      strcat(fullString, nav_dir);
      strcat(fullString, "\"");
      datapoints++;
      sent_nav_dir++;
    }
  }

  /* Send message through UART to M5, for MQTT publish*/
  if (datapoints > 0) {
    strcat(fullString, "}");  // finish packaging the message since datapoints
                              // have been gathered
    uart_puts(UART_ID, fullString);
    sleep_ms(1500);
    /* clear the message */
    memset(fullString, '\0', strlen(fullString));
    strcat(fullString, "{");
  }
}

//To extract the data 
void getIntDatapoints(void) {
  memset(barcodeReading, '\0', strlen(barcodeReading));
  strcat(barcodeReading, "reading #");
  currentIntegerData[0] = getSpeed(); //Speed
  currentIntegerData[1] = getTurning(); //Turning
  currentIntegerData[2] = getDistance(); //Distance
  currentIntegerData[3] = getHumpHeight(); //HumpHeight
  currentIntegerData[4] = getCoordinates(); //Coordinates
  char[8] tempData;
  sprintf(tempData, "%d", varyData);
  strcat(barcodeReading, tempData);
  varyData++;
}

int getSpeed(void){
  if(varyData%4 < 2){
    return(10+varyData%3)
  }
  return (10 - varyData%3);
}
int getTurning(void){
  return (varyData%3);
}
int getDistance(void){
  return (varyData*5 + 20);
}
int getHumpHeight(void){
  return (varyData%4);
}
int getCoordinates(void){
   return (10*varyData%4 + );
}
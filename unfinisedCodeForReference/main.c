/**
 * MSP LATENCY TEST CODE
 * */

#include <driverlib.h>
#include <ESP8266.h>
#include <UART_Driver.h>
#include "stdio.h"
#include "string.h"

volatile int adcValue;
// char ESP8266_Buffer[ESP8266_BUFFER_SIZE];

void sendData(int, int);
void sendMQTT();
void Initalise_Timer(void);
static unsigned long getTime(void);
void sendBarcode(char[50]);

// const int NUMBER_OF_DATAPOINTS = 6; // max number of data points expected in one loop around the submodules
unsigned int previousIntegerData[5];
unsigned int currentIntegerData[5];
static char *datapointNames[6]={"avgSpeedWhole","avgSpeedDecimal","turning","distanceWhole","distanceDecimal", "humpHeight"};
#define SPEED = 0;
#define TURNING = 1;
#define DISTANCE_WHOLE = 2;
#define DISTANCE_DECIMAL = 3;
#define HUMP_HEIGHT = 4;
#define BARCODE = 5;
// uint8_t navEndpoint[3];
const int maxDatapoints = 7; // including 1 for navigation

char fullString[255] = "{";

int intTimes;
int timerValue;
#define TICKPERIOD      1000

char buffer[10000] =
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTES"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTES"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTES"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTES"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTES"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTES"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTES"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTES"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTES"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTEST"
    "TESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTESTTE"
    "STTESTTESTTESTTESTTESTTESTTESTTESTTES";

void Initialise_Timer(void)
{
    /* Timer_A UpMode Configuration Parameter */
    const Timer_A_UpModeConfig upConfig =
    {
            TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
            TIMER_A_CLOCKSOURCE_DIVIDER_12,          // SMCLK/12 = 1MHz
            TICKPERIOD,                             // 1000 tick period = 1 millisec per tick
            TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
            TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
            TIMER_A_DO_CLEAR                        // Clear value
    };

    int a = CS_getSMCLK();

    /* Configuring Timer_A0 for Up Mode */
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

    /* Enabling interrupts */
    Interrupt_enableInterrupt(INT_TA0_0);
    Timer_A_clearTimer(TIMER_A0_BASE);
}

/* struct for MSP's serial */
eUSCI_UART_Config UART0Config =
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,
     6,
     8,
     0,
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

/* Struct for M5's serial */
eUSCI_UART_Config UART2Config =
{
     EUSCI_A_UART_CLOCKSOURCE_SMCLK,
     6,
     8,
     0,
     EUSCI_A_UART_NO_PARITY,
     EUSCI_A_UART_LSB_FIRST,
     EUSCI_A_UART_ONE_STOP_BIT,
     EUSCI_A_UART_MODE,
     EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION
};

void main()
{
    MAP_WDT_A_holdTimer();
    Initialise_Timer();

    /*Ensure MSP432 is Running at 12 MHz*/
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);
    PCM_setCoreVoltageLevel(PCM_VCORE1);
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    /*Initialize required hardware peripherals for the MSP's Serial Port*/
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_UART_initModule(EUSCI_A0_BASE, &UART0Config);
    MAP_UART_enableModule(EUSCI_A0_BASE);
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);

    /*Initialize required hardware peripherals for the M5's Serial Port*/
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_UART_initModule(EUSCI_A2_BASE, &UART2Config);
    MAP_UART_enableModule(EUSCI_A2_BASE);
    MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA2);

    MAP_Interrupt_enableMaster();

    UART_Printf(EUSCI_A0_BASE, "\n\n****************Communications Start***************\r\n");
    /*Start ESP8266 serial terminal, will not return*/
    int j = 0;
    int i = 0;
    intTimes = 0;

    while(1)
    {
        for(i=0; i<10; i++){
            char timeData[8];
            Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
            UART_Printf(EUSCI_A2_BASE, buffer);
            /* Stop Timer */
            Timer_A_stopTimer(TIMER_A0_BASE);   // do not clear, want to add up to get time taken for a few transmissions
            sprintf(timeData, "%d", getTime());
            UART_Printf(EUSCI_A0_BASE, "\nTime taken (ms) for 10000 UART transmissions is ");
            UART_Printf(EUSCI_A0_BASE, timeData);
            intTimes = 0; // reset timer
        }
        break;
    }
}

// /* API for submodules to call */
// void sendData(int module, int value){
//     int i = 0;
//     /* Only package for MQTT if data has changed from previous one */
//         /* Serialise the data */
//         char charData[15];      // temp store for the incoming datapoint
//         if(strlen(fullString) != 1){
//             strcat(fullString, ", ");
//         }
//         strcat(fullString, "\"");
//         strcat(fullString, moduleNames[module]);
//         strcat(fullString, "\": ");
//         sprintf(charData, "%d", value);
//         strcat(fullString, charData);

//         if(barcodeAvail == 1){
//             if(strlen(fullString) != 1){
//                 strcat(fullString, ", ");
//             }
//             strcat(fullString, "\"barcode\": \"");
//             strcat(fullString, barcodeData);
//             strcat(fullString, "\"");
//             memset(barcodeData, '\0', strlen(barcodeData));
//             barcodeAvail = 0;
//         }
//         /* Prepare for next data point */
//         oldData[module] = value;
// //    }

//     datapointCount++;

//     /* Send data to M5 if all submodules have had a chance to run */
//     if(datapointCount == NUMBER_OF_DATAPOINTS){
//         /* Complete the serialisation */
//         strcat(fullString, "}");
//         fullString_isFull = true;

//         /* Send the collated data points through UART */
//         sendMQTT();

//         /* Clear counter for data points collected */
//         datapointCount = 0;
//     }

// }

// void sendBarcode(char barcode[50]){
//     int i = 0;
//     strcat(barcodeData, barcode);
//     barcodeAvail = 1;
//     for(i=0;i<20000;i++);
// }

// void sendMQTT(){
//     int i = 0;
//     /* Sends fullString through UART to MSP & M5 */
//     /* Start Timer to track the time taken by the UART communication */
//     for(i=0; i<1000; i++){
//     Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);

//     UART_Printf(EUSCI_A2_BASE, fullString);
//     /* Stop Timer */
//     Timer_A_stopTimer(TIMER_A0_BASE);   // do not clear, want to add up to get time taken for a few transmissions

//     }
//     /* Reset fullString after transmitting */
//     memset(fullString, '\0', strlen(fullString));
//     strcat(fullString, "{");
//     fullString_isFull = false;


//     /* Delay to allow the serial buffers to clear,` else multiple fullStrings will be sent together to MQTT*/
// //    for(i=0;i<115000;i++);
// }

/* Returns time taken in milliseconds (int) */
static unsigned long getTime(void)
{
    unsigned long timetaken=0;
    timetaken = 0;
    /* Number of times the interrupt occurred (1 interrupt = 1000 ticks)    */
    timetaken = intTimes * TICKPERIOD;

    /* Number of ticks (between 1 to 999) before the interrupt could occur */
    timetaken += Timer_A_getCounterValue(TIMER_A0_BASE);

    /* Clear Timer */
    Timer_A_clearTimer(TIMER_A0_BASE);

    return timetaken;
}

// int getData(){
//     ADC14->CTL0 |= ADC14_CTL0_SC;   // software trigger to START CONVERSION
//     while(!(ADC14->IFGR0 & BIT0));
//     adcValue = ADC14->MEM[0];       // read ADC14MEM0 register value, clear ADC14IFGR0

// }

void TA0_0_IRQHandler(void)
{
    /* Increment global variable (count number of interrupt occurred) */
    intTimes++;

    /* Clear interrupt flag */
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
}

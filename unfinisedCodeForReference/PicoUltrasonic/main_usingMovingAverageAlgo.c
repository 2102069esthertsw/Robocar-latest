#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/types.h"


/**
* To see the output, minicom -b 115200 -o -D /dev/ttyS3
* 
*/

// #ifndef PICO_DEFAULT_LED_PIN
// #error blink example requires a board with a regular LED
// #endif
uint led_pin = 25;
uint C_TRIGGER_PIN = 17;
uint C_ECHO_PIN = 16;
uint L_TRIGGER_PIN = 18;
uint L_ECHO_PIN = 19;
uint R_TRIGGER_PIN = 20;
uint R_ECHO_PIN = 21;


// -----------------------------------------------------------
float   measure_distance(ECHO_PIN,TRIGGER_PIN){
    float   distance_cm = 0.0;

    gpio_set_input_enabled( ECHO_PIN, true );

    gpio_put( TRIGGER_PIN, false ); // make sure that the pin is low
    sleep_ms(2);                // wait a bit to let things stabalize
    uint echo_pin = gpio_get( ECHO_PIN);
	// printf("\nPulsed the pin high echo pin is %d", echo_pin );
    
    gpio_put( TRIGGER_PIN, true);  // go high
    sleep_ms( 10 );             // sleep 3 mili seconds
    uint tp1 = gpio_get( TRIGGER_PIN);
    // printf("\nPulsed the pin trigg 1 pin is %d", tp1 );

    gpio_put( TRIGGER_PIN, false);  // Switch the trigger low again to let the SR04 send the pulse
    uint tp = gpio_get( TRIGGER_PIN);
    // printf("\nPulsed the pin trigg pin is %d", tp );

	/*
		The echo pin woutputs a pulse between 150 micro-seconds and 25 mili-seconds, or if no object is found, it will send a 38 ms pulse. 
		speed of sound is 343 meters per second. This would depend upon the elevation and hummidity, but 
		sufficiently accurate for our application. 

		speed = distance travelled / time taken
		
		so, distance = (speed * time taken)/2 -- Divide by 2 because we are listening to the echo
		
		Now, speed of sound is 343 meters/second, whch is 3.43 centimeters per second, which is
		3.43/ 1000000 = 0.0343 cm/microsecond. 
		
		
	// */
    absolute_time_t  listen_start_time = get_absolute_time();
    // printf("\nTest");
    absolute_time_t  max_wait_time  = delayed_by_ms( listen_start_time, 30 );  // No point waiting more than 30 Mili sec. 
	// printf("\nTest");
    // printf("\nTest time %llu ",listen_start_time);
    // printf("\nTest time2 %llu ",max_wait_time);
    do{
        absolute_time_t  t_now = get_absolute_time();
        int64_t diff = absolute_time_diff_us( max_wait_time,t_now);
        // printf("\nT diff %d", diff);
        if( diff>0 ){
            // printf("T diff %d", diff);
            break;
        }
        echo_pin = gpio_get(ECHO_PIN);
        // uint echo_pin = gpio_get( ECHO_PIN);
	    // printf("\nPulsed the pin high echo pin is %d", echo_pin );
        if( echo_pin != 0 ){   // We got an echo!
            absolute_time_t  first_echo_time = t_now;
            // printf("\n the pin went high ");
            while( echo_pin == 1 && diff<0 ){
                echo_pin = gpio_get( ECHO_PIN );
                t_now = get_absolute_time();
            }
        // printf("\n pin is at %d", echo_pin );


      if( echo_pin == 1 ){
        break;  // will return 0 cm.
      }
    //   printf("\nStart %llu ", listen_start_time );
    //   printf( "\nend %llu ", t_now );
            int64_t pulse_high_time = absolute_time_diff_us( first_echo_time, t_now );
      float pulse_high_time_float = (float) pulse_high_time;
            // printf("\n High Time %f ", pulse_high_time_float ); 
            distance_cm = pulse_high_time_float * 0.01715; //pulse_high_time_float / 58.0 ;
            break;
        }
    }
    while( true );
    gpio_set_input_enabled( ECHO_PIN, false );
    return distance_cm;
}



// ---------------------------------------------------------------
int main() {
    
    stdio_init_all();
    // setup the trigger pin in putput mode and echo pin to input mode
    gpio_init( C_TRIGGER_PIN );
    gpio_init( C_ECHO_PIN );
    gpio_set_dir( C_TRIGGER_PIN, GPIO_OUT );
    gpio_set_dir( C_ECHO_PIN, GPIO_IN );

    gpio_init( L_TRIGGER_PIN );
    gpio_init( L_ECHO_PIN );
    gpio_set_dir( L_TRIGGER_PIN, GPIO_OUT );
    gpio_set_dir( L_ECHO_PIN, GPIO_IN );


    gpio_init( R_TRIGGER_PIN );
    gpio_init( R_ECHO_PIN );
    gpio_set_dir( R_TRIGGER_PIN, GPIO_OUT );
    gpio_set_dir( R_ECHO_PIN, GPIO_IN );

    gpio_init(led_pin);
    gpio_set_dir( led_pin, GPIO_OUT);
    
    float sample[10];// collect sample
    int counterForSamples=0;
    float sum=0;
    float avg =0;
    while (true) {
        // flash the on-board LED to show that we are still alive. 
        //reads distance every 0.1 sec
        gpio_put( led_pin, true);
        //  printf("Blinking On!\r\n");
        sleep_ms(100);
        //  printf("Blinking Off!\r\n");
        gpio_put( led_pin, false);
        sleep_ms(100);
        // printf("Measuring!!\r\n");
        
        float  C_distance = measure_distance(C_ECHO_PIN,C_TRIGGER_PIN);
        float  L_distance = measure_distance(L_ECHO_PIN,L_TRIGGER_PIN);
        
        float  R_distance = measure_distance(R_ECHO_PIN,R_TRIGGER_PIN);    
        
        sample[counterForSamples]=C_distance;
        for (int i=0;i<=counterForSamples;i++){
            sum+=sample[i];
        }
        
        counterForSamples+=1;
        avg=sum/counterForSamples;
        if (counterForSamples ==10){
            counterForSamples=0;
        }
        sum=0;
        printf(" \n%5.2f", avg );
        // printf(" \n%5.2f", C_distance );
        // if (C_distance>400){
        //     printf("\n C Out of range.");
        // }else if(C_distance<2){
        //     printf("\n C Out of range. Too close!");
        // }else{
        // printf(" \nC Closest object is %5.2f cm away\n", C_distance );
        // }
        // if (L_distance>400){
        //     printf("\n L Out of range.");
        // }else if(L_distance<2){
        //     printf("\n L Out of range. Too close!");
        // }else{
        // printf(" \nL Closest object is %5.2f cm away\n", L_distance );
        // }
        // if (R_distance>400){
        //     printf("\n R Out of range.");
        // }else if(R_distance<2){
        //     printf("\n R Out of range. Too close!");
        // }else{
        // printf(" \n R Closest object is %5.2f cm away\n", R_distance );
        // }

    
}
}
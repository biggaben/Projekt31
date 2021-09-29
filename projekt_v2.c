
#include <stdio.h>
#include "brick.h"
#include <unistd.h>

#define Sleep( msec ) usleep(( msec ) * 1000 ) /* Definerar sleep, Sleep(1000)= 1 sekund */

#define MOTOR_RIGHT     OUTA
#define MOTOR_LEFT      OUTB
#define MOTOR_C         OUTC
#define MOTOR_D         OUTD
#define SENSOR_TOUCH    IN1
#define SENSOR_GYRO     IN2
#define SENSOR_DISTANCE IN3
#define SENSOR_4		IN4

#define MOTOR_BOTH     ( MOTOR_LEFT | MOTOR_RIGHT ) /* Bitvis ELLER ger att båda motorerna styrs samtidigt */   
#define MOTOR_ALL	( MOTOR_BOTH | MOTOR_C )


POOL_T touchSensor; 
int TouchReturnValue = 0;

POOL_T gyroSensor;
int gyroValue = 0;

POOL_T sonicSensor;
int readDistance = 0;

int stop = 0;
int max_speed;
int max_speed_c;         /* variabel för max hastighet på motorn */

int motor;
int speed;
int degrees_to_turn = 0;



int main( void ){
	
    if ( !brick_init()) return ( 1 ); /* Initialiserar EV3-klossen */
        printf( "*** ( EV3 ) Hello! ***\n" );
        Sleep( 1000 );
		
    if ( tacho_is_plugged( MOTOR_BOTH, TACHO_TYPE__NONE_ )) {  /* TACHO_TYPE__NONE_ = Alla typer av motorer */
        max_speed = tacho_get_max_speed( MOTOR_LEFT, 0 ); // Kollar motorns maxhastigheten
		max_speed_c = tacho_get_max_speed( MOTOR_C, 0 );
        tacho_reset( MOTOR_BOTH );
		tacho_reset( MOTOR_C );
    }
	
    return ( 0 );
}


	
	
void reg_sensors(){
	touchSensor = sensor_search( LEGO_EV3_TOUCH ); // Registrerar sensorer
	gyroSensor = sensor_search(LEGO_EV3_GYRO);
	sonicSensor = sensor_search( LEGO_EV3_US );
}
	
	
void set_sensor_mode(){
	touch_set_mode_touch(touchSensor); // anger vilken "mode" touchsensorn skall ha
	gyro_set_mode_gyro_ang(gyroSensor);  // anger vilken "mode" gyrosensorn skall ha
	us_set_mode_us_dist_cm(sonicSensor);  // anger vilken "mode" sonicsensorn skall ha
}


// Sätter maxhastighet, och rotation på motorer. -x är fråmåt.
void set_motor_speed(motor, speed){
	tacho_set_speed_sp( motor, max_speed * speed );
}

//Lastar av boken när distansmätaren uppnår värde på <= 30 cm
void unload_book(){
	readDistance = (sensor_get_value(0, sonicSensor, 0)/10);
	//printf("Distance: %d cm\n", readDistance );
	if(readDistance <= 30){
		tacho_stop( MOTOR_BOTH );
		tacho_run_forever( MOTOR_C );
		Sleep( 5000 );
		tacho_stop( MOTOR_ALL );
	}
	
}	

void infinite_task( task, time_until_break ){
	for(;;){
		task;
		Sleep( time_until_break )
		break;
	}
}

void activate_motor(motor){
	tacho_stop( MOTOR_ALL );
	set_motor_speed(motor, -1);
	tacho_run_forever( motor );
}

void activate_motor_reverse(motor){
	tacho_stop( MOTOR_ALL );
	set_motor_speed(motor, 1);
	tacho_run_forever( motor );
}
	

void turn_right_in_degrees(degrees_to_turn){
	gyroValue = 0;
	gyroValue = sensor_get_value(0, gyroSensor, 0);
	while( gyroValue <= degrees_to_turn ){
		tacho_run_forever( MOTOR_RIGHT );
	}
	tacho_stop( MOTOR_ALL );
}

void turn_left_in_degrees(degrees_to_turn){
	gyroValue = 0;
	gyroValue = sensor_get_value(0, gyroSensor, 0);
	while( gyroValue >= degrees_to_turn ){
		tacho_run_forever( MOTOR_LEFT );
	}
	tacho_stop( MOTOR_ALL );
}
		
	
		
//BRA ATT HA
/*gyroValue = sensor_get_value(0, gyroSensor, 0);
while( gyroValue < 90 ){
	gyroValue = sensor_get_value(0, gyroSensor, 0);
	printf("Gyro: %d\n", gyroValue );
}*/
	
//TouchReturnValue = sensor_get_value(0, touchSensor, 0);*/
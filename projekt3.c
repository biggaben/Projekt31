
#include <stdio.h>
#include "brick.h"
#include <unistd.h>

#define Sleep( msec ) usleep(( msec ) * 1000 ) /* Definerar sleep, Sleep(1000)= 1 sekund */

#define MOTOR_LEFT    	 OUTA
#define MOTOR_RIGHT      OUTB
#define MOTOR_C        	 OUTC
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
int smallest_distance_degree;
int gyroValueInit;

POOL_T sonicSensor;
int readDistance = 0;
int smallest_distance_value = 999;

int max_hastighet;
int max_hastighet_c;         /* variabel för max hastighet på motorn */

int main( void ){
	
    if ( !brick_init()) return ( 1 ); /* Initialiserar EV3-klossen */
        printf( "*** ( EV3 ) Hello! ***\n" );
        Sleep( 2000 );
    if ( tacho_is_plugged( MOTOR_BOTH, TACHO_TYPE__NONE_ )) {  /* TACHO_TYPE__NONE_ = Alla typer av motorer */
        max_hastighet = tacho_get_max_speed( MOTOR_LEFT, 0 );/* Kollar maxhastigheten som motorn kan ha */
	max_hastighet_c = tacho_get_max_speed( MOTOR_C, 0 );
        tacho_reset( MOTOR_BOTH );
	tacho_reset( MOTOR_C );
    } else {
        printf( "Anslut vänster motor i port A,\n"
        "Anslut höger motor i port B.\n");
        brick_uninit();
        return ( 0 );  /* Stänger av sig om motorer ej är inkopplade */
    }
	
	if(!sensor_is_plugged((SENSOR_TOUCH|SENSOR_GYRO), SENSOR_TYPE__NONE_)) {
		printf("Stoppa in sensorer i port 1 och 2\n");
		brick_uninit();
		return(0);
	}
	
	if(!sensor_is_plugged((SENSOR_DISTANCE), SENSOR_TYPE__NONE_)) {
		printf("Stoppa in sensorer i port 3\n");
		brick_uninit();
		return(0);
	}
	
    touchSensor = sensor_search( LEGO_EV3_TOUCH ); // Registrerar sensorer
	gyroSensor = sensor_search(LEGO_EV3_GYRO);
	sonicSensor = sensor_search( LEGO_EV3_US );
	
    touch_set_mode_touch(touchSensor); // anger vilken "mode" touchsensorn skall ha
	sensor_set_mode(gyroSensor, LEGO_EV3_GYRO_GYRO_G_AND_A);  // anger vilken "mode" gyrosensorn skall ha
    sensor_set_mode(gyroSensor, LEGO_EV3_GYRO_GYRO_ANG);
	us_set_mode_us_dist_cm(sonicSensor);  // anger vilken "mode" sonicsensorn skall ha
	
    tacho_set_speed_sp( MOTOR_BOTH, max_hastighet * 1 );  // Sätter hastighetenpå båda motorerna till 50% av maxhastigheten
    /* Om man vill köra bakåt anger man negativ hastighet, till exempel max_hastighet * (-0.5) */
    tacho_set_speed_sp( MOTOR_C, max_hastighet * -1 );
	
	
    //gyroValue0 = sensor_get_value(0, gyroSensor, 0);
    /*printf("Gyro0: %d , Gyro1: %d \n", gyroValue0, gyroValue1 );*/
    readDistance = (sensor_get_value(0, sonicSensor, 0));
    tacho_set_speed_sp( MOTOR_LEFT, max_hastighet * 0.7 );
    tacho_set_speed_sp( MOTOR_RIGHT, max_hastighet * -0.7 );
    sensor_set_mode(gyroSensor, LEGO_EV3_GYRO_GYRO_G_AND_A);  // anger vilken "mode" gyrosensorn skall ha
    sensor_set_mode(gyroSensor, LEGO_EV3_GYRO_GYRO_ANG);
    while(gyroValue < 360){
        tacho_run_forever(MOTOR_BOTH);
        printf("Gyro: %d \n", gyroValue);
        gyroValue = sensor_get_value(0, gyroSensor, 0);
        readDistance = (sensor_get_value(0, sonicSensor, 0));
        if (smallest_distance_value > readDistance){
            smallest_distance_value = readDistance;
            smallest_distance_degree = gyroValue;
        }
    }

    tacho_stop(MOTOR_BOTH);
    Sleep(1000);
    tacho_set_speed_sp( MOTOR_LEFT, max_hastighet * -0.1 );
    tacho_set_speed_sp( MOTOR_RIGHT, max_hastighet * 0.1 );
    tacho_run_forever(MOTOR_BOTH);
    while(gyroValue != smallest_distance_degree){
        gyroValue = sensor_get_value(0, gyroSensor, 0);
    }

    tacho_stop(MOTOR_BOTH);

    /*while(readDistance > 30){
        readDistance = (sensor_get_value(0, sonicSensor, 0)/10);
        tacho_run_forever(MOTOR_BOTH);
    }*/
    Sleep(500);
    //tacho_run_forever( MOTOR_C );
    //Sleep( 5000 );
    //tacho_stop( MOTOR_ALL );
    sensor_set_mode(gyroSensor, LEGO_EV3_GYRO_GYRO_G_AND_A);  // anger vilken "mode" gyrosensorn skall ha
    sensor_set_mode(gyroSensor, LEGO_EV3_GYRO_GYRO_ANG);
    gyroValueInit = sensor_get_value(0, gyroSensor, 0);
    gyroValue = sensor_get_value(0, gyroSensor, 0 );
    Sleep(1000);
    tacho_set_speed_sp( MOTOR_LEFT, max_hastighet * -0.1 );
    tacho_set_speed_sp( MOTOR_RIGHT, max_hastighet * 0.1 );
    tacho_run_forever( MOTOR_RIGHT );
    while(gyroValue > -87){
        printf("Gyro: %d \n", gyroValue);
        gyroValue = sensor_get_value(0, gyroSensor, 0);
    }
    
    tacho_stop( MOTOR_ALL );
    tacho_set_speed_sp( MOTOR_BOTH, max_hastighet * 0.5 );
    tacho_run_forever( MOTOR_BOTH );

    Sleep(10000);
    tacho_stop(MOTOR_ALL);

    readDistance = (sensor_get_value(0, sonicSensor, 0));
    tacho_set_speed_sp( MOTOR_LEFT, max_hastighet * 0.7 );
    tacho_set_speed_sp( MOTOR_RIGHT, max_hastighet * -0.7 );
    sensor_set_mode(gyroSensor, LEGO_EV3_GYRO_GYRO_G_AND_A);  // anger vilken "mode" gyrosensorn skall ha
    sensor_set_mode(gyroSensor, LEGO_EV3_GYRO_GYRO_ANG);
    while(gyroValue < 360){
        tacho_run_forever(MOTOR_BOTH);
        printf("Gyro: %d \n", gyroValue);
        gyroValue = sensor_get_value(0, gyroSensor, 0);
        readDistance = (sensor_get_value(0, sonicSensor, 0));
        if (smallest_distance_value > readDistance){
            smallest_distance_value = readDistance;
            smallest_distance_degree = gyroValue;
        }
    }

    tacho_stop(MOTOR_BOTH);
    Sleep(1000);
    tacho_set_speed_sp( MOTOR_LEFT, max_hastighet * -0.1 );
    tacho_set_speed_sp( MOTOR_RIGHT, max_hastighet * 0.1 );
    tacho_run_forever(MOTOR_BOTH);
    while(gyroValue != smallest_distance_degree){
        gyroValue = sensor_get_value(0, gyroSensor, 0);
    }

    tacho_stop(MOTOR_ALL);
    Sleep(1000);
    tacho_set_speed_sp( MOTOR_BOTH, max_hastighet * 0.5 );
    tacho_run_forever(MOTOR_BOTH);
    while(readDistance > 300){
        readDistance = (sensor_get_value(0, sonicSensor, 0));
    }
    tacho_stop( MOTOR_ALL );

    tacho_run_forever( MOTOR_C );
	Sleep( 8000 );
	tacho_stop( MOTOR_ALL );
    Sleep(100);
    tacho_set_speed_sp( MOTOR_LEFT, max_hastighet * 1 );
    tacho_set_speed_sp( MOTOR_RIGHT, max_hastighet * -1 );
    tacho_run_forever(MOTOR_BOTH);
    Sleep(5000);

    return 0;
}




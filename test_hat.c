#include "Adafruit_Motor_HAT.h"
#include <stdlib.h>

int main(void){
	struct motor mot3 ;
	struct motor mot4 ;
	motor_init(&mot3, 3);
	motor_init(&mot4, 4);

	motor_run(RELEASE, &mot3);
	motor_run(RELEASE, &mot4);

	motor_set_speed(80, &mot4);
	motor_run(BACKWARD, &mot4);
	motor_set_speed(80, &mot3);
	motor_run(FORWARD, &mot3);
	sleep(2);
	motor_set_speed(0, &mot3);
	sleep(1);
	motor_set_speed(80, &mot3);
	sleep(2);
	motor_close(&mot3);
	motor_close(&mot4);
}

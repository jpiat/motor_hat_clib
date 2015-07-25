#include "Adafruit_Motor_HAT.h"
#include <stdlib.h>

int main(void){
	struct motor mot3 ;
	struct motor mot4 ;
	init_motor(&mot3, 3);
	init_motor(&mot4, 4);

	mot_run(RELEASE, &mot3);
	mot_run(RELEASE, &mot4);

	setSpeed(80, &mot4);
	mot_run(BACKWARD, &mot4);
	setSpeed(80, &mot3);
	mot_run(FORWARD, &mot3);
	sleep(2);
	setSpeed(0, &mot3);
	sleep(1);
	setSpeed(80, &mot3);
	sleep(2);
	close_motor(&mot3);
	close_motor(&mot4);
}

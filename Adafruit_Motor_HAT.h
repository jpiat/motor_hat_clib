
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>


#ifndef ADAFRUIT_MOTOR_HAT_H
#define ADAFRUIT_MOTOR_HAT_H

#define HAT_ADDR 0X60

enum mot_command{
	FORWARD,
	BACKWARD,
	RELEASE,
	BRAKE
};

struct mot_pins{
	char pwm;
	char in1 ;
	char in2 ;
};


struct motor{
	struct mot_pins * pins ;
	int nb ;
};

int init_motor(struct motor * mot, int motor_nb);
void mot_run(enum mot_command command, struct motor * mot);
void setSpeed(int speed, struct motor * mot);
void close_motor(struct motor * mot);


#endif
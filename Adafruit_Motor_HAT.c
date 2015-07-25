
#include "Adafruit_Motor_HAT.h"

#define  REG_MODE1     0x00
#define  REG_MODE2     0x01
#define  REG_SUBADR1   0x02
#define  REG_SUBADR2   0x03
#define  REG_SUBADR3   0x04
#define  REG_PRESCALE  0xFE
#define  REG_LED0_ON_L 0x06
#define  REG_LED0_ON_H  0x07
#define  REG_LED0_OFF_L 0x08
#define  REG_LED0_OFF_H  0x09
#define  REG_ALL_LED_ON_L  0xFA
#define  REG_ALL_LED_ON_H  0xFB
#define  REG_ALL_LED_OFF_L 0xFC
#define  REG_ALL_LED_OFF_H 0xFD


#define  BIT_RESTART      0x80
#define  BIT_SLEEP        0x10
#define  BIT_ALLCALL      0x01
#define  BIT_INVRT        0x10
#define  BIT_OUTDRV       0x04


#define MOT_0_PWM 8
#define MOT_0_IN2 9
#define MOT_0_IN1 10

#define MOT_1_PWM 13
#define MOT_1_IN2 12
#define MOT_1_IN1 11

#define MOT_2_PWM 2
#define MOT_2_IN2 3
#define MOT_2_IN1 4

#define MOT_3_PWM 7
#define MOT_3_IN2 6
#define MOT_3_IN1 5

#define USER_PWM_0 0
#define USER_PWM_1 1
#define USER_PWM_2 14
#define USER_PWM_3 15

struct mot_pins mot_pins_array[] = {{MOT_0_PWM, MOT_0_IN1, MOT_0_IN2}, {MOT_1_PWM, MOT_1_IN1, MOT_1_IN2}, {MOT_2_PWM, MOT_2_IN1, MOT_2_IN2}, {MOT_3_PWM, MOT_3_IN1, MOT_3_IN2}};


struct motor_hat{
	int i2c_fd ;
	char mot_init[4];
};


struct motor_hat hat ;

static inline void i2c_write8(unsigned char reg, unsigned char val)
{
	unsigned char i2c_buffer[2];

	if (ioctl(hat.i2c_fd, I2C_SLAVE, HAT_ADDR) < 0) {
		return ; 
	}

	i2c_buffer[0] = reg;
	i2c_buffer[1] = val;
	write(hat.i2c_fd, i2c_buffer, 2);
}

static inline void i2c_read8(unsigned char reg, unsigned char * val)
{
	unsigned char i2c_buffer[2];

	if (ioctl(hat.i2c_fd, I2C_SLAVE, HAT_ADDR) < 0) {
		return ; 
	}

	i2c_buffer[0] = reg;
	write(hat.i2c_fd, i2c_buffer, 1);
	read(hat.i2c_fd, val, 1);
}


int init_hat(){
	char mode1 ;
	int i ;
	for(i = 0  ; i < 4 ; i ++) hat.mot_init[i] = 0 ;
	hat.i2c_fd = open("/dev/i2c-1", O_RDWR);
	if (hat.i2c_fd < 0) {
	// ERROR HANDLING; you can check errno to see what went wrong 
		printf("could not open I2C device : %s!\n", "/dev/i2c-1");
		close(hat.i2c_fd);
		return -1 ;
	}
	if (ioctl(hat.i2c_fd, I2C_SLAVE, HAT_ADDR) < 0){
		printf("I2C communication error ! \n");     
		close(hat.i2c_fd);
		return -1 ;
	}
	
	i2c_write8(REG_MODE2, BIT_OUTDRV);
	i2c_write8(REG_MODE1, BIT_ALLCALL);

	i2c_read8(REG_MODE1, &mode1) ;
    	mode1 = mode1 & BIT_SLEEP  ;
        i2c_write8(REG_MODE1, mode1)  ;
}


void setPWMFreq(unsigned int freq){
	float prescaleval ;
	unsigned int int_prescale ;
	char oldmode, newmode ;
	prescaleval = 25000000.0   ; // 25MHz
	prescaleval /= 4096.0     ; // 12-bit
	prescaleval /= (float) freq ;
	prescaleval -= 1.0 ;
	int_prescale = prescaleval + 0.5;

	i2c_read8(REG_MODE1, &oldmode);
	newmode = (oldmode & 0x7F) | 0x10;         // sleep
	i2c_write8(REG_MODE1, newmode);        // go to sleep
	i2c_write8(REG_PRESCALE, int_prescale);
	i2c_write8(REG_MODE1, oldmode);
	usleep(5);
	i2c_write8(REG_MODE1, oldmode | 0x80);
}



void set_pwm_on_off(int channel, int on, int off){
    i2c_write8(REG_LED0_ON_L+4*channel, on & 0xFF);
    i2c_write8(REG_LED0_ON_H+4*channel, on >> 8);
    i2c_write8(REG_LED0_OFF_L+4*channel, off & 0xFF);
    i2c_write8(REG_LED0_OFF_H+4*channel, off >> 8);
}


void setAllPWM(int on, int off){
    i2c_write8(REG_ALL_LED_ON_L, on & 0xFF);
    i2c_write8(REG_ALL_LED_ON_H, on >> 8);
    i2c_write8(REG_ALL_LED_OFF_L, off & 0xFF);
    i2c_write8(REG_ALL_LED_OFF_H, off >> 8);
}


void  setPin(int pin, int value){
                if (value == 0){
                        set_pwm_on_off(pin, 0, 4096) ;
		}else{
                        set_pwm_on_off(pin, 4096, 0) ;
		}
}


int motor_init(struct motor * mot, int motor_nb){
	if(hat.i2c_fd == 0){
		init_hat();
		setPWMFreq(1600);
	}
	hat.mot_init[motor_nb-1] = 1 ;
	mot->nb = motor_nb ;
	mot->pins = &(mot_pins_array[motor_nb-1]);
	motor_run(RELEASE, mot);
}

void motor_run(enum mot_command command, struct motor * mot){
                if (command == FORWARD){
                        setPin(mot->pins->in2, 0);
                        setPin(mot->pins->in1, 1);
                }else if (command == BACKWARD){
                        setPin(mot->pins->in1, 0);
                        setPin(mot->pins->in2, 1);
                }else if (command == RELEASE){
                        setPin(mot->pins->in1, 0);
                        setPin(mot->pins->in2, 0);
		}else if (command == BRAKE){
                        setPin(mot->pins->in1, 1);
                        setPin(mot->pins->in2, 1);
                }
}

void motor_set_speed(int speed, struct motor * mot){
        if (speed < 0){
                speed = 0;
        }else if (speed > 255){
                speed = 255;
	}
        set_pwm_on_off(mot->pins->pwm, 0, speed*16);
}

void motor_close(struct motor * mot){
	int i ;	
	motor_run(RELEASE, mot);
	hat.mot_init[mot->nb-1] = 0 ;
	for(i = 0 ; i < 4 ; i ++) if(hat.mot_init[i] != 0) return ; // if not all motor were killed
	close(hat.i2c_fd);
}



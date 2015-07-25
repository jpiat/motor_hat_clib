
CFLAGS=
LDFLAGS=

OBJS += \
Adafruit_Motor_HAT.o \
test_hat.o

all : test_hat

clean :
	rm *.o test_hat

test_hat : $(OBJS)
	gcc -o $@ $(OBJS)

%.o : %.c
	gcc $(CFLAGS) -c $< 

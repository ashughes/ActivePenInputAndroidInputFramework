CFLAGS=
LDFLAGS=
LINUX_KERNEL=<path to your Android Linux kernel>
ASM_INCLUDE=$(LINUX_KERNEL)/arch/arm/include
LINUX_INCLUDE=$(LINUX_KERNEL)/include

INCLUDES=-I$(ASM_INCLUDE) \
	 -I$(LINUX_INCLUDE)

# Make variables (CC, etc...)

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump
AWK		= awk

ALL=wacandroid

all: $(ALL)

wacandroid: wacandroid.c wactablet.c wacserial.c wacusb.c
	$(CC) $(INCLUDES) wacandroid.c wactablet.c wacserial.c wacusb.c -o wacandroid

clean:
	rm -rf *.o *.h.gch core* $(ALL)


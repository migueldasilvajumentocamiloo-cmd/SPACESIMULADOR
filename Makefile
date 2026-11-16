TARGET = spacesim
OBJS = source/main.o

TITLE = SpaceSim PS3
APPID = SPACESIM0
CONTENTID = UP0001-SPACESIM0_00-0000000000000000

include $(PSL1GHT)/Makefile.base

CFLAGS   += -std=gnu99 -O2 -Wall
CXXFLAGS += -std=gnu++11 -O2 -Wall
LDFLAGS  += -lm

LIBS := -ltiny3d -lrsx -lgcm_sys -lsysutil -lsysfs -lio -lm

all: pkg

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET).elf $(TARGET).self $(TARGET).pkg $(TARGET).bin

.PHONY: all clean

.SUFFIXES:

ifeq ($(strip $(PSL1GHT)),)
$(error "Please set PSL1GHT in your environment. export PSL1GHT=<path/to/PSL1GHT>")
endif

TARGET		:=	spacesim
BUILD		:=	build
SOURCES		:=	source
INCLUDES	:=	include source

TITLE		:=	SpaceSim PS3
APPID		:=	SPACESIM0
CONTENTID	:=	UP0001-SPACESIM0_00-0000000000000000

LIBS		:=	-ltiny3d -lrsx -lgcm_sys -lsysutil -lsysfs -lio -lm

include $(PSL1GHT)/ppu_rules

CFLAGS		+=	-g -O2 -Wall $(MACHDEP) $(INCLUDE)
CXXFLAGS	+=	$(CFLAGS)
LDFLAGS		+=	$(MACHDEP) -Wl,-Map,$(notdir $@).map

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))

export LD	:=	$(CC)
export OFILES	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o)
export INCLUDE	+=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			-I$(PSL1GHT)/ppu/include \
			-I$(PS3DEV)/ppu/include \
			-I$(PS3DEV)/portlibs/ppu/include \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	+=	-L$(PSL1GHT)/ppu/lib \
			-L$(PS3DEV)/ppu/lib \
			-L$(PS3DEV)/portlibs/ppu/lib

.PHONY: $(BUILD) clean all pkg

all: pkg

pkg: $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile $(OUTPUT).elf
	@mkdir -p $(BUILD)/pkg/USRDIR
	@make_self $(OUTPUT).elf $(BUILD)/pkg/USRDIR/EBOOT.BIN
	@if [ -f ICON0.PNG ]; then cp ICON0.PNG $(BUILD)/pkg/ICON0.PNG; fi
	@python3 $(PS3DEV)/bin/sfo.py --title "$(TITLE)" --appid "$(APPID)" -f $(PS3DEV)/bin/sfo.xml $(BUILD)/pkg/PARAM.SFO
	@python3 $(PS3DEV)/bin/pkg.py --contentid $(CONTENTID) $(BUILD)/pkg/ $(OUTPUT).pkg

$(BUILD):
	@[ -d $@ ] || mkdir -p $@

clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).self $(TARGET).pkg

else

DEPENDS	:=	$(OFILES:.o=.d)

$(OUTPUT).elf: $(OFILES)

-include $(DEPENDS)

endif

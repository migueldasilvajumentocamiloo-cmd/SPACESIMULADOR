.SUFFIXES:

ifeq ($(strip $(PSL1GHT)),)
$(error "Please set PSL1GHT in your environment. export PSL1GHT=<path/to/PSL1GHT>")
endif

TARGET		:=	spacesim
BUILD		:=	build
SOURCES		:=	source
INCLUDES	:=	include

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
			-I$(PS3DEV)/portlibs/ppu/include \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	+=	-L$(PSL1GHT)/ppu/lib -L$(PS3DEV)/portlibs/ppu/lib

.PHONY: $(BUILD) clean all pkg

all: pkg

pkg: $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile $(OUTPUT).elf
	@mkdir -p $(BUILD)/pkg/USRDIR
	@python3 -c "import zlib,struct; w,h=320,176; raw=b''.join(b'\x00'+b'\x0a\x18\x45'*w for _ in range(h)); chk=lambda t,d:struct.pack('>I',len(d))+t+d+struct.pack('>I',zlib.crc32(t+d)&0xffffffff); open('$(BUILD)/pkg/ICON0.PNG','wb').write(b'\x89PNG\r\n\x1a\n'+chk(b'IHDR',struct.pack('>IIBBBBB',w,h,8,2,0,0,0))+chk(b'IDAT',zlib.compress(raw))+chk(b'IEND',b''))"
	@make_self_npdrm $(OUTPUT).elf $(BUILD)/pkg/USRDIR/EBOOT.BIN $(CONTENTID)
	@python3 $$(which sfo.py) --title "$(TITLE)" --appid "$(APPID)" -f /tmp/psl1ght/tools/generic/sfo.xml $(BUILD)/pkg/PARAM.SFO
	@python3 $$(which pkg.py) --contentid $(CONTENTID) $(BUILD)/pkg/ $(OUTPUT).pkg

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

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

	# === Ícone simples 320x176 ===
	@python3 -c "import zlib, struct
w,h=320,176
# Fundo azul espacial com borda
raw = b''
for y in range(h):
    for x in range(w):
        if y==0 or y==h-1 or x==0 or x==w-1:
            raw += b'\x00\xFF\xFF\xFF'  # branco borda
        else:
            raw += b'\x00\x00\x10\x40'  # azul escuro
chk=lambda t,d:struct.pack('>I',len(d))+t+d+struct.pack('>I',zlib.crc32(t+d)&0xffffffff)
open('$(BUILD)/pkg/ICON0.PNG','wb').write(b'\x89PNG\r\n\x1a\n'+chk(b'IHDR',struct.pack('>IIBBBBB',w,h,8,2,0,0,0))+chk(b'IDAT',zlib.compress(raw,9))+chk(b'IEND',b''))"

	# === Criar SELF com make_self (sem npdrm primeiro para testar) ===
	@make_self $(OUTPUT).elf $(BUILD)/pkg/USRDIR/EBOOT.BIN

	# === Criar PARAM.SFO usando caminho correto ===
	@python3 $(PS3DEV)/bin/sfo.py --title "$(TITLE)" --appid "$(APPID)" -f $(PSL1GHT)/tools/generic/sfo.xml $(BUILD)/pkg/PARAM.SFO

	# === Criar PKG ===
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

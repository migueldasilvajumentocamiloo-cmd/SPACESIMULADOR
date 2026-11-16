# SpaceSim PS3 — Makefile (PSL1GHT)

# Nome do projeto
TARGET = spacesim

# Arquivos fonte
OBJS = src/main.o

# Identificadores obrigatórios do PS3
TITLE = SpaceSim PS3
APPID = SPACESIM0
CONTENTID = UP0001-SPACESIM0_00-0000000000000000

# Configuração do PSL1GHT
include $(PSL1GHT)/Makefile.base

# Bandeiras de compilação
CFLAGS   += -std=gnu99 -O2 -Wall
CXXFLAGS += -std=gnu++11 -O2 -Wall
LDFLAGS  += -lm

# Bibliotecas necessárias
LIBS := -lrsx -lgcm_sys -lsysutil -lsysfs -lpad -lm

# Regras de compilação
all: pkg

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(TARGET).elf $(TARGET).self $(TARGET).pkg $(TARGET).bin

.PHONY: all clean

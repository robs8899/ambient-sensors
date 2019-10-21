# TARGET = remote-dht
TARGET = remote-htu

MCU  = atmega328p
FCPU = 8000000UL

BUILD_BASE = build
FLASH_DIR  = flash

SOURCE_DIRS     = src/core src/dev src/util
BUILD_DIRS      = $(addprefix $(BUILD_BASE)/,$(SOURCE_DIRS))
SOURCE_FILES    = $(foreach sdir,$(SOURCE_DIRS),$(wildcard $(sdir)/*.c)) src/$(TARGET).c
INCLUDE_FILES   = $(foreach sdir,$(SOURCE_DIRS),$(wildcard $(sdir)/*.h))
OBJECT_FILES    = $(patsubst %.c,$(BUILD_BASE)/%.o,$(SOURCE_FILES))

CC = avr-gcc
LD = avr-gcc

CFLAGS = -std=gnu99 -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections \
	-funsigned-char -funsigned-bitfields -mmcu=$(MCU) -DF_CPU=$(FCPU)

LDFLAGS = -Wl,-Map,$(BUILD_BASE)/$(TARGET).map,--gc-sections -mmcu=$(MCU)

.PHONY: all check obj elf hex clean

# --- Phony Targets ---

all: hex

check: $(BUILD_DIRS)

obj: check $(OBJECT_FILES)

elf: obj
	$(LD) $(LDFLAGS) $(OBJECT_FILES) -o $(BUILD_BASE)/$(TARGET).elf
	avr-objdump -h -S $(BUILD_BASE)/$(TARGET).elf > $(BUILD_BASE)/$(TARGET).lss
	avr-size --format=avr --mcu=$(MCU) $(BUILD_BASE)/$(TARGET).elf

hex: elf
	avr-objcopy -O ihex -j .data -j .text $(BUILD_BASE)/$(TARGET).elf $(BUILD_BASE)/$(TARGET)_flash.hex
	avr-objcopy -O ihex -j .eeprom --change-section-lma .eeprom=0 $(BUILD_BASE)/$(TARGET).elf $(BUILD_BASE)/$(TARGET)_eeprom.hex

clean:
	"rm" -rf $(BUILD_BASE)

# --- File Targets ---

$(BUILD_BASE)/%.o : %.c $(INCLUDE_FILES)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIRS):
	"mkdir" -p $@

include $(BOARD_DIR)/firmware/firmware.mk

BOARDINC += $(BOARD_DIR)/generated/controllers/generated

# defines SHORT_BOARD_NAME
include $(BOARD_DIR)/meta-info.env

# reduce memory usage monitoring
DDEFS += -DRAM_UNUSED_SIZE=100

# no LED on this board — assign to non-existent pin
DDEFS += -DLED_CRITICAL_ERROR_BRAIN_PIN=Gpio::I15

# save Lua RAM — disable unused Lua subsystems
DDEFS += -DWITH_LUA_CONSUMPTION=FALSE
DDEFS += -DWITH_LUA_PID=FALSE
DDEFS += -DWITH_LUA_STOP_ENGINE=FALSE

# software knock detection on ADC3
DDEFS += -DSTM32_ADC_USE_ADC3=TRUE
DDEFS += -DEFI_SOFTWARE_KNOCK=TRUE

# knock frequency spectrogram
DDEFS += -DKNOCK_SPECTROGRAM=TRUE

# MAX31855 thermocouple for EGT
DDEFS += -DEFI_MAX_31855=TRUE

# ========== OpenBLT bootloader ==========
export USE_OPENBLT = yes

# CAN bootloader pins (PD0 = CAN1_RX, PD1 = CAN1_TX)
DDEFS += -DBOOT_COM_CAN_CHANNEL_INDEX=0
DDEFS += -DOPENBLT_CAN_RX_PORT=GPIOD
DDEFS += -DOPENBLT_CAN_RX_PIN=0
DDEFS += -DOPENBLT_CAN_TX_PORT=GPIOD
DDEFS += -DOPENBLT_CAN_TX_PIN=1
DDEFS += -DBOOT_BACKDOOR_ENTRY_TIMEOUT_MS=0

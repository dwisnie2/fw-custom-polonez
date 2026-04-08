#include "pch.h"
#include "board_overrides.h"

Gpio getCommsLedPin() {
	return Gpio::Unassigned;
}

Gpio getRunningLedPin() {
	return Gpio::Unassigned;
}

Gpio getWarningLedPin() {
	return Gpio::Unassigned;
}

// Polonez board defaults — Speeduino-to-rusEFI adapter with STM32/GD32
static void customBoardDefaultConfiguration() {
	// ========== Trigger ==========
	engineConfiguration->triggerInputPins[0] = Gpio::D4;  // crank (36-2)
	engineConfiguration->triggerInputPins[1] = Gpio::Unassigned;
	engineConfiguration->camInputs[0] = Gpio::D3;         // cam

	// ========== Injection — 6 cylinders sequential ==========
	engineConfiguration->injectionPins[0] = Gpio::D8;
	engineConfiguration->injectionPins[1] = Gpio::A8;
	engineConfiguration->injectionPins[2] = Gpio::D10;
	engineConfiguration->injectionPins[3] = Gpio::E7;
	engineConfiguration->injectionPins[4] = Gpio::D9;
	engineConfiguration->injectionPins[5] = Gpio::D11;

	// ========== Ignition — 6 individual coils ==========
	engineConfiguration->ignitionPins[0] = Gpio::E2;
	engineConfiguration->ignitionPins[1] = Gpio::E3;
	engineConfiguration->ignitionPins[2] = Gpio::C13;
	engineConfiguration->ignitionPins[3] = Gpio::D5;
	engineConfiguration->ignitionPins[4] = Gpio::D6;
	engineConfiguration->ignitionPins[5] = Gpio::B9;

	// ========== Fuel pump (active low) ==========
	engineConfiguration->fuelPumpPin = Gpio::E11;
	engineConfiguration->fuelPumpPinMode = OM_INVERTED;

	// ========== Outputs ==========
	engineConfiguration->tachOutputPin = Gpio::E8;
	engineConfiguration->acRelayPin = Gpio::E12;
	engineConfiguration->boostControlPin = Gpio::B15;

	// ========== AC switch input ==========
	engineConfiguration->acSwitch = Gpio::E1;

	// ========== Stepper idle ==========
	engineConfiguration->idle.stepperDirectionPin = Gpio::B10;
	engineConfiguration->idle.stepperStepPin = Gpio::B11;
	engineConfiguration->stepperEnablePin = Gpio::A15;
	engineConfiguration->useStepperIdle = true;

	// ========== CAN bus ==========
	engineConfiguration->canTxPin = Gpio::D1;
	engineConfiguration->canRxPin = Gpio::D0;

	// ========== SPI ==========
	engineConfiguration->is_enabled_spi_1 = true;   // EGT / sensors
	engineConfiguration->is_enabled_spi_3 = true;   // SD card

	engineConfiguration->spi1mosiPin = Gpio::B5;
	engineConfiguration->spi1misoPin = Gpio::B4;
	engineConfiguration->spi1sckPin = Gpio::B3;

	engineConfiguration->spi3mosiPin = Gpio::C12;
	engineConfiguration->spi3misoPin = Gpio::C11;
	engineConfiguration->spi3sckPin = Gpio::C10;

	// ========== SD card on SPI3 ==========
	engineConfiguration->sdCardCsPin = Gpio::D2;
	engineConfiguration->sdCardSpiDevice = SPI_DEVICE_3;
	engineConfiguration->isSdCardEnabled = true;

	// ========== ADC channels ==========
	engineConfiguration->tps1_1AdcChannel = EFI_ADC_2;      // PA2
	engineConfiguration->clt.adcChannel = EFI_ADC_1;        // PA1
	engineConfiguration->iat.adcChannel = EFI_ADC_0;        // PA0
	engineConfiguration->map.sensor.hwChannel = EFI_ADC_3;  // PA3
	engineConfiguration->afr.hwChannel = EFI_ADC_8;         // PB0
	engineConfiguration->vbattAdcChannel = EFI_ADC_4;       // PA4

	// ========== Analog dividers ==========
	// 5.6k high side / 10k low side
	engineConfiguration->analogInputDividerCoefficient = 1.47f;
	// 6.34k high side / 1k low side
	engineConfiguration->vbattDividerCoeff = (6.34f + 1) / 1;
	engineConfiguration->adcVcc = 3.3f;

	// ========== Sensor calibration ==========
	engineConfiguration->clt.config.bias_resistor = 2490;
	engineConfiguration->iat.config.bias_resistor = 2490;
}

void setup_custom_board_overrides() {
	custom_board_DefaultConfiguration = customBoardDefaultConfiguration;

	void customBoardTsAction(uint16_t subSystem, uint16_t index);
	custom_board_ts_command = customBoardTsAction;
}

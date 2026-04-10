#include "pch.h"
#include "board_overrides.h"
#include "frequency_sensor.h"

// ========== VSS spike rejection filter ==========
// vehicleSpeedSensor has external linkage in upstream — we hook into it
extern FrequencySensor vehicleSpeedSensor;

// Custom converter: time-based spike rejection, counting from last ACCEPTED edge.
// Enable: engineConfiguration->devBit0 (checkbox in TunerStudio)
// Max ratio: engineConfiguration->scriptSetting[7]
class SpikeRejectingVssConverter : public SensorConverter {
public:
	SensorResult convert(float rawFrequency) const override {
		efitimeus_t now = getTimeNowUs();

		if (engineConfiguration->devBit0 && m_lastAcceptedTimeUs > 0) {
			float maxRatio = engineConfiguration->scriptSetting[7];
			if (maxRatio < 1.1f) {
				maxRatio = 3.0f;  // safety fallback
			}

			efitimeus_t elapsedUs = now - m_lastAcceptedTimeUs;

			// Min acceptable period = last accepted period / maxRatio
			float lastPeriodUs = 1000000.0f / m_lastAcceptedFreq;
			if ((float)elapsedUs < lastPeriodUs / maxRatio) {
				// Edge came too soon vs last accepted — spike, reject
				return m_lastKph;
			}

			// Accepted: compute corrected freq from accepted-to-accepted interval
			float correctedFreq = 1000000.0f / (float)elapsedUs;
			m_lastAcceptedTimeUs = now;
			m_lastAcceptedFreq = correctedFreq;
			return convertFreqToKph(correctedFreq);
		}

		// Filter disabled or first reading — passthrough
		m_lastAcceptedTimeUs = now;
		m_lastAcceptedFreq = rawFrequency;
		return convertFreqToKph(rawFrequency);
	}

private:
	SensorResult convertFreqToKph(float freq) const {
		auto pulsePerKm = engineConfiguration->driveWheelRevPerKm
		                 * engineConfiguration->vssGearRatio
		                 * engineConfiguration->vssToothCount;
		if (pulsePerKm == 0) {
			return 0;
		}
		m_lastKph = freq * 3600.0f / pulsePerKm;
		return m_lastKph;
	}

	mutable efitimeus_t m_lastAcceptedTimeUs = 0;
	mutable float m_lastAcceptedFreq = 0;
	mutable float m_lastKph = 0;
};

static SpikeRejectingVssConverter spikeRejectingVssConverter;
static bool vssFilterInstalled = false;

static void polonezPeriodicFast() {
	// One-shot: swap VSS converter to our spike-rejecting version after init
	if (!vssFilterInstalled) {
		vehicleSpeedSensor.setFunction(spikeRejectingVssConverter);
		vssFilterInstalled = true;
	}
}

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
	// ========== Trigger — Rover K16 (offset = 0) ==========
	engineConfiguration->triggerInputPins[0] = Gpio::D4;  // crank
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

	// ========== VSS spike filter defaults ==========
	engineConfiguration->devBit0 = true;           // enable by default
	engineConfiguration->scriptSetting[7] = 3.0f;  // max frequency jump ratio
	strncpy(engineConfiguration->scriptSettingName[7], "VSSspikeRatio", 16);
}

void setup_custom_board_overrides() {
	custom_board_DefaultConfiguration = customBoardDefaultConfiguration;
	custom_board_periodicFastCallback = polonezPeriodicFast;

	void customBoardTsAction(uint16_t subSystem, uint16_t index);
	custom_board_ts_command = customBoardTsAction;
}

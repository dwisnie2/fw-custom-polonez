#include "pch.h"

void boardTuneDefaults() {
	engineConfiguration->displacement = 3.0;
	engineConfiguration->cylindersCount = 6;
	engineConfiguration->cylinderBore = 86.0f;  // 2JZ-GTE bore → knock freq calc

	// ========== Trigger: use TunerStudio selection (no hardcoded trigger type) ==========

	// ========== Knock detection ==========
	engineConfiguration->enableSoftwareKnock = true;
	engineConfiguration->enableKnockSpectrogram = true;
	engineConfiguration->enableKnockSpectrogramFilter = true;
	engineConfiguration->knockSpectrumSensitivity = 1.0f;
}

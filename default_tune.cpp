#include "pch.h"

void boardTuneDefaults() {
	engineConfiguration->displacement = 3.0;
	engineConfiguration->cylindersCount = 6;
	engineConfiguration->cylinderBore = 86.0f;  // 2JZ-GTE bore → knock freq calc

	// ========== Trigger: Rover K16 v2 (MEMS3 Common Pattern 2 + gap[3] noise filter) ==========
	engineConfiguration->trigger.type = trigger_type_e::TT_ROVER_K16_V2;

	// ========== Knock detection ==========
	engineConfiguration->enableSoftwareKnock = true;
	engineConfiguration->enableKnockSpectrogram = true;
	engineConfiguration->enableKnockSpectrogramFilter = true;
	engineConfiguration->knockSpectrumSensitivity = 1.0f;
}

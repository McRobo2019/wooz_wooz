#ifndef SOUND_PLAYER_H_
#define SOUND_PLAYER_H_

#include "ev3api.h"

using namespace ev3api;

#define sound_set_volume(vol)  { SoundPlayer::setVolume(vol); }
#define sound_play_chime1(...) { SoundPlayer::playChime1(__VA_ARGS__); }
#define sound_play_chime2(...) { SoundPlayer::playChime2(__VA_ARGS__); }

class SoundPlayer {

	public:
		SoundPlayer() { ; }
		static void setVolume(uint8_t vol) { ev3_speaker_set_volume(vol); }
		static void playChime1(uint16_t fq, int32_t dur) { ev3_speaker_play_tone(fq, dur); }
		static void playChime2(uint16_t fq, int32_t dur) { 
				ev3_speaker_play_tone(fq, dur);
				tslp_tsk(100);
				ev3_speaker_play_tone(fq, dur);
	   	}
		static void playWav(const memfile_t *file, int32_t dur) { ev3_speaker_play_file(file, dur); }
		static void stop() { ev3_speaker_stop(); }
};

#endif // SOUND_PLAYER_H

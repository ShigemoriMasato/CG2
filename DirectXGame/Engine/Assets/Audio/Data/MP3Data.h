#pragma once
#include "AudioData.h"

class MP3Data : public AudioData {
public:

	MP3Data() = default;
	~MP3Data() = default;

	void Load(std::filesystem::path filepath) override;
	int Play(IXAudio2* xAudio, bool isLoop) override;

private:

};


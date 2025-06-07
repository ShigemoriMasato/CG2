#pragma once
#include <wrl.h>
#include <cstdint>
#include <vector>

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

struct ChunkHeader {
	char id[4];
	int32_t size;
};

struct RiffHeader {
	ChunkHeader chunk;
	char type[4];
};

struct FormatChunk {
	ChunkHeader chunk;
	WAVEFORMATEX fmt;
};

struct SoundData {
	WAVEFORMATEX wfex;
	BYTE* pBuffer;
	uint32_t buffferSize;
};

class Audio {
public:

	void Initialize();

	int SoundLoadWave(const char* filename);

	void SoundUnload(int soundHandle);

	void SoundPlayWave(int soundHandle);

private:

	//Audio
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice = nullptr;

	std::vector<SoundData> sdList_;
};

#pragma once
#include <string>
#include <xaudio2.h>
#include <filesystem>
#include <fstream>
#include <array>
#include <wrl.h>

enum class AudioType {
	wav,
	mp3
};

// チャンクヘッダ
struct ChunkHeader {
	char id[4];   // チャンク毎のID
	int32_t size; // チャンクサイズ
};

// RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk; // RIFF
	char type[4];  // WAVE
};

// FMTチャンク
struct FormatChunk {
	ChunkHeader chunk; // fmt
	WAVEFORMATEX fmt;  // 波形フォーマット
};

class AudioData {
public:

	AudioData() = default;
	~AudioData();

	virtual void Load(std::filesystem::path filePath) = 0;

	void SetVolume(float volume);

	/// <summary>
	/// 読み込んだデータを再生する
	/// </summary>
	/// <returns>再生ハンドル。AudioManagerから返す際は、音の種類*10 + 再生ハンドルにすること。</returns>
	virtual int Play(IXAudio2* xAudio, bool isLoop) = 0;

	bool IsPlay(int handle);
	void Stop(int handle);

protected:

	// 波形フォーマット
	WAVEFORMATEX wfex_;
	// バッファの先頭アドレス
	BYTE* pBuffer_;
	// バッファのサイズ
	unsigned int bufferSize_;
	// 音声データの名前
	std::string name_;

	AudioType type_;

	static const int resourceNum_ = 8;

	std::array<IXAudio2SourceVoice*, resourceNum_> playResource_ = {
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
	};
	IXAudio2SourceVoice* loopPlayResource_ = 0;
	int playResourceIndex_ = 0;

	float volume_ = 1.0f;

};

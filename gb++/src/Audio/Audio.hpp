#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <SFML/Audio.hpp>
#include <cmath>
#include <vector>

class AudioStream : public sf::SoundStream {
	float time;
	float period;
	float frequency;

public:
	void init() {
		frequency = 700.0f;
		period = 1.0f / frequency;
		time = 0.0f;
		SampleBuffer.resize(44100);
		initialize(1, 44100);
	}
private:
	virtual bool onGetData(Chunk& data) {
		for (size_t i = 0; i < 44100; i++) {
			time -= time > period ? period : 0;
			float sine_sample = sin(2.0f * M_PI * time / period);
			short sine2int = ((sine_sample + 1) / 2) * 65535 - 32768;
			time += 1.0f / 44100.0f;
			SampleBuffer[i] = 0.5f * sine2int;
		}

		data.samples = &SampleBuffer[0];
		data.sampleCount = 44100;

		return true;
	}

	virtual void onSeek(sf::Time timeOffset) {

	}

	std::vector<sf::Int16> SampleBuffer;
};

class Memory;

class Audio {
	AudioStream* AS;
	Memory* MemoryBus;
public:
	Audio(Memory* _MemoryBus);
	~Audio();
	void Clock();
};

#endif
#include "Audio.h"
#include <OpenAL\alut.h>
#include <assert.h>

Audio::Audio(const std::string &filePath) : buffer(alutCreateBufferFromFile(filePath.c_str())),
                                            source(0)
{
    assert(buffer != 0);

    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer); // 音データのバッファを指定

    Loop(false);
}

void Audio::Loop(const bool isLoop)
{
    alSourcei(source, AL_LOOPING, isLoop ? AL_TRUE : AL_FALSE); // ループの設定
}

void Audio::Play()
{
    alSourcePlay(source);
}

void Audio::Stop()
{
    alSourceStop(source);
}

void Audio::Pause()
{
    alSourcePause(source);
}

const bool Audio::IsPlaying() const
{
    return GetOffset() != 0;
}

const float Audio::GetDuration() const
{
    ALint size, frequency, channels, bits;

    alGetBufferi(buffer, AL_SIZE, &size);
    alGetBufferi(buffer, AL_FREQUENCY, &frequency);
    alGetBufferi(buffer, AL_CHANNELS, &channels);
    alGetBufferi(buffer, AL_BITS, &bits);

    return static_cast<float>(size) / static_cast<float>(frequency * channels * (bits / 8));
}

const float Audio::GetOffset() const
{
    float offset;

    alGetSourcef(source, AL_SEC_OFFSET, &offset);

    return offset;
}
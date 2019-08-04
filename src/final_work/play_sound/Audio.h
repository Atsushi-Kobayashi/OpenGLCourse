#pragma once

#include <string>
#include <OpenAL/al.h>

class Audio
{
public:
    explicit Audio(const std::string &filePath);

    // ループ設定
    void Loop(const bool isLoop);

    // 再生
    void Play();

    // 一時停止
    void Pause();

    // 停止
    void Stop();

    // 再生中かどうか
    const bool IsPlaying() const;

    // 音声データの尺を秒単位で取得する
    const float GetDuration() const;

    // 現在の再生位置を秒単位で取得する
    const float GetOffset() const;

private:
    ALuint buffer;
    ALuint source;
};
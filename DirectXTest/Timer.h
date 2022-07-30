#pragma once
#include <chrono>

using namespace std::chrono;

//  経過時間を計測するクラス
class Timer
{
public:
    Timer();
    ~Timer();

    //  リセット
    void Reset();

    //  時間計測の開始
    void Start();
    //  時間計測の停止
    void Stop();

    //  計測開始しているか
    bool IsStart()const{ return m_isStart; }

    //  経過時間を得る
    float GetElapseTime();
private:
    //  計測開始時間
    system_clock::time_point m_startTime;
    //  現在時間
    system_clock::time_point m_nowTime;

    //  計測開始しているか
    //  NOTE: 計測開始してなければ、経過時間を0と返すためのbool
    bool m_isStart;

    //  現在のゲームスピードを管理するクラス
    class GameSpeedOperator& m_speed;
};
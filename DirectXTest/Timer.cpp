#include "Timer.h"

#include "Singleton.h"
#include "GameSpeedOperator.h"

Timer::Timer()
    :m_speed(Singleton<GameSpeedOperator>::GetInstance())
    , m_isStart(false)
{
    //  処理なし
}

Timer::~Timer()
{
    //  処理なし
}

//  リセット
void Timer::Reset()
{
    //  スタートしていない状態から
    m_isStart = false;
}

//  計測開始
void Timer::Start()
{
    m_startTime = system_clock::now();
    m_isStart = true;
}
//  計測停止
void Timer::Stop()
{
    m_isStart = false;
}

//  経過時間を得る
//  計測開始していなければ0を返す
float Timer::GetElapseTime()
{
    if (m_isStart)
    {
        //  現在の時間を得る
        m_nowTime = system_clock::now();
        //  経過時間を得る
        auto erapsedTime = m_nowTime - m_startTime;

        //  経過時間をfloat型に変換して返す
        return static_cast<float>(duration_cast<microseconds>(erapsedTime).count() * 0.0001f) * m_speed.GetGameSpeed();
    }
    else
    {
        return 0;
    }
}

#pragma once
#include <chrono>

using namespace std::chrono;

//  ゲームスピードを管理するクラス
//  シングルトンで扱う
class GameSpeedOperator
{
public:
    GameSpeedOperator();
    ~GameSpeedOperator();

    //  現在のゲームスピードを得る
    float GetGameSpeed()const { return m_gameSpeed; }
    //  一時停止中か
    bool IsPause()const { return m_isPause; }
    
    //  一時停止
    //  NOTE: 現在の仕様ではゲーム内のオブジェクトの動きを止めるだけで、
    //        ゲームそのものの経過時間を止めることはできないため、
    //        一時停止しても、時間が経過すれば新しいエネミーが登場します。
    void Pause();
    //  一時停止の解除
    void ReStart();
private:
    //  現在のゲームスピードを保存する変数
    float m_gameSpeed;
    //  一時停止中か
    bool m_isPause;
};
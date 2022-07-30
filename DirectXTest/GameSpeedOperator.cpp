#include "GameSpeedOperator.h"

//  定数
static const float DEFAULT_SPEED = 1.0f;       //  デフォルトのゲームスピード

GameSpeedOperator::GameSpeedOperator()
    :m_gameSpeed(DEFAULT_SPEED)
    , m_isPause(false)
{
}

GameSpeedOperator::~GameSpeedOperator()
{
}

//  一時停止
//  NOTE: 現在の仕様ではゲーム内のオブジェクトの動きを止めるだけで、
//        ゲームそのものの経過時間を止めることはできないため、
//        一時停止しても、時間が経過すれば新しいエネミーが登場します。
void GameSpeedOperator::Pause()
{
    m_gameSpeed = 0.0f;
    m_isPause = true;
}

//  一時停止の解除
void GameSpeedOperator::ReStart()
{
    m_gameSpeed = DEFAULT_SPEED;
    m_isPause = false;
}


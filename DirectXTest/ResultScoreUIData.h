#pragma once
#include <DirectXMath.h>

using namespace DirectX;

//  リザルトスコアUIのデータ
namespace ResultScoreUIData
{
    //  スコアの増加量
    const float SCORE_INCREASE_RATE = 0.015;

    //  コロン文字列
    const char* COLON_STRING = ":";

    //  生存時間スコアの文字列
    const char* ALIVE_STRING = "Alive Time";
    //  生存時間スコア文字列の表示座標
    const XMFLOAT2 ALIVE_TIME_SCORE_STRING_POS = { 600,226 };
    //  生存時間スコアのコロン文字列の表示座標
    const XMFLOAT2 ALIVE_CORON_STRING_POS = { 920,226 };
    //  生存時間スコアの表示座標
    const XMFLOAT2 ALIVE_TIME_SCORE_NUM_POS = { 1020,226 };

    //  回避スコアの文字列
    const char* DODGE_STRING = "Dodge";
    //  回避スコア文字列の表示座標
    const XMFLOAT2 DODGE_SCORE_STRING_POS = { 600,326 };
    //  回避スコアのコロン文字列の表示座標
    const XMFLOAT2 DODGE_CORON_STRING_POS = { 920,326 };
    //  回避スコアの表示座標
    const XMFLOAT2 DODGE_SCORE_NUM_POS = { 1020,326 };

    //  残機スコアの文字列
    const char* LIFE_STRING = "Life";
    //  残機スコア文字列の表示座標
    const XMFLOAT2 LIFE_SCORE_STRING_POS = { 600,426 };
    //  残機スコアのコロン文字列の表示座標
    const XMFLOAT2 LIFE_CORON_STRING_POS = { 920,426 };
    //  残機スコアの表示座標
    const XMFLOAT2 LIFE_SCORE_NUM_POS = { 1020,426 };

    //  エネミーの撃破数スコアの文字列
    const char* DEFEAT_STRING = "Kill Score";
    //  エネミーの撃破数スコア文字列の表示座標
    const XMFLOAT2 DEFEAT_ENEMY_SCORE_STRING_POS = { 600,526 };
    //  エネミーの撃破数スコアのコロン文字列の表示座標
    const XMFLOAT2 DEFEAT_CORON_STRING_POS = { 920,526 };
    //  エネミーの撃破数スコアの表示座標
    const XMFLOAT2 DEFEAT_ENEMY_NUM_SCORE_POS = { 1020,526 };

    //  総スコア以外のスコアの文字列の表示倍率
    const float SCORE_STRING_RATE = 1.0f;
    //  総スコア以外のスコアの値の表示倍率
    const float SCORE_NUM_RATE = 1.0f;

    //  総スコアの文字列
    const char* TOTAL_SCORE_STRING = "Total";
    //  総スコア文字列の表示座標
    const XMFLOAT2 TOTAL_SCORE_STRING_POS = { 600,676 };
    //  総スコアのコロン文字列の表示座標
    const XMFLOAT2 TOTAL_SCORE_CORON_STRING_POS = { 920,676 };
    //  総スコアの表示座標
    const XMFLOAT2 TOTAL_SCORE_NUM_POS = { 1020,676 };

    //  総スコアの文字列の表示倍率
    const float TOTAL_SCORE_STRING_RATE = 1.5f;
    //  総スコアの値の表示倍率
    const float TOTAL_SCORE_NUM_RATE = 1.5f;
}
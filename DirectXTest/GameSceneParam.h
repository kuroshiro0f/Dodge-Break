#pragma once
#include <DirectXMath.h>

using namespace DirectX;

namespace GameSceneParam
{
    //  ゲームオーバー時の文字の位置
    const XMFLOAT2 GAMEOVER_STRING_POS = { 822.0f, 482.0f };
    //  ゲームクリア後、フェードアウトが始まるまでの時間
    const float TRANGITION_TIME_CLEAR_FOR_FADE_OUT = 100.0f;
    //  プレイヤーの登場する時間
    const float PLAYER_APPEARANCE_TIME = 33.0f;
    //  カメラの移動開始時間
    const float START_CAMERA_MOVE_TIME = 100.0f;

    //  スタート時のカメラの移動速度
    const float START_MOVE_CAMERA_SPEED = 0.5f;
    //  ゲーム開始時のカメラ位置
    const XMFLOAT3 START_SCENE_CAMERA_POS_ADD = { 0,-75.0f,42.0f };

    //  失敗時のリザルトにおけるゲーム時間の経過スピード
    const float FAILED_RESULT_GAME_TIME_SPEED = 0;
    const float FAILED_RESULT_TRANSITION_TIME = 10.0f;

#if _DEBUG
    //  チェックポイント指定用定数
    const int NEXT_CHECK_POINT = 1;  //  次
    const int PREV_CHECK_POINT = -1; //  前
#endif
}
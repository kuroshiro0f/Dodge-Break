#pragma once

//  プレイヤー被弾後の再開に必要なデータ
namespace RestartData
{
    const float RESTART_TIME = 100.0f;      //  プレイヤーが撃破されてから再登場するまでの時間
    const float RETURN_TIME = 500.0f;       //  ウェーブやエネミーの出現時間を遅らせる時間
}
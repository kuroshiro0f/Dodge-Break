#pragma once

//  イベントの種類
enum class EventType
{
    Dodge,          //  回避
    HitEnergyEffect,    //  エネルギーのエフェクトが塔に着弾
    UseEnemyDefeater,   //  塔の使用
    EnergyMax,          //  エネルギーが最大量に達する
    DeleteEnemy,        //  敵の撃破
    DamagePlayer,       //  プレイヤーの被弾
    Restart,            //  再開
    FinishWave          //  ウェーブの終了

    , None
};
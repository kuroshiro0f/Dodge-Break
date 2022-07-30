#pragma once

//  エフェクトの種類
enum class EffectType {
    //  プレイヤー
    AppearancePlayer,    //  登場
    DamagePlayer,            //  ダメージ
    ExitPlayer,              //  退場

    //  エネルギー
    DodgeEnergy,        //  回避時のエネルギー
    DodgeEnergyForEnergyMax,    //  エネルギーが最大の時の回避時のエネルギー

    //  エネミー
    DefeatAimEnemy,     //  各色ごとの撃破
    DefeatNoAimEnemy,
    DefeatUniqueShootEnemy,

    //  障害物
    HitAimEnemyTower,           //  各色ごとの撃破
    HitNoAimEnemyTower,
    HitUniqueShootEnemyTower,
    ChargeAimEnergy,            //  エネルギーが溜まった
    ChargeNoAimEnergy,
    ChargeUniqueShootEnergy,
    ActivateAimEnemyTower,      //  塔の起動
    ActivateNoAimEnemyTower,
    ActivateUniqueShootEnemyTower

    , None
};
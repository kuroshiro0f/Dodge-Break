#pragma once

//  エフェクトの種類
enum class EffectType
{
    None = 0,   //  初期化用

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

    //  エネミーの攻撃
    BeamAttack,        //  レーザー攻撃

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
};
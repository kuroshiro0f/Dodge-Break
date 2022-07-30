#pragma once

//  エネミーの種類
enum class EnemyType
{
    AimingShootEnemy,   //  プレイヤーを狙う攻撃を撃つ
    NoAimingShootEnemy,     //  固定の動きの攻撃を撃つ
    UniqueShootEnemy,       //  特殊な攻撃をする

    NullType        //  初期化用

    , None
};
#pragma once

//  当たり判定のあるオブジェクトの種類
enum class CollisionType {
    Player,     //  プレイヤー
    Enemy,          //  エネミー
    EnemyDefeater,  //  エネミーを倒すオブジェクト
    EnemyDefeaterActivateEffect, //  エネルギーが溜まった際に塔に飛んでいくエフェクト
    EnemyAttack,    //  エネミーの攻撃
    DodgeRange      //  回避範囲そのものの実体

    , None
};
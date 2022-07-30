#pragma once

//  音の種類
enum class SoundType 
{
    //  タイトル
    TitleBGM,           //  BGM
    TransitionGameSceneSE,  //  遷移

    //  ゲームシーン
    GameSceneBGM,       //  BGM

    //  リザルト
    ResultBGM,          //  BGM
    TransitionTitleSE,  //  遷移
    ScoreSE,        //  スコアの表示
    ValueSE,        //  評価の表示
    PraiseSE,       //  褒める

    //  プレイヤー
    AppearPlayerSE,     //  登場
    DodgeSE,            //  回避
    DamagePlayerSE,     //  被弾

    //  エネミー
    DefeatEnemySE,      //  撃破

    //  塔
    AbsorpEnergySE,     //  エネルギーの吸収
    ActivateEnemyDefeaterSE,    //  起動
    UseEnemyDefeaterSE,         //  使用

    //  攻撃
    AttackSE,        //  発射

    //  enumのラスト
    EndSound

    , None
};
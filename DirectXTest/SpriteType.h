#pragma once

//  画像の種類
//  NOTE: enum classにするとエラーが発生
//  TODO: 上記のエラーの原因調査
enum SpriteType
{
    //  タイトル
    TitleBack,    //  背景
    TitleText,        //  タイトル
    Start,            //  スタートの文字

    //  ゲームシーン
    Life,         //  残機
    Gauge,        //  ゲージ本体
    GaugeFrame,   //  ゲージの枠
    Time,         //  Time文字列

    //  リザルト
    FirstEvaluate,      //  1段階目の評価
    SecondEvaluate,     //  2段階目の評価
    ThirdEvaluate,      //  3段階目の評価
    ForthEvaluate,      //  4段階目の評価
    ResultBack,         //  背景
    ResultText,         //  リザルト
    Return,         //  スタートの文字
    Tips1,      //  Tips1つ目
    Tips2,      //  Tips2つ目
    Tips3,      //  Tips3つ目

    //  その他
    FadeOutBack,        //  フェードアウト用の黒い画像

    //  enumのラスト
    EndDescriptors

    , None
};
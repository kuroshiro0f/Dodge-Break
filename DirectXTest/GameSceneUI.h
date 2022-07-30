#pragma once
#include <memory>

//  ゲームシーンのUIクラス
class GameSceneUI
{
public:
    GameSceneUI();
    ~GameSceneUI();

    //  初期化
    void Init(class IPlayerParameterGetter* _playerParameterGetter, class IScoreGetter* _scoreGetter, const std::shared_ptr<class EventNotificator> _eventClass);
    //  リセット
    void Reset();

    //  更新
    void Update();

    //  画像描画
    void DrawSprite();

    //  文字描画
    void DrawString();

    //  フェードアウト
    void FadeOut();
    //  フェードアウトが終わったか
    bool IsFinishFadeOut();
private:
    //  フェードアウト用アルファ値
    float m_alfa;
    //  フェードアウトは始まったか
    bool m_isStartFadeOut;

    //  クラスへのポインタ
    class EnergyGaugeUI* m_energyGaugeUI;   //  エネルギーゲージUIの描画
    class LifeUI* m_lifeUI;     //  残機UIの描画
    class GameSceneScoreUI* m_scoreUI;      //  スコアUIの描画
    class ColorValue* m_color;      //  色の設定

    //  シングルトンで取得するクラス
    class SpriteDrawer& m_sprite;       //  画像の描画
};
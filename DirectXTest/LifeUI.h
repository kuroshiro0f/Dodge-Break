#pragma once

//  残機のUIを描画するクラス
class LifeUI
{
public:
    LifeUI();
    ~LifeUI();

    //  初期化
    void Init(class IPlayerParameterGetter* _playerParameterGetter);
    //  リセット
    void Reset();

    //  更新
    void Update();

    //  描画
    void Draw();
public:
    //  残機の減少アニメーション
    void PlayAnimationDecreaseLife();

    //  残機量
    int m_displayLifeNum;

    //  アニメーション中か
    bool m_isPlayAnimation;

    //  残機の数を得るためのクラスへのポインタ
    IPlayerParameterGetter* m_playerParameterGetter;
    //  時間計測のクラスへのポインタ
    class Timer* m_flashingTimer;   //  点滅間隔計測用
    Timer* m_PlayAnimationTimer;      //  アニメーション時間計測用

    //  画像描画クラスのシングルトンインスタンスを持つための変数
    class SpriteDrawer& m_sprite;
};
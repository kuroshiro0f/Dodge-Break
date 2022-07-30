#pragma once

//  タイトルシーンのUIクラス
class TitleUI
{
public:
    TitleUI();
    ~TitleUI();

    //  更新
    void Update(const bool _isClick);

    //  画像描画
    void DrawSprite();
private:
    //  点滅アニメーション
    void PlayAnimationFlashing(const float _flashingSpeed);
    //  透明化させるか
    bool m_isMakeTransparent;
    //  アルファ値
    float m_alfaValue;

    //  クラスへのポインタ
    class ColorValue* m_color;  //  色の値を返す

    //  シングルトンクラスへの参照
    class SpriteDrawer& m_sprite;   //  画像の描画
};
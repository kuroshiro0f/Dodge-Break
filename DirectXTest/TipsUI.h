#pragma once

enum SpriteType;
//  TipsのUIを表示するクラス
class TipsUI
{
public:
    TipsUI();
    ~TipsUI();

    //  表示するTipsを決定
    void DecideDisplayTips();

    //  画像描画
    void DrawSprite();
private:
    //  表示する画像の種類
    SpriteType m_spriteType;

    //  画像の描画クラスのインスタンスをシングルトンで取得
    class SpriteDrawer& m_sprite;
};
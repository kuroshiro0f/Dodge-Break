#pragma once
#include <DirectXColors.h>

using namespace DirectX;

enum class ColorType;
//  色の値を返すクラス
//  関数などで色の指定が必要な際はこのクラスを使用する
//  他のクラスで使う際はColorType.hもincludeする必要がある
class ColorValue
{
public:
    ColorValue();
    ~ColorValue();

    //  色を指定する時に呼び出す関数
    //  ColorTypeとアルファ値を指定
    XMVECTORF32 GetColorWithAlfa(const ColorType _colorType, const float _alfa = 1.0f);
private:
    //  色の情報を持つ構造体へのポインタ
    struct ColorData* m_data;
};
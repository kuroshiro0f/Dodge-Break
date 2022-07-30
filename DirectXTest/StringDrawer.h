#pragma once
#include <wrl.h>
#include <SpriteFont.h>
#include <ResourceUploadBatch.h>

#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib,"dxguid.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

//  定数
const float SHIFT_STRING = 2.0f;

//  文字を描画するクラス
class StringDrawer
{
public:
    StringDrawer();
    ~StringDrawer();

    //  黒い影付き黄色文字の描画
    void DrawStringBlackAndYellow(const char* _string, const XMFLOAT2& _pos, const float _scale = 1.0f, const float _alfa = 1.0f, const float _rotation = 0.0f);

    //  黒い影付き黄色整数の描画
    void DrawStringBlackAndYellowForNumber(const int _num, const XMFLOAT2& _pos, const float _scale = 1.0f, const float _alfa = 1.0f, const float _rotation = 0.0f);

    //  黒い影付き黄色小数の描画
    void DrawStringBlackAndYellowForFewNumber(const float _num, const XMFLOAT2& _pos, const float _scale = 1.0f, const float _alfa = 1.0f, const float _rotation = 0.0f);

private:
    //  クラスへのポインタ
    class ColorValue* m_color;  //  色の値を返す

    //  シングルトンクラスへの参照
    class Device& m_device;     //  デバイスの管理
};
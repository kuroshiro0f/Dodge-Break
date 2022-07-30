#include "ColorValue.h"

#include "ColorType.h"
#include "ColorData.h"

ColorValue::ColorValue()
    :m_data(new ColorData())
{
    //  処理なし
}

ColorValue::~ColorValue()
{
    delete m_data;
}

//  色を指定する時に呼び出す関数
//  ColorTypeとアルファ値を指定
XMVECTORF32 ColorValue::GetColorWithAlfa(const ColorType _colorType, const float _alfa)
{
    return { m_data->Color[_colorType].f[0],m_data->Color[_colorType].f[1],m_data->Color[_colorType].f[2],_alfa };
}

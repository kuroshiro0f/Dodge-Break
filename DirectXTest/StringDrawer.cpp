#include "StringDrawer.h"

#include <string>

#include "ColorType.h"
#include "ColorValue.h"

#include "Singleton.h"
#include "Device.h"

StringDrawer::StringDrawer()
    :m_device(Singleton<Device>::GetInstance())
    , m_color(new ColorValue())
{
    //  処理なし
}

StringDrawer::~StringDrawer()
{
    delete m_color;
}

//  黒い影付き黄色文字の描画
void StringDrawer::DrawStringBlackAndYellow(const char* _string, const XMFLOAT2& _pos, const float _scale, const float _alfa, const float _rotation)
{
    const XMFLOAT2& origin = { 0,0 };

    XMFLOAT2 secondPos = { _pos.x - SHIFT_STRING,_pos.y - SHIFT_STRING };
    m_device.spriteFont->DrawString(m_device.spriteBatch, _string, _pos, m_color->GetColorWithAlfa(ColorType::StringShadowColor, _alfa), _rotation, origin, _scale);
    m_device.spriteFont->DrawString(m_device.spriteBatch, _string, secondPos, m_color->GetColorWithAlfa(ColorType::StringColor, _alfa), _rotation, origin, _scale);
}

//  黒い影付き黄色整数の描画
void StringDrawer::DrawStringBlackAndYellowForNumber(const int _num, const XMFLOAT2& _pos, const float _scale, const float _alfa, const float _rotation)
{
    const XMFLOAT2& origin = { 0,0 };

    std::string str = std::to_string(_num);
    XMFLOAT2 secondPos = { _pos.x - SHIFT_STRING,_pos.y - SHIFT_STRING };
    m_device.spriteFont->DrawString(m_device.spriteBatch, str.c_str(), _pos, m_color->GetColorWithAlfa(ColorType::StringShadowColor, _alfa), _rotation, origin, _scale);
    m_device.spriteFont->DrawString(m_device.spriteBatch, str.c_str(), secondPos, m_color->GetColorWithAlfa(ColorType::StringColor, _alfa), _rotation, origin, _scale);
}

//  黒い影付き黄色小数の描画
void StringDrawer::DrawStringBlackAndYellowForFewNumber(const float _num, const XMFLOAT2& _pos, const float _scale, const float _alfa, const float _rotation)
{
    const XMFLOAT2& origin = { 0,0 };

    std::string str = std::to_string(_num);
    XMFLOAT2 secondPos = { _pos.x - SHIFT_STRING,_pos.y - SHIFT_STRING };
    m_device.spriteFont->DrawString(m_device.spriteBatch, str.c_str(), _pos, m_color->GetColorWithAlfa(ColorType::StringShadowColor, _alfa), _rotation, origin, _scale);
    m_device.spriteFont->DrawString(m_device.spriteBatch, str.c_str(), secondPos, m_color->GetColorWithAlfa(ColorType::StringColor, _alfa), _rotation, origin, _scale);
}

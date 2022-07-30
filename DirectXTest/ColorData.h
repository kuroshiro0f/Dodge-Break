#pragma once
#include <unordered_map>
#include <DirectXColors.h>

#include "ColorType.h"

using namespace DirectX;

//  ColorTypeを指定すると、色を決定するための値を返す
//  アルファ値の変更をする場合はColorValueクラスを利用
//  他のクラスで使う際はColorType.hもincludeする必要がある
struct ColorData
{
    std::unordered_map<ColorType, XMVECTORF32> Color =
    {
        {ColorType::StringShadowColor,Colors::Black/* = {(0.0f),(0.0f),(0.0f),(1.0f)}*/},
        {ColorType::StringColor,Colors::White/* = {(1.0f),(1.0f),(1.0f),(1.0f)}*/},

        {ColorType::DefaultSpriteColor,Colors::White/* = {(1.0f),(1.0f),(1.0f),(1.0f)}*/}
    };
};
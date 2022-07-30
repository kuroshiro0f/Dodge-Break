#pragma once

//  色の種類
//  何に使われている色かで種類分け
enum class ColorType
{
    StringShadowColor,  //  文字の影の色
    StringColor,        //  文字の内側の色

    DefaultSpriteColor  //  画像の色を変更しない時に指定する

    , None
};
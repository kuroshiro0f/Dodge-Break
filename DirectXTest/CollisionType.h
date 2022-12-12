#pragma once

//  衝突判定のあるオブジェクトの形状の種類
enum class CollisionType
{
    None = 0,   //  初期化用
    Sphere,     //  球状
    Box         //  箱型
};
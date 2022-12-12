#pragma once

//  箱型オブジェクトのデータ
//  NOTE: 現状の衝突判定では矩形扱いで判定するため、
//        中身も矩形のデータを持たせる
struct BoxData
{
    float width;    //  X方向の広さ
    float height;   //  Y方向の高さ
    float depth;    //  Z方向の奥行

    float angle;    //  角度
};
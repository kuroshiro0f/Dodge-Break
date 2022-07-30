#pragma once
#include <unordered_map>
#include <DirectXMath.h>

#include "SpriteType.h"

using namespace DirectX;

//  画像のデータ
namespace SpriteData
{
    //  ファイルへのパス
    std::unordered_map<SpriteType, const wchar_t*> FILE_PATH =
    {
        { SpriteType::TitleBack,L"Data/Img/Title/TitleBack.png" },
        { SpriteType::TitleText,L"Data/Img/Title/TitleLogo.png" },
        { SpriteType::Start,L"Data/Img/Title/Start.png" },

        { SpriteType::Life,L"Data/Img/GameScene/Life.png" },
        { SpriteType::Gauge,L"Data/Img/GameScene/Gauge.png" },
        { SpriteType::GaugeFrame,L"Data/Img/GameScene/GaugeFrame.png" },
        { SpriteType::Time,L"Data/Img/GameScene/Time.png" },
        
        { SpriteType::FirstEvaluate,L"Data/Img/Result/FirstEvaluate.png" },
        { SpriteType::SecondEvaluate,L"Data/Img/Result/SecondEvaluate.png" },
        { SpriteType::ThirdEvaluate,L"Data/Img/Result/ThirdEvaluate.png" },
        { SpriteType::ForthEvaluate,L"Data/Img/Result/ForthEvaluate.png" },
        { SpriteType::ResultBack,L"Data/Img/Result/ResultBack.png" },
        { SpriteType::ResultText,L"Data/Img/Result/ResultLogo.png" },
        { SpriteType::Return,L"Data/Img/Result/Return.png" },
        { SpriteType::Tips1,L"Data/Img/Result/Tips1.png" },
        { SpriteType::Tips2,L"Data/Img/Result/Tips2.png" },
        { SpriteType::Tips3,L"Data/Img/Result/Tips3.png" },

        { SpriteType::FadeOutBack,L"Data/Img/FadeOut.png" }
    };

    //  元画像の大きさ
    std::unordered_map<SpriteType, const XMINT2> SPRITE_SIZE =
    {
        {SpriteType::Gauge,{1104,262}}
    };
};
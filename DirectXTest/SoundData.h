#pragma once
#include <unordered_map>

#include "SoundType.h"

//  音のロードに必要なデータ
namespace SoundData
{
    //  ファイルへのパス
    std::unordered_map<SoundType, const wchar_t*> PATH_MAP =
    {
        {SoundType::TitleBGM,L"Data/Sound/BGM/TitleBGM.wav"},
        {SoundType::TransitionGameSceneSE,L"Data/Sound/SE/Title/TransitionGameScene.wav"},

        {SoundType::GameSceneBGM,L"Data/Sound/BGM/GameSceneBGM.wav"},

        {SoundType::ResultBGM,L"Data/Sound/BGM/ResultBGM.wav"},
        {SoundType::TransitionTitleSE,L"Data/Sound/SE/Result/TransitionTitle.wav"},
        {SoundType::ScoreSE,L"Data/Sound/SE/Result/Score.wav"},
        {SoundType::ValueSE,L"Data/Sound/SE/Result/Value.wav"},
        {SoundType::PraiseSE,L"Data/Sound/SE/Result/Praise.wav"},

        {SoundType::AppearPlayerSE,L"Data/Sound/SE/Player/AppearPlayer.wav"},
        {SoundType::DodgeSE,L"Data/Sound/SE/Player/Dodge.wav"},
        {SoundType::DamagePlayerSE,L"Data/Sound/SE/Player/DamagePlayer.wav"},

        {SoundType::DefeatEnemySE,L"Data/Sound/SE/Enemy/DefeatEnemy.wav"},

        {SoundType::AbsorpEnergySE,L"Data/Sound/SE/EnemyDefeater/AbsorpEnergy.wav"},
        {SoundType::ActivateEnemyDefeaterSE,L"Data/Sound/SE/EnemyDefeater/ActivateEnemyDefeater.wav"},
        {SoundType::UseEnemyDefeaterSE,L"Data/Sound/SE/EnemyDefeater/UseEnemyDefeater.wav"},

        {SoundType::AttackSE,L"Data/Sound/SE/Attack/Attack.wav"}
    };

    //  初期音量
    std::unordered_map<SoundType, float> FIRST_VOLUME =
    {
        {SoundType::TitleBGM,0.2f},
        {SoundType::TransitionGameSceneSE,0.1f},

        {SoundType::GameSceneBGM,0.05f},

        {SoundType::ResultBGM,0.1f},
        {SoundType::TransitionTitleSE,0.2f},
        {SoundType::ScoreSE,0.1f},
        {SoundType::ValueSE,0.3f},
        {SoundType::PraiseSE,0.1f},

        {SoundType::AppearPlayerSE,0.2f},
        {SoundType::DodgeSE,0.5f},
        {SoundType::DamagePlayerSE,2.0f},

        {SoundType::DefeatEnemySE,0.1f},

        {SoundType::AbsorpEnergySE,0.03f},
        {SoundType::ActivateEnemyDefeaterSE,0.5f},
        {SoundType::UseEnemyDefeaterSE,0.8f},

        {SoundType::AttackSE,0.3f}
    };
};
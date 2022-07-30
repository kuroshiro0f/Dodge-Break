#pragma once
#include <string>
#include <unordered_map>

#include "EffectType.h"

//  エフェクトのロードに必要なデータの集合体
namespace EffectData 
{
    //  ファイルへのパス
    std::unordered_map<EffectType, const wchar_t*> PATH_MAP =
    {
        {EffectType::AppearancePlayer,L"Data/Effect/Player/AppearPlayer.efk"},
        {EffectType::DamagePlayer,L"Data/Effect/Player/DamagePlayer.efk"},
        {EffectType::ExitPlayer,L"Data/Effect/Player/ExitPlayer.efk"},

        {EffectType::DodgeEnergy ,L"Data/Effect/Energy/DodgeEffect.efk"},
        {EffectType::DodgeEnergyForEnergyMax ,L"Data/Effect/Energy/DodgeEffectForEnergyMax.efk"},

        {EffectType::DefeatAimEnemy, L"Data/Effect/Enemy/DefeatAimEnemy.efk"},
        {EffectType::DefeatNoAimEnemy, L"Data/Effect/Enemy/DefeatNoAimEnemy.efk"},
        {EffectType::DefeatUniqueShootEnemy, L"Data/Effect/Enemy/DefeatUniqueShootEnemy.efk"},

        {EffectType::HitAimEnemyTower, L"Data/Effect/Obstacle/HitAimEnemyTower.efk"},
        {EffectType::HitNoAimEnemyTower, L"Data/Effect/Obstacle/HitNoAimEnemyTower.efk"},
        {EffectType::HitUniqueShootEnemyTower, L"Data/Effect/Obstacle/HitUniqueShootEnemyTower.efk"},
        {EffectType::ChargeAimEnergy, L"Data/Effect/Obstacle/ChargeEnergyForAim.efk"},
        {EffectType::ChargeNoAimEnergy, L"Data/Effect/Obstacle/ChargeEnergyForNoAim.efk"},
        {EffectType::ChargeUniqueShootEnergy, L"Data/Effect/Obstacle/ChargeEnergyForUniqueShoot.efk"},
        {EffectType::ActivateAimEnemyTower,L"Data/Effect/Obstacle/ActivateAimEnemyCube.efk"},
        {EffectType::ActivateNoAimEnemyTower,L"Data/Effect/Obstacle/ActivateNoAimEnemyCube.efk"},
        {EffectType::ActivateUniqueShootEnemyTower,L"Data/Effect/Obstacle/ActivateUniqueShootEnemyCube.efk"}
    };

    //  テクスチャなどが保存されてるファイルへのパス
    std::unordered_map<EffectType, const wchar_t*> FILE_MAP =
    {
        {EffectType::AppearancePlayer,L"Data/Effect/Player"},
        {EffectType::DamagePlayer,L"Data/Effect/Player"},
        {EffectType::ExitPlayer,L"Data/Effect/Player"},

        {EffectType::DodgeEnergy,L"Data/Effect/Energy"},
        {EffectType::DodgeEnergyForEnergyMax,L"Data/Effect/Energy"},

        {EffectType::DefeatAimEnemy, L"Data/Effect/Enemy"},
        {EffectType::DefeatNoAimEnemy, L"Data/Effect/Enemy"},
        {EffectType::DefeatUniqueShootEnemy, L"Data/Effect/Enemy"},
        
        {EffectType::HitAimEnemyTower, L"Data/Effect/Obstacle"},
        {EffectType::HitNoAimEnemyTower, L"Data/Effect/Obstacle"},
        {EffectType::HitUniqueShootEnemyTower, L"Data/Effect/Obstacle"},
        {EffectType::ChargeAimEnergy, L"Data/Effect/Obstacle"},
        {EffectType::ChargeNoAimEnergy, L"Data/Effect/Obstacle"},
        {EffectType::ChargeUniqueShootEnergy, L"Data/Effect/Obstacle"},
        {EffectType::ActivateAimEnemyTower, L"Data/Effect/Obstacle"},
        {EffectType::ActivateNoAimEnemyTower, L"Data/Effect/Obstacle"},
        {EffectType::ActivateUniqueShootEnemyTower, L"Data/Effect/Obstacle"}
    };

    //  拡大率
    std::unordered_map<EffectType, const float> MAGNIFICATION_MAP =
    {
        {EffectType::AppearancePlayer, 2.5f},
        {EffectType::DamagePlayer,4.0f},
        {EffectType::ExitPlayer, 2.5f},

        {EffectType::DodgeEnergy, 1.0f},
        {EffectType::DodgeEnergyForEnergyMax, 1.0f},

        {EffectType::DefeatAimEnemy, 2.0f},
        {EffectType::DefeatNoAimEnemy, 2.0f},
        {EffectType::DefeatUniqueShootEnemy, 2.0f},
        
        {EffectType::HitAimEnemyTower, 1.2f},
        {EffectType::HitNoAimEnemyTower, 1.2f},
        {EffectType::HitUniqueShootEnemyTower, 1.2f},
        {EffectType::ChargeAimEnergy, 1.2f},
        {EffectType::ChargeNoAimEnergy, 1.2f},
        {EffectType::ChargeUniqueShootEnergy, 1.2f},
        {EffectType::ActivateAimEnemyTower, 1.2f},
        {EffectType::ActivateNoAimEnemyTower, 1.2f},
        {EffectType::ActivateUniqueShootEnemyTower, 1.2f}
    };
};
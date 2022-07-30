#pragma once
#include <unordered_map>
#include <string>

#include "PMDModelType.h"

//  PMDModelのロード用データ
namespace PMDModelData
{
    //  各モデルのファイルへのパス
    std::unordered_map<PMDModelType, const std::string> FILE_PATH =
    {
        //  プレイヤー
        {PMDModelType::Player,"Data/Model/Player/Player.pmd"},
        {PMDModelType::PlayerShadow,"Data/Model/Player/PlayerShadow.pmd"},

        //  エネミー
        {PMDModelType::AimEnemy,"Data/Model/Enemy/AimEnemy.pmd"},
        {PMDModelType::NoAimEnemy,"Data/Model/Enemy/NoAimEnemy.pmd"},
        {PMDModelType::UniqueShootEnemy,"Data/Model/Enemy/UniqueShootEnemy.pmd"},
        {PMDModelType::EnemyShadow,"Data/Model/Enemy/EnemyShadow.pmd"},

        //  弾
        {PMDModelType::AimingBullet,"Data/Model/Bullet/AimBullet.pmd"},
        {PMDModelType::NoAimingBullet,"Data/Model/Bullet/NoAimBullet.pmd"},
        {PMDModelType::UniqueBullet,"Data/Model/Bullet/UniqueShootBullet.pmd"},
        {PMDModelType::ZigzagBullet,"Data/Model/Bullet/Zigzag.pmd"},
        {PMDModelType::HitZigzagBullet,"Data/Model/Bullet/HitZigzag.pmd"},
        {PMDModelType::BulletShadow,"Data/Model/Bullet/BulletShadow.pmd"},

        //  エネミーを倒す塔
        //  TODO: 各ファイルの名称をObstacleからEnemyDefeaterに修正
        //  土台
        {PMDModelType::Pedestal,"Data/Model/Obstacle/Pedestal.pmd"},
        //  宙に浮いている立方体部分
        {PMDModelType::AimEnemyDefeater,"Data/Model/Obstacle/AimObstacle.pmd"},
        {PMDModelType::NoAimEnemyDefeater,"Data/Model/Obstacle/NoAimObstacle.pmd"},
        {PMDModelType::UniqueShootEnemyDefeater,"Data/Model/Obstacle/UniqueShootObstacle.pmd"},

        //  地面
        {PMDModelType::Ground,"Data/Model/Others/Ground.pmd"},

        //  背景
        {PMDModelType::BackGround,"Data/Model/Others/UnderGround.pmd"}
    };
};
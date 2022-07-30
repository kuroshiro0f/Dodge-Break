#pragma once
#include <string>
#include <unordered_map>

#include "MapChipType.h"

//  マップチップのロードに必要なデータの集合体
namespace MapChipData
{
    std::unordered_map<MapChipType, const std::string> PATH_MAP =
    {
        {MapChipType::Obstacle,"Data/Map/Obstacle.csv"},

        {MapChipType::First,"Data/Map/FirstStage.csv"},

        {MapChipType::Random1, "Data/Map/Stage01.csv"},
        {MapChipType::Random2, "Data/Map/Stage02.csv"},
        {MapChipType::Random3, "Data/Map/Stage03.csv"},
        {MapChipType::Random4, "Data/Map/Stage04.csv"},
        {MapChipType::Random5, "Data/Map/Stage05.csv"},

        {MapChipType::Last, "Data/Map/LastStage.csv"}
    };
};
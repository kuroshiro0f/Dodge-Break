#if _DEBUG
#include "DataReLoader.h"

DataReLoader::DataReLoader()
{
    //  処理なし
}

DataReLoader::~DataReLoader()
{
    m_reloadFunc.clear();
    m_reloadFunc.shrink_to_fit();
}

//  データの再読み込みを行う
void DataReLoader::ReLoad()
{
    for (auto itr : m_reloadFunc)
    {
        itr();
    }
}
#endif
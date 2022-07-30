#pragma once

#if _DEBUG
#include <functional>
#include <vector>

//  データの再読み込みを行うクラス
//  シングルトンで扱う
class DataReLoader
{
public:
    DataReLoader();
    ~DataReLoader();

    //  データの再読み込みを行うための関数を登録
    void RegisterReLoadFunc(std::function<void()> _reloadFunc) { m_reloadFunc.emplace_back(_reloadFunc); }

    //  データの再読み込みを行う
    void ReLoad();
private:
    //  データの再読み込みを行うための関数を格納する配列
    std::vector<std::function<void()>> m_reloadFunc;
};
#endif
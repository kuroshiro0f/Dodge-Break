#pragma once
#include <type_traits>

//  列挙型Tのイテレータを作成するクラス
//  beginVal: イテレータの最初の要素
//  endVal: イテレータの最後の要素
//  NOTE: テンプレートクラスのためヘッダーに定義を記述
template < typename T, T beginVal, T endVal>
class EnumIterator {
public:
    //  イテレータの始端を指すコンストラクタ
    EnumIterator() : m_val(static_cast<m_val_t>(beginVal)) {}
    //  イテレータの終端を指すコンストラクタ
    EnumIterator(const T& _end) : m_val(static_cast<m_val_t>(_end)) {}
    //  イテレータが次の要素を指すように演算子++をオーバーロード
    EnumIterator operator++()
    {
        m_val++;
        return *this;
    }
    //  イテレータが現在指している要素を返すように間接演算子をオーバーロード
    T operator*()const { return static_cast<T>(m_val); }
    //  始端の要素
    EnumIterator begin()const { return *this; }
    //  終端の要素
    EnumIterator end() {
        static const EnumIterator endIter = ++EnumIterator(endVal);
        return endIter;
    }
    //  イテレータの終端を検知するために演算子!=をオーバーロード
    bool operator!=(const EnumIterator& _end)const { return m_val != _end.m_val; }
private:
    //  列挙型を安全にキャストできるように列挙型Tの基底型を取得
    //  NOTE: underlying_type<T>::typeで列挙型Tの基底型を取得できる
    typedef typename std::underlying_type<T>::type m_val_t;
    //  現在指している要素の番号
    int m_val;
};
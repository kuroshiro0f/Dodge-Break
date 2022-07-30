#include "EventNotificator.h"

#include "EventType.h"

EventNotificator::EventNotificator()
    :m_eventList({})
{
    //  処理なし
}

EventNotificator::~EventNotificator()
{
    m_eventList.clear();
}

//  登録
void EventNotificator::Register(const EventType _type, const std::function<void()> _func)
{
    m_eventList.insert(std::pair<EventType, std::function<void()>>(_type, _func));
}

//  通知
void EventNotificator::Notificate(const EventType _type)
{
    //  リストの中から、_typeで指定されたキーと紐づけられた範囲を検索
    auto range = m_eventList.equal_range(_type);
    for (auto itr = range.first; itr != range.second; itr++)
    {
        //  リストに登録された関数を実行
        auto targetPair = *itr;
        targetPair.second();
    }
}

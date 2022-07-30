#pragma once
#include <unordered_map>
#include <functional>

enum class EventType;
//  イベントを登録し、通知を行うクラス
//  Register関数でイベントに対するリアクションを行う関数のポインタを登録して、
//  適切なタイミングでNotificate関数を呼び出すことで、イベントを通知できる
class EventNotificator
{
public:
    EventNotificator();
    ~EventNotificator();

    //  登録
    void Register(const EventType _type, const std::function<void()> _func);

    //  通知
    void Notificate(const EventType _type);
private:
    //  イベントが起きた際に実行する関数のポインタのリスト
    std::unordered_multimap<EventType, std::function<void()>> m_eventList;
};
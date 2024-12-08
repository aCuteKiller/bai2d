//
// Created by 22190 on 2024/12/3.
//

#ifndef BAI2D_EVENT_BASE_H
#define BAI2D_EVENT_BASE_H
#pragma once

#include<vector>
#include<map>
#include<string>
#include <iostream>
#include "graphics.h"
#include "bai2d_constants.h"

class EventAble;

/**
 * @class SlotArgs
 * @brief 事件管理器中依赖的槽的参数类型
 *
 * @author aCuteKiller
 * @date 2024/12/3
 *
 * @section 主要功能
 * - [列出主要功能点 1]
 * - [列出主要功能点 2]
 */
class SlotArgs {
};

struct InputInfo : public SlotArgs {
    POINT clickPosition;

    // 添加构造函数并初始化 clickPosition
    InputInfo() : clickPosition({0, 0}) {}
};

// 信号函数指针

using Signal = void (EventAble::*)();

// 槽函数指针
using Slot = void (EventAble::*)(const SlotArgs &);

// 用于标记信号
namespace bai {
    typedef void signal;
    typedef void slot;
}

// SIGNAL和SLOT是为了应对与父类类型不一致的问题
// 在设计基础事件管理器时，是针对一个顶级基类Object设计的，后续需要使用的个管理的所有类，都需要继承（public）自Object。
#define SIGNAL(signal) reinterpret_cast<Signal>(signal)
#define SLOT(slot) reinterpret_cast<Slot>(slot)

/**
 * @class EventAble
 * 要使用事件管理器，所有需要使用到事件的类都需要继承自EventAble。
 */
class EventAble {

};


class Event {
    class Executor;

private:
    EventAble *sender;
    Signal signal_;
    std::vector<Executor *> executors;

    class Executor {
    public:
        Slot slot_;
        EventAble *receiver;
    public:
        Executor(Slot slot_, EventAble &receiver);

        ~Executor();

        void notify(const SlotArgs &slotArgs) const;

        bool operator==(const Executor &r) const;
    };

public:
    Event(EventAble &sender, Signal signal_);

    ~Event();

    EventAble *getSender();

    Signal getSignal();

    void addExecutor(Slot slot_, EventAble &receiver);

    void removeExecutor(Slot slot_, EventAble &receiver);

    void emit(const SlotArgs &slotArgs);
};

/**
 * @class BaseEventManager
 * @brief 基础事件管理器
 *
 * @author aCuteKiller
 * @date 2024/12/3
 *
 * @section 主要功能
 * - [列出主要功能点 1]
 * - [列出主要功能点 2]
 */
class BaseEventManager {

protected:
    // 事件集
    std::vector<Event *> events;
public:
    BaseEventManager();

    ~BaseEventManager();

    void disConnect(EventAble &sender, Signal signal_, EventAble &receiver, Slot slot_);

    void connect(EventAble &sender, Signal signal_, EventAble &receiver, Slot slot_);

    void removeEvent(EventAble &sender, Signal signal_);

    void notify(const EventAble &sender, Signal signal_,const SlotArgs &slotArgs);

};

#endif // BAI2D_EVENT_BASE_H

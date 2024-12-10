//
// Created by 22190 on 2024/12/3.
//

#include "bai2d_event_base.h"

void Event::Executor::notify(const SlotArgs &slotArgs) const {
    (this->receiver->*slot_)(slotArgs); // 解引用指针
}

Event::Executor::~Executor() {
    this->receiver = nullptr;
    this->slot_ = nullptr;
}

bool Event::Executor::operator==(const Event::Executor &r) const {
    return receiver == r.receiver && slot_ == r.slot_;
}

Event::Executor::Executor(Slot slot_, EventAble &receiver) {
    this->slot_ = slot_;
    this->receiver = &receiver;
}

Event::Event(EventAble &sender, Signal signal_) {
    this->sender = &sender;
    this->signal_ = signal_;
    this->executors = std::vector<Executor *>();
}

Event::~Event() {
    this->sender = nullptr;
    this->signal_ = nullptr;
    for (auto *e: executors) {
        delete e;
    }
}

Signal Event::getSignal() {
    return signal_;
}

EventAble *Event::getSender() {
    return sender;
}

void Event::addExecutor(Slot slot_, EventAble &receiver) {
    for (auto *e: executors) {
        if (*e == Executor(slot_, receiver)) {
            return;
        }
    }
    executors.push_back(new Executor(slot_, receiver));
}

void Event::emit(const SlotArgs &slotArgs) {
    for (auto *e: executors) {
        e->notify(slotArgs);
    }
}

void Event::removeExecutor(Slot slot_, EventAble &receiver) {
    for (auto it = executors.begin(); it != executors.end(); ++it) {
        if ((*it)->receiver == &receiver && (*it)->slot_ == slot_) {
            delete *it;
            executors.erase(it);
            break;
        }
    }
}

void BaseEventManager::notify(const EventAble &sender, Signal signal_, const SlotArgs &slotArgs) {
    for (auto *e: events) {
        try {
            if (e->getSender() == &sender && e->getSignal() == signal_) {
                e->emit(slotArgs);
            }
        } catch (const std::exception &e) {}
    }
}

void BaseEventManager::disConnect(EventAble &sender, Signal signal_, EventAble &receiver, Slot slot_) {
    for (auto *e: events) {
        if (e->getSender() == &sender && e->getSignal() == signal_) {
            e->addExecutor(slot_, receiver);
            return;
        }
    }
    auto *pEvent = new Event(sender, signal_);
    pEvent->addExecutor(slot_, receiver);
    events.push_back(pEvent);
}

void BaseEventManager::connect(EventAble &sender, Signal signal_, EventAble &receiver, Slot slot_) {
    for (auto *e: events) {
        if (e->getSender() == &sender && e->getSignal() == signal_) {
            e->removeExecutor(slot_, receiver);
            return;
        }
    }
    auto pEvent = new Event(sender, signal_);
    pEvent->addExecutor(slot_, receiver);
    events.push_back(pEvent);
}

BaseEventManager::BaseEventManager() {
    this->events = std::vector<Event *>();
}

BaseEventManager::~BaseEventManager() {
    for (auto *e: events) {
        delete e;
    }
    events.clear();
}

void BaseEventManager::removeEvent(EventAble &sender, Signal signal_) {
    for (auto it = events.begin(); it != events.end(); ++it) {
        if ((*it)->getSender() == &sender && (*it)->getSignal() == signal_) {
            delete *it;
            events.erase(it);
            break;
        }
    }
}


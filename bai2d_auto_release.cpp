//
// Created by 22190 on 2024/12/10.
//

#include "bai2d_auto_release.h"

void Ref::incrementRefCount() {
    refCount++;
}

int Ref::decrementRefCount() {
    return --refCount;
}

int Ref::getRefCount() const {
    return refCount;
}

bool Ref::isAutoRelease() const {
    return this->refCount <= 1;
}

void Ref::release() {
    if (this->isAutoRelease()) {
        delete this;
    }
}

void Ref::autoRelease() {
    AutoReleasePool::getInstance().addRef(this);
}

Ref::Ref() {
    refCount = 1;
}

void AutoReleasePool::tryRelease() {
    for (auto &ref: refs) {
        ref->autoRelease();
    }
}

AutoReleasePool::AutoReleasePool() {
    this->refs = std::vector<Ref *>();
}

AutoReleasePool &AutoReleasePool::getInstance() {
    return instance;
}

void AutoReleasePool::addRef(Ref *ref) {
    for (auto &r: refs) {
        if (r == ref) {
            return;
        }
    }
    refs.push_back(ref);
}

AutoReleasePool AutoReleasePool::instance;
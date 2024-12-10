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
        popFromPool();
        delete this;
    }
}

void Ref::autoRelease() {
    AutoReleasePool::getInstance().addRef(this);
}

Ref::Ref() {
    refCount = 1;
}

void Ref::popFromPool() {
    AutoReleasePool::getInstance().removeRef(this);
}

void AutoReleasePool::tryRelease() {
    for (auto &ref: refs) {
        ref->release();
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

unsigned long long AutoReleasePool::getPoolSize() {
    return this->refs.size();
}

void AutoReleasePool::removeRef(Ref *ref) {
    for (int i = 0; i < refs.size(); i++) {
        if (refs[i] == ref) {
            refs.erase(refs.begin() + i);
            return;
        }
    }
}

AutoReleasePool AutoReleasePool::instance;
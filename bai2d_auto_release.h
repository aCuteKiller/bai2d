//
// Created by 22190 on 2024/12/10.
//

#ifndef BAI2D_AUTO_RELEASE_H
#define BAI2D_AUTO_RELEASE_H

#include <vector>

#define CREATE_FUNC(__TYPE__) \
static __TYPE__ *create()     \
{                             \
__TYPE__ *pRet = new (std::nothrow) __TYPE__(); \
if (pRet){                    \
pRet->autoRelease();          \
return pRet;\
}                             \
else{                         \
delete pRet;                  \
pRet= nullptr;                \
return nullptr;\
}\
}                             \

#define CREATE(__TYPE__) __TYPE__::create()

class AutoReleasePool;

class Ref {
private:
    int refCount;
public:
    Ref();

    virtual ~Ref() = default;

    void incrementRefCount();

    int decrementRefCount();

    [[nodiscard]] int getRefCount() const;

    [[nodiscard]] virtual bool isAutoRelease() const;

    virtual void release();

    void autoRelease();
};

class AutoReleasePool {
private:
    std::vector<Ref *> refs;
    static AutoReleasePool instance;

    AutoReleasePool();

public:
    static AutoReleasePool &getInstance();

    void tryRelease();

    void addRef(Ref *ref);
};

#endif //BAI2D_AUTO_RELEASE_H

//
// Created by 22190 on 2024/12/5.
//

#include <cassert>
#include "bai2d_object.h"


Mesh &Object::getMesh() const {
    return *this->mesh;
}

BaseObject::BaseObject(const POINT &position) : BaseObject(position, DEFAULT_WIDTH, DEFAULT_HEIGHT) {
}

Object::Object(const POINT &position, RectMesh *mesh, CollisionAble *collision) : Object(position,
                                                                                         BaseObject::DEFAULT_WIDTH,
                                                                                         BaseObject::DEFAULT_HEIGHT,
                                                                                         mesh, collision) {
}

void Object::draw() {
    if (this->getIsVisible())
        this->mesh->show();
    if (this->isCheckCollision)
        this->collision->check();
}

BaseObject *BaseObject::setVisible(bool b) {
    this->visible = b;
    return this;
}

BaseObject *BaseObject::setPosition(const POINT &point) {
    this->position.x = point.x;
    this->position.y = point.y;
    return this;
}

BaseObject *BaseObject::setRotateAngle(int angle) {
    this->rotateAngle = angle;
    return this;
}

Object *Object::setMesh(Mesh *m) {
    this->mesh = m;
    return this;
}

Object *Object::setCollision(CollisionAble *c) {
    delete this->collision;
    this->collision = c;
    return this;
}

CollisionAble *Object::getCollision() const {
    return this->collision;
}

POINT BaseObject::getPosition() {
    return this->position;
}

bool BaseObject::isVisible() const {
    return this->visible;
}

int BaseObject::getRotateAngle() const {
    return this->rotateAngle;
}

Object::Object(const POINT &position, RectMesh *mesh) : Object(position, mesh, nullptr) {
}

Object::Object(const POINT &position) : Object(position, nullptr, nullptr) {
}

const int BaseObject::DEFAULT_WIDTH = 50;
const int BaseObject::DEFAULT_HEIGHT = 50;

BaseObject::BaseObject() : BaseObject(POINT{0, 0}) {

}

const int Pawn::ACTION_FRAME_INTERVAL = 30;

BaseObject *BaseObject::setRenderingIndex(int index) {
    this->renderingIndex = index;
    return this;
}

int BaseObject::getRenderingIndex() const {
    return this->renderingIndex;
}

BaseObject *BaseObject::update() {
    updateRealPosition();
    return this;
}

Pawn *Pawn::update() {
    Actor::update();
    if (TimeUtils::getNowMilliseconds() - lastMoveTime > 1000 / ACTION_FRAME_INTERVAL) {
        lastMoveTime = TimeUtils::getNowMilliseconds();
        jumpOneStep();
        this->updateAction();
    }
    return this;
}

void BaseObject::updateRealPosition() {
    BaseObject *obj = getParentObj();
    POINT realPosition = POINT();
    if (obj != nullptr) {
//        obj->updateRealPosition();
        realPosition.x += this->getParentOffset().x;
        realPosition.y += this->getParentOffset().y;
        realPosition.x += obj->getPosition().x;
        realPosition.y += obj->getPosition().y;
    } else {
        realPosition = this->getPosition();
    }
    this->setPosition(realPosition);
}

BaseObject *BaseObject::setPosition(int x, int y) {
    this->position.x = x;
    this->position.y = y;
    return this;
}

BaseObject *BaseObject::offsetPosition(int x, int y) {
    this->position.x += x;
    this->position.y += y;
    return this;
}

std::vector<BaseObject *> BaseObject::getAllOwnObjects() {
    std::vector<BaseObject *> objs, objsTemp;
    objs.push_back(const_cast<BaseObject *>(this)); // 使用 const_cast 转换
    if (!this->attachObjs->isEmpty()) {
        // 这里递归查询所有子对象
        objsTemp = attachObjs->getAllInOneVector();
        for (auto iteratorTemp: objsTemp) {
            objs.push_back(iteratorTemp);
        }
    }
    return objs;
}

void BaseObjectManager::addObject(BaseObject *object) {
    if (isContains(object))return;
    this->objects.push_back(object);
}

bool BaseObjectManager::isEmpty() const {
    return this->objects.empty();
}

void BaseObjectManager::removeObject(int index) {
    if (index < 0 || index >= this->objects.size()) return;
    auto iterator = this->objects.begin();
    for (int i = 0; i < index; i++) iterator++;
    this->objects.erase(iterator);
}

void BaseObjectManager::removeObject(BaseObject *object) {
    if (object != nullptr)
        for (auto iterator = this->objects.begin(); iterator != this->objects.end(); iterator++) {
            if (*iterator == object) {
                this->objects.erase(iterator);
                return;
            }
        }
}

bool BaseObjectManager::isContains(BaseObject *object) const {
    if (object != nullptr)
        for (auto iterator: this->objects) {
            if (iterator == object) return true;
        }
    return false;
}

BaseObjectManager::BaseObjectManager() {
    this->objects = std::vector<BaseObject *>();
}

BaseObjectManager::~BaseObjectManager() {
    this->objects.clear();
}

std::vector<BaseObject *> BaseObjectManager::getAllInOneVector() {
    std::vector<BaseObject *> v;
    std::vector<BaseObject *> vTemp;
    for (auto iterator: this->objects) {
        vTemp = iterator->getAllOwnObjects();
        for (auto iteratorTemp: vTemp) {
            v.push_back(iteratorTemp);
        }
    }
    return v;
}

std::vector<BaseObject *> BaseObjectManager::getAllInOneVectorWithSortAndUnique() {
    std::vector<BaseObject *> v = getAllInOneVector();
    std::set<BaseObject *> unique_objects(v.begin(), v.end());
    v.assign(unique_objects.begin(), unique_objects.end());
    std::sort(v.begin(), v.end(), [](BaseObject *a, BaseObject *b) {
        return a->getRenderingIndex() < b->getRenderingIndex();
    });
    return v;
}

BaseObject *BaseObject::attach(BaseObject *obj) {
    if (obj != nullptr && !this->attachObjs->isContains(obj)) {
        if (obj->parentObj != nullptr) {
            obj->parentObj->detach(obj);
            obj->decrementRefCount();
        }
        this->attachObjs->addObject(obj);
        obj->parentObj = this;
        obj->incrementRefCount();
    }
    return this;
}

void BaseObject::detach(BaseObject *object) {
    if (object == nullptr) return;
    this->attachObjs->removeObject(object);
    object->parentObj = nullptr;
    object->decrementRefCount();

}

BaseObject *BaseObject::getParentObj() {
    return this->parentObj;
}

void BaseObject::setParentOffset(int x, int y) {
    this->parentOffset.x = x;
    this->parentOffset.y = y;
}

POINT BaseObject::getParentOffset() const {
    return this->parentOffset;
}

BaseObject::~BaseObject() {
    for (auto iterator: *this->attachObjs) {
        iterator->decrementRefCount();
    }
    delete this->attachObjs;
    this->parentObj = nullptr;
}

Object::~Object() {
    delete this->collision;
    delete this->mesh;
}

int BaseObject::getIsVisible() const {
    return visible;
}

BaseObject *BaseObject::setAnchor(ObjectAnchor oa) {
    this->anchor = oa;
    return this;
}

ObjectAnchor BaseObject::getAnchor() const {
    return anchor;
}

BaseObject *BaseObject::setWidth(int w) {
    if (w <= 0)return this;
    this->width = w;
    return this;
}

BaseObject *BaseObject::setHeight(int h) {
    if (h <= 0)return this;
    this->height = h;
    return this;
}

int BaseObject::getWidth() const {
    return int(width * this->getScale());
}

int BaseObject::getHeight() const {
    return int(height * this->getScale());
}

BaseObject::BaseObject(const POINT &position, int w, int h) : visible(true), rotateAngle(0), renderingIndex(0), Ref() {
    assert(w > 0 && h > 0);
    this->attachObjs = new BaseObjectManager();
    this->parentObj = nullptr;
    parentOffset = POINT{0, 0};
    this->position = position;
    this->anchor = ObjectAnchor::CENTER;
    this->width = w;
    this->height = h;
    this->scale = 1;
}

POINT BaseObject::getAnchorOffset() {
    POINT offset{0, 0};
    switch (anchor) {
        case ObjectAnchor::LEFT_TOP:
            offset.x = -getWidth() / 2;
            offset.y = -getHeight() / 2;
            break;
        case ObjectAnchor::LEFT_BOTTOM:
            offset.x = -getWidth() / 2;
            offset.y = getHeight() / 2;
            break;
        case ObjectAnchor::RIGHT_TOP:
            offset.x = getWidth() / 2;
            offset.y = -getHeight() / 2;
            break;
        case ObjectAnchor::RIGHT_BOTTOM:
            offset.x = getWidth() / 2;
            offset.y = getHeight() / 2;
            break;
        case ObjectAnchor::CENTER:
            break;
    }
    return offset;
}

BaseObject *BaseObject::setScale(double s) {
    assert(s > 0);
    this->scale = s;
    return this;
}

double BaseObject::getScale() const {
    return this->scale;
}

BaseObject *BaseObject::offsetPosition(POINT offset) {
    this->position.x += offset.x;
    this->position.y += offset.y;
    return this;
}

BaseObjectManager &BaseObject::getChildren() {
    return *this->attachObjs;
}

void Pawn::updateAction() {
    // 根据移动向量，speed 通过三角函数 重新计算移动距离，然后将位置加上距离
    double angle = atan2(this->towards.y, this->towards.x);
    double x = this->towards.x != 0 ? speed * cos(angle) : 0;
    double y = this->towards.y != 0 ? speed * sin(angle) : 0;
    this->offsetPosition(static_cast<int>(std::round(x)), static_cast<int>(std::round(y)));
}

void Pawn::setSpeed(int s) {
    if (s < 0) s = 0;
    this->speed = s;
}

int Pawn::getSpeed() const {
    return this->speed;
}

bai::slot Pawn::moveUp(const SlotArgs &args) {
    this->toUp();
    this->towards.y = -1;
    this->move();
}

bai::slot Pawn::moveDown(const SlotArgs &args) {
    this->toDown();
    this->towards.y = 1;
    this->move();
}

bai::slot Pawn::moveLeft(const SlotArgs &args) {
    this->toLeft();
    this->towards.x = -1;
    this->move();
}

bai::slot Pawn::moveRight(const SlotArgs &args) {
    this->toRight();
    this->towards.x = 1;
    this->move();
}

bai::slot Pawn::resetVectorX(const SlotArgs &args) {
    this->towards.x = 0;
    if (this->towards.x == 0 && this->towards.y == 0)
        this->idle();
}

bai::slot Pawn::resetVectorY(const SlotArgs &args) {
    this->towards.y = 0;
    if (this->towards.x == 0 && this->towards.y == 0)
        this->idle();
}

Pawn::Pawn() : Actor() {
    this->jumping = false;
    this->falling = false;
    this->speed = 0;
    this->lastMoveTime = 0;
    this->jumpHeight = DEFAULT_JUMP_HEIGHT;
    this->maxJumpTimes = 1;
    this->hasJumpTimes = 0;
    this->jumpSpeed = DEFAULT_JUMP_SPEED;
    this->realJumpHeight = 0;
    this->inTheAirTime = DEFAULT_IN_THE_AIR_TIME;
    this->startInTheAir = 0;
}

const int Pawn::DEFAULT_JUMP_HEIGHT = 30;
const int Pawn::DEFAULT_JUMP_SPEED = 5;
const int Pawn::DEFAULT_IN_THE_AIR_TIME = 300;

Pawn *Pawn::setJumpHeight(int height) {
    assert(height >= 0);
    this->jumpHeight = height;
    return this;
}

Pawn *Pawn::setMaxJumpTimes(int times) {
    assert(times >= 0);
    this->maxJumpTimes = times;
    return this;
}

bool Pawn::isJumping() const {
    return this->jumping;
}

bool Pawn::isFalling() const {
    return this->falling;
}

bai::slot Pawn::jump(const SlotArgs &args) {
    if (this->isJumping() || this->isFalling()) {
        if (this->hasJumpTimes >= this->maxJumpTimes)
            return;
    }
    this->jumping = true;
    this->falling = false;
    this->hasJumpTimes++;
    if (this->hasJumpTimes > 1) {
        this->getAnimationStateMachine().resetPlayPace();
        this->realJumpHeight = this->jumpHeight + this->beforeJumpPosition.y - this->getPosition().y;
    } else {
        this->realJumpHeight += this->jumpHeight;
        this->beforeJumpPosition = this->getPosition();
    }
}

Pawn *Pawn::setJumpSpeed(int s) {
    assert(s > 1);
    this->jumpSpeed = s;
    return this;
}

void Pawn::jumpOneStep() {
    if (this->isJumping()) {
        if (!this->isFalling()) {
            startJump();
            this->offsetPosition(0, -this->jumpSpeed);
            if (this->beforeJumpPosition.y - this->getPosition().y >= this->realJumpHeight) {
                this->falling = true;
                this->startInTheAir = TimeUtils::getNowMilliseconds();
            }
        } else {
            if (TimeUtils::getNowMilliseconds() - startInTheAir < this->inTheAirTime) {
                jumpLoop();
            } else {
                fallingDown();
                this->offsetPosition(0, this->jumpSpeed);
                if (this->getPosition().y >= this->beforeJumpPosition.y) {
                    this->jumping = false;
                    if (this->towards.x != 0)
                        this->move();
                    else
                        this->idle();
                    endJump();
                }
            }
        }

    } else {
        this->falling = false;
        this->realJumpHeight = 0;
        this->hasJumpTimes = 0;
    }
}

POINT Pawn::getTowards() const {
    return this->towards;
}

bool Object::isCollision(Object *object) const {
    if (!this->isCheckCollision)return false;
    return this->collision->isCollision(object->getCollision());
}

Object::Object() : Object(POINT{0, 0}) {

}

Object::Object(const POINT &position, int w, int h, RectMesh *mesh, CollisionAble *collision) : BaseObject(position) {
    assert(w > 0 && h > 0);
    this->setWidth(w);
    this->setHeight(h);
    if (mesh == nullptr) {
        auto *pMesh = new RectMesh(position, w, h);
        pMesh->setColor(BLACK);
        this->mesh = pMesh;
        pMesh = nullptr;
    } else {
        this->mesh = mesh;
    }
    if (collision == nullptr)
        this->collision = new RectangleCollision(position, w, h);
    else
        this->collision = collision;
}

Object *Object::update() {
    BaseObject::update();
    this->collision->setPosition(EasyXUtils::getOffsetPoint(this->getPosition(), this->getAnchorOffset()));
    this->mesh->setPosition(EasyXUtils::getOffsetPoint(this->getPosition(), this->getAnchorOffset()));
    this->mesh->setRotateAngle(this->getRotateAngle());
    return this;
}

Object *Object::setIsCheckCollision(bool b) {
    this->isCheckCollision = b;
    return this;
}

Object *Object::setScale(double s) {
    BaseObject::setScale(s);
    this->collision->setScale(s);
    this->mesh->setScale(s);
    return this;
}

Object::Object(int w, int h) : Object(POINT{0, 0}, w, h, nullptr, nullptr) {

}

void Weight::draw() {

}

AnimationStateMachine &Actor::getAnimationStateMachine() {
    return this->animationStateMachine;
}

Actor *Actor::update() {
    Object::update();
    if (this->isEnableAnimationStateMachine) {
        this->animationStateMachine.updateAll(this->getPosition(), this->getRotateAngle());
    } else {
        this->mesh->setPosition(this->getPosition())->setRotateAngle(this->getRotateAngle());
    }
    return this;
}

void Actor::draw() {
    if (this->getIsVisible()) {
        if (this->isEnableAnimationStateMachine) {
            if (this->animationStateMachine.getCurrentAnimation()) {
                this->animationStateMachine.getCurrentAnimation()->getMesh().show();
            }
        } else {
            this->mesh->show();
        }
    }
    if (this->isCheckCollision)
        this->collision->check();
}

Actor *Actor::setScale(double s) {
    Object::setScale(s);
    this->animationStateMachine.setScale(s);
    return this;
}

Actor::Actor() : Object() {
    animationStateMachine = AnimationStateMachine();
    this->isEnableAnimationStateMachine = false;
}

Actor *Actor::enableAnimationStateMachine() {
    this->isEnableAnimationStateMachine = true;
    return this;
}

Actor *Actor::disableAnimationStateMachine() {
    this->isEnableAnimationStateMachine = true;
    return this;
}





//
// Created by 22190 on 2024/12/5.
//

#include <cassert>
#include "bai2d_object.h"

Mesh *Mesh::setRotateAngle(int angle) {
    this->rotateAngle = angle;
    return this;
}

Mesh::Mesh(const POINT &position) {
    this->rotateAngle = 0;
    this->position = position;
}

Mesh *Mesh::setPosition(const POINT &point) {
    this->position = point;
    return this;
}

Mesh *Mesh::setPosition(int x, int y) {
    this->position.x = x;
    this->position.y = y;
    return this;
}

Mesh::~Mesh() = default;

[[maybe_unused]] Mesh::Mesh(const Mesh &mesh) {
    this->position = mesh.position;
    this->rotateAngle = mesh.rotateAngle;
}

POINT Mesh::getPosition() const {
    return this->position;
}

int Mesh::getRotateAngle() const {
    return this->rotateAngle;
}

Mesh::Mesh() : Mesh(POINT{}) {
}

void Mesh::show() {
    this->beforeDraw();
    this->draw();
    this->afterDraw();
}

void RectMesh::draw() {
    // 计算所有旋转后的上下左右点
    POINT pts[5];
    // position(中心点) rotateAngle width height计算新的坐标点
    pts[0] = getRotatePoint(this->position,
                            {this->position.x - this->width / 2, this->position.y - this->height / 2},
                            this->rotateAngle);
    pts[1] = getRotatePoint(this->position,
                            {this->position.x + this->width / 2, this->position.y - this->height / 2},
                            this->rotateAngle);
    pts[2] = getRotatePoint(this->position,
                            {this->position.x + this->width / 2, this->position.y + this->height / 2},
                            this->rotateAngle);
    pts[3] = getRotatePoint(this->position,
                            {this->position.x - this->width / 2, this->position.y + this->height / 2},
                            this->rotateAngle);
    pts[4] = pts[0];
    this->isFill() ? fillpolygon(pts, 5) : polyline(pts, 5);
}

RectMesh *RectMesh::setWidth(int w) {
    this->width = w;
    return this;
}

RectMesh *RectMesh::setHeight(int h) {
    this->height = h;
    return this;
}

int RectMesh::getHeight() const {
    return this->height;
}

int RectMesh::getWidth() const {
    return this->width;
}

RectMesh::RectMesh(const POINT &position, int width, int height) : GeometryMesh(position), width(width),
                                                                   height(height) {

}

GeometryMesh *GeometryMesh::setFill(bool b) {
    this->fill = b;
    return this;
}

bool GeometryMesh::isFill() const {
    return this->fill;
}

GeometryMesh *GeometryMesh::setColor(COLORREF c) {
    this->color = c;
    return this;
}

void GeometryMesh::beforeDraw() {
    this->tempColor = getlinecolor();
    setlinecolor(this->color);
    setfillcolor(this->color);
}

void GeometryMesh::afterDraw() {
    setlinecolor(this->tempColor);
    setfillcolor(this->tempColor);
}

CircleMesh::CircleMesh(const POINT &position, int radius) : GeometryMesh(position), radius(radius) {

}

CircleMesh *CircleMesh::setRadius(int r) {
    this->radius = r;
    return this;
}

int CircleMesh::getRadius() const {
    return this->radius;
}

void CircleMesh::draw() {
    // 圆不用旋转，直接绘图
    this->isFill() ? fillcircle(this->position.x, this->position.y, this->radius) :
    circle(this->position.x, this->position.y, this->radius);
}

RectangleCollision::RectangleCollision(const POINT &position, int width, int height) : CollisionAble(
        CollisionCategory::RECTANGLE, position, new RectMesh(position, width, height)) {
    this->width = width;
    this->height = height;
}

POINT RectangleCollision::getLeftTop() const {
    return {this->position.x - this->width / 2, this->position.y - this->height / 2};
}

POINT RectangleCollision::getLeftBottom() const {
    return {this->position.x - this->width / 2, this->position.y + this->height / 2};
}

POINT RectangleCollision::getRightTop() const {
    return {this->position.x + this->width / 2, this->position.y - this->height / 2};
}

POINT RectangleCollision::getRightBottom() const {
    return {this->position.x + this->width / 2, this->position.y + this->height / 2};
}

bool RectangleCollision::isCollision(CollisionAble *c) {
    return CollisionLogicHandler::isCollisionFirstRect(*this, *c);
}

RectangleCollision *RectangleCollision::setWidth(int w) {
    if (w <= 0) return this;
    this->width = w;
    dynamic_cast<RectMesh *>((this->mesh))->setWidth(w);
    return this;
}

RectangleCollision *RectangleCollision::setHeight(int h) {
    if (h <= 0) return this;
    this->height = h;
    dynamic_cast<RectMesh *>((this->mesh))->setHeight(h);
    return this;
}

int RectangleCollision::getWidth() const {
    return this->width;
}

int RectangleCollision::getHeight() const {
    return this->height;
}

void RectangleCollision::check() {
    COLORREF memory = getlinecolor();
    setlinecolor(RED);
    CollisionAble::check();
    setlinecolor(memory);
}

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
    if (!this->getIsVisible()) return;
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
    this->collision = c;
    return this;
}

CollisionAble &Object::getCollision() const {
    return *this->collision;
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

CollisionCategory CollisionAble::getCategory() const {
    return this->category;
}

CollisionAble::CollisionAble(CollisionCategory category, const POINT &position, Mesh *m) : category(category) {
    if (m == nullptr) return;
    this->mesh = m;
    this->mesh->setPosition(position);
}

POINT CollisionAble::getPosition() const {
    return this->position;
}

CollisionAble *CollisionAble::setPosition(const POINT &point) {
    this->position.x = point.x;
    this->position.y = point.y;
    this->mesh->setPosition(this->position);
    return this;
}

void CollisionAble::check() {
    if (this->mesh == nullptr) return;
    this->mesh->show();
}

CollisionAble::CollisionAble(CollisionCategory category, const POINT &position) : CollisionAble(category, position,
                                                                                                nullptr) {
}

void CollisionAble::setMesh(Mesh *m) {
    if (m == nullptr) return;
    this->mesh = m;
}

Mesh &CollisionAble::getMesh() const {
    return *mesh;
}

CollisionAble::~CollisionAble() = default;


bool CollisionLogicHandler::isCollisionFirstRect(const RectangleCollision &c1, const CollisionAble &c2) {
    switch (c2.getCategory()) {
        case CollisionCategory::CIRCLE:
            return false;
        case CollisionCategory::RECTANGLE:
            return isCollision(c1, dynamic_cast<const RectangleCollision &>(c2));
        default:
            return false;
    }
}

bool CollisionLogicHandler::isCollision(const RectangleCollision &c1, const RectangleCollision &c2) {
    // 判断两个方形是否相交,通过矩形左上角和长度宽高判断
    POINT leftTop1 = c1.getLeftTop();
    POINT leftTop2 = c2.getLeftTop();
    return leftTop1.x < leftTop2.x + c2.getWidth() && leftTop1.x + c1.getWidth() > leftTop2.x &&
           leftTop1.y < leftTop2.y + c2.getHeight() && leftTop1.y + c1.getHeight() > leftTop2.y;
}

const int BaseObject::DEFAULT_WIDTH = 50;
const int BaseObject::DEFAULT_HEIGHT = 50;

BaseObject::BaseObject() : BaseObject(POINT{0, 0}) {

}

int MoveableObject::ACTION_FRAME_INTERVAL = 30;

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

MoveableObject *MoveableObject::update() {
    ObjectWithState::update();
    if (TimeUtils::getNowMilliseconds() - lastMoveTime > 1000 / ACTION_FRAME_INTERVAL) {
        lastMoveTime = TimeUtils::getNowMilliseconds();
        this->updateAction();
    }
    return this;
}

void BaseObject::updateRealPosition() {
    BaseObject *obj = getParentObj();
    POINT realPosition = POINT();
    if (obj != nullptr) {
        obj->updateRealPosition();
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

void ObjectManager::addObject(BaseObject *object) {
    if (isContains(object))return;
    this->objects.push_back(object);
}

bool ObjectManager::isEmpty() const {
    return this->objects.empty();
}

void ObjectManager::removeObject(int index) {
    if (index < 0 || index >= this->objects.size()) return;
    auto iterator = this->objects.begin();
    for (int i = 0; i < index; i++) iterator++;
    this->objects.erase(iterator);
}

void ObjectManager::removeObject(BaseObject *object) {
    if (object != nullptr)
        for (auto iterator = this->objects.begin(); iterator != this->objects.end(); iterator++) {
            if (*iterator == object) {
                this->objects.erase(iterator);
                return;
            }
        }
}

void ObjectManager::removeAll() {
    this->objects.clear();
}

bool ObjectManager::isContains(BaseObject *object) const {
    if (object != nullptr)
        for (auto iterator: this->objects) {
            if (iterator == object) return true;
        }
    return false;
}

ObjectManager::ObjectManager() {
    this->objects = std::vector<BaseObject *>();
}

ObjectManager::~ObjectManager() {
    this->removeAll();
}

void ObjectManager::showAll() {
    auto objs = getAllInOneVectorWithSortAndUnique();
    for (auto iterator: objs) {
        iterator->update()->draw();
    }
}

std::vector<BaseObject *> ObjectManager::getAllInOneVector() {
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

std::vector<BaseObject *> ObjectManager::getAllInOneVectorWithSortAndUnique() {
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
        }
        this->attachObjs->addObject(obj);
        obj->parentObj = this;
    }
    return this;
}

void BaseObject::detach(BaseObject *object) {
    if (object == nullptr) return;
    this->attachObjs->removeObject(object);
    object->parentObj = nullptr;
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
    return width;
}

int BaseObject::getHeight() const {
    return height;
}

BaseObject::BaseObject(const POINT &position, int w, int h) : visible(true), rotateAngle(0), renderingIndex(0) {
    assert(w > 0 && h > 0);
    this->attachObjs = new ObjectManager();
    this->parentObj = nullptr;
    parentOffset = POINT{0, 0};
    this->position = position;
    this->anchor = ObjectAnchor::CENTER;
    this->width = w;
    this->height = h;
}

POINT BaseObject::getAnchorOffset() {
    POINT offset{0, 0};
    switch (anchor) {
        case ObjectAnchor::LEFT_TOP:
            offset.x = -width / 2;
            offset.y = -height / 2;
            break;
        case ObjectAnchor::LEFT_BOTTOM:
            offset.x = -width / 2;
            offset.y = height / 2;
            break;
        case ObjectAnchor::RIGHT_TOP:
            offset.x = width / 2;
            offset.y = -height / 2;
            break;
        case ObjectAnchor::RIGHT_BOTTOM:
            offset.x = width / 2;
            offset.y = height / 2;
            break;
        case ObjectAnchor::CENTER:
            break;
    }
    return offset;
}

void MoveableObject::updateAction() {
    // 根据移动向量，speed 通过三角函数 重新计算移动距离，然后将位置加上距离
    double angle = atan2(this->moveVector.y, this->moveVector.x);
    double x = this->moveVector.x != 0 ? speed * cos(angle) : 0;
    double y = this->moveVector.y != 0 ? speed * sin(angle) : 0;
    this->setPosition(this->getPosition().x + static_cast<int>(std::round(x)),
                      this->getPosition().y + static_cast<int>(std::round(y)));
}

void MoveableObject::setSpeed(int s) {
    if (s < 0) s = 0;
    this->speed = s;
}

int MoveableObject::getSpeed() const {
    return this->speed;
}

bai::slot MoveableObject::moveUp(const SlotArgs &args) {
    this->moveVector.y = -1;
    this->afterMove();
}

bai::slot MoveableObject::moveDown(const SlotArgs &args) {
    this->moveVector.y = 1;
    this->afterMove();
}

bai::slot MoveableObject::moveLeft(const SlotArgs &args) {
    this->moveVector.x = -1;
    this->afterMove();
}

bai::slot MoveableObject::moveRight(const SlotArgs &args) {
    this->moveVector.x = 1;
    this->afterMove();
}

bai::slot MoveableObject::resetVectorX() {
    this->moveVector.x = 0;
    if (this->moveVector.x == 0 && this->moveVector.y == 0)
        this->moveStop();
}

bai::slot MoveableObject::resetVectorY() {
    this->moveVector.y = 0;
    if (this->moveVector.x == 0 && this->moveVector.y == 0)
        this->moveStop();
}

bool Object::isCollision(Object &object) const {
    return this->collision->isCollision(&object.getCollision());
}

Object::Object() : Object(POINT{0, 0}) {

}

Object::Object(const POINT &position, int w, int h, RectMesh *mesh, CollisionAble *collision) : BaseObject(position) {
    assert(w > 0 && h > 0);
    this->setWidth(w);
    this->setHeight(h);
    if (mesh == nullptr) {
        this->mesh = new RectMesh(position, this->getWidth(), this->getHeight());
    } else {
        this->mesh = mesh;
    }
    if (collision == nullptr)
        this->collision = new RectangleCollision(this->getPosition(), this->getWidth(), this->getHeight());
    else
        this->collision = collision;
}

Object *Object::update() {
    BaseObject::update();
    this->collision->setPosition(getOffsetPoint(this->getPosition(), this->getAnchorOffset()));
    this->mesh->setPosition(getOffsetPoint(this->getPosition(), this->getAnchorOffset()));
    this->mesh->setRotateAngle(this->getRotateAngle());
    return this;
}

Object *Object::setIsCheckCollision(bool b) {
    this->isCheckCollision = b;
    return this;
}

void Weight::draw() {

}

ImageMesh::ImageMesh(const std::string &assetName, int width, int height) : BaseImageMesh(width, height) {
    this->asset = &GlobalAssetManager::getInstance().getImageAsset(assetName);
}

BaseImageMesh::BaseImageMesh(int width, int height) : Mesh(POINT{}) {
    assert(width > 0 && height > 0);
    this->width = width;
    this->height = height;
}

BaseImageMesh *BaseImageMesh::setWidth(int w) {
    assert(w > 0);
    this->width = w;
    return this;
}

BaseImageMesh *BaseImageMesh::setHeight(int h) {
    assert(h > 0);
    this->height = h;
    return this;
}

int BaseImageMesh::getWidth() const {
    return this->width;
}

int BaseImageMesh::getHeight() const {
    return this->height;
}

void ImageMesh::draw() {
    if (this->asset == nullptr) return;
    IMAGE tempImg;
    rotateimage(&tempImg, this->asset->getImage(), this->getRotateAngle());
    EasyXUtils::putImageAlpha(position, getWidth(), getHeight(), tempImg, isMirrorX(), isMirrorY());
}

MultiImageMesh::MultiImageMesh(int width, int height) : BaseImageMesh(width, height) {
    assert(width > 0 && height > 0);
    this->assetManager = AssetManager{};
    currentIndex = 0;
    isLoop = false;
    playInterval = 0;
    lastTime = 0;
}

MultiImageMesh *MultiImageMesh::addAsset(const std::string &assetName) {
    this->assetManager.addAsset(&GlobalAssetManager::getInstance().getImageAsset(assetName));
    return this;
}

MultiImageMesh *MultiImageMesh::setPlayInterval(int i) {
    assert(i >= 0);
    this->playInterval = i;
    return this;
}

MultiImageMesh *MultiImageMesh::setIsLoop(bool loop) {
    this->isLoop = loop;
    return this;
}

void MultiImageMesh::draw() {
    if (this->assetManager.isEmpty()) return;
    IMAGE tempImg;
    rotateimage(&tempImg, this->assetManager.getAsset(this->currentIndex)->castTo<ImageAsset>()->getImage(),
                this->getRotateAngle());
    EasyXUtils::putImageAlpha(position, getWidth(), getHeight(), tempImg, isMirrorX(), isMirrorY());
}

MultiImageMesh *MultiImageMesh::resetCurrentIndex() {
    this->currentIndex = 0;
    return this;
}

void MultiImageMesh::beforeDraw() {
    if (this->assetManager.isEmpty()) return;
    if (TimeUtils::getNowMilliseconds() - lastTime >= playInterval) {
        lastTime = TimeUtils::getNowMilliseconds();
        if (this->currentIndex >= this->assetManager.size() - 1 && !this->isLoop) {
            this->currentIndex = int(this->assetManager.size() - 1);
            return;
        }
        this->currentIndex++;
        if (this->currentIndex >= this->assetManager.size())
            this->resetCurrentIndex();
    }
}

MultiImageMesh::~MultiImageMesh() = default;


ObjectStateAnimation::ObjectStateAnimation(Mesh *mesh) {
    assert(mesh != nullptr);
    this->mesh = mesh;
}

void ObjectStateAnimation::play() {
    this->mesh->show();
}

Mesh &ObjectStateAnimation::getMesh() {
    return *this->mesh;
}

void AnimationStateMachine::addAnimation(byte state, ObjectStateAnimation *animation) {
    assert(animation != nullptr);
    this->animations[state] = animation;
}

void AnimationStateMachine::removeAnimation(byte state) {
    this->animations.erase(state);
}

AnimationStateMachine::~AnimationStateMachine() {
    for (auto &[state, animation]: animations) {
        delete animation;
    }
}

AnimationStateMachine *AnimationStateMachine::setCurrentState(byte stateEnum) {
    this->currentState = stateEnum;
    if (typeid(MultiImageMesh) == typeid(this->getCurrentAnimation()->getMesh())) {
        this->getCurrentAnimation()->getMesh().castTo<MultiImageMesh>()->resetCurrentIndex();
    }
    return this;
}

void AnimationStateMachine::play() {
    if (this->animations[this->currentState] != nullptr)
        this->animations[this->currentState]->play();
}

ObjectStateAnimation *AnimationStateMachine::getCurrentAnimation() {
    ObjectStateAnimation *&pAnimation = this->animations[this->currentState];
    if (pAnimation == nullptr) return nullptr;
    pAnimation->getMesh().setMirrorX(this->getTowardsX())->setMirrorY(this->getTowardsY());
    return pAnimation;
}

AnimationStateMachine::AnimationStateMachine() : MirrorAble() {
    this->animations = std::map<byte, ObjectStateAnimation *>();
    this->currentState = 0;
}

ObjectWithState *ObjectWithState::update() {
    Object::update();
    if (this->animationStateMachine.getCurrentAnimation()) {
        this->animationStateMachine.getCurrentAnimation()->getMesh().setPosition(
                getOffsetPoint(this->getPosition(), this->getAnchorOffset()));
        this->animationStateMachine.getCurrentAnimation()->getMesh().setRotateAngle(this->getRotateAngle());
    }
    return this;
}

void ObjectWithState::draw() {
    if (!this->getIsVisible()) return;
    if (this->animationStateMachine.getCurrentAnimation()) {
        this->animationStateMachine.getCurrentAnimation()->getMesh().show();
    }
    if (this->isCheckCollision)
        this->collision->check();
}

AnimationStateMachine &ObjectWithState::getAnimationStateMachine() {
    return this->animationStateMachine;
}

MirrorAble *MirrorAble::setMirrorX(MirrorAble::ToWardsX x) {
    this->xTowards = x;
    return this;
}

MirrorAble *MirrorAble::setMirrorY(MirrorAble::ToWardsY y) {
    this->yTowards = y;
    return this;
}

bool MirrorAble::isMirrorX() const {
    if (this->xTowards == ToWardsX::RIGHT)
        return false;
    return true;
}

bool MirrorAble::isMirrorY() const {
    if (this->yTowards == ToWardsY::DOWN)
        return false;
    return true;
}

MirrorAble::ToWardsX MirrorAble::getTowardsX() const {
    return this->xTowards;
}

MirrorAble::ToWardsY MirrorAble::getTowardsY() const {
    return this->yTowards;
}

MirrorAble::MirrorAble() : xTowards(ToWardsX::RIGHT), yTowards(ToWardsY::DOWN){}



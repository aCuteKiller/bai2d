//
// Created by 22190 on 2024/12/8.
//

#include <cassert>
#include "bai2d_mesh.h"


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
    pts[0] = EasyXUtils::getRotatePoint(this->position,
                                        {this->position.x - this->width / 2, this->position.y - this->height / 2},
                                        this->rotateAngle);
    pts[1] = EasyXUtils::getRotatePoint(this->position,
                                        {this->position.x + this->width / 2, this->position.y - this->height / 2},
                                        this->rotateAngle);
    pts[2] = EasyXUtils::getRotatePoint(this->position,
                                        {this->position.x + this->width / 2, this->position.y + this->height / 2},
                                        this->rotateAngle);
    pts[3] = EasyXUtils::getRotatePoint(this->position,
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

ObjectStateAnimation::ObjectStateAnimation(Mesh *mesh) {
    assert(mesh != nullptr);
    this->mesh = mesh;
}

void ObjectStateAnimation::forwardPlay() {
    try {
        auto *pReversePlayAble = dynamic_cast<ReversePlayAble *>(&this->getMesh());
        pReversePlayAble->setIsReverse(false);
    } catch (const std::bad_cast &e) {}
}

Mesh &ObjectStateAnimation::getMesh() {
    return *this->mesh;
}

void ObjectStateAnimation::reversePlay() {
    try {
        auto *pReversePlayAble = dynamic_cast<ReversePlayAble *>(&this->getMesh());
        pReversePlayAble->setIsReverse(true);
    } catch (const std::bad_cast &e) {}
}

void ObjectStateAnimation::play() {
    if (this->isReverse()) {
        this->reversePlay();
    } else {
        this->forwardPlay();
    }
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
    if (typeid(MultiImageMesh) == typeid(this->getCurrentAnimation()->getMesh()) && lastState != currentState) {
        this->getCurrentAnimation()->getMesh().castTo<MultiImageMesh>()->resetCurrentIndex();
        lastState = currentState;
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

AnimationStateMachine *AnimationStateMachine::setAllPosition(const POINT &point) {
    for (auto &[state, animation]: this->animations) {
        animation->getMesh().setPosition(point);
    }
    return this;
}

AnimationStateMachine *AnimationStateMachine::setAllRotateAngle(int angle) {
    for (auto &[state, animation]: this->animations) {
        animation->getMesh().setRotateAngle(angle);
    }
    return this;
}

AnimationStateMachine *AnimationStateMachine::updateAll(const POINT &point, int angle) {
    for (auto &[state, animation]: this->animations) {
        animation->getMesh().setPosition(point)->setRotateAngle(angle);
    }
    return this;
}

AnimationStateMachine *AnimationStateMachine::resetPlayPace() {
    if (typeid(MultiImageMesh) == typeid(this->getCurrentAnimation()->getMesh())) {
        this->getCurrentAnimation()->getMesh().castTo<MultiImageMesh>()->resetCurrentIndex();
    }
    return this;
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

MirrorAble::MirrorAble() : xTowards(ToWardsX::RIGHT), yTowards(ToWardsY::DOWN) {}

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

MultiImageMesh::MultiImageMesh(int width, int height) : BaseImageMesh(width, height), ReversePlayAble(),
                                                        currentIndex(0), playInterval(0), isLoop(false), lastTime(0) {
    assert(width > 0 && height > 0);
    this->assetManager = AssetManager{};
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
    if (this->isReverse())
        this->backwardPlay();
    else
        this->forwardPlay();
}

void MultiImageMesh::forwardPlay() {
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

void MultiImageMesh::backwardPlay() {
    if (this->assetManager.isEmpty()) return;
    if (TimeUtils::getNowMilliseconds() - lastTime >= playInterval) {
        lastTime = TimeUtils::getNowMilliseconds();
        if (this->currentIndex <= 0 && !this->isLoop) {
            this->currentIndex = 0;
            return;
        }
        this->currentIndex--;
        if (this->currentIndex < 0)
            this->resetCurrentIndex();
    }
}

MultiImageMesh::~MultiImageMesh() = default;

ReversePlayAble::ReversePlayAble() : isReversePlay(false) {

}

ReversePlayAble *ReversePlayAble::setIsReverse(bool reverse) {
    this->isReversePlay = reverse;
    return this;
}

bool ReversePlayAble::isReverse() const {
    return this->isReversePlay;
}
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
    this->scale = 1;
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

Mesh *Mesh::setScale(double s) {
    assert(s > 0);
    this->scale = s;
    return this;
}

Mesh *Mesh::offsetPosition(int x, int y) {
    this->position.x += x;
    this->position.y += y;
    return this;
}

Mesh *Mesh::offsetPosition(POINT offset) {
    this->position.x += offset.x;
    this->position.y += offset.y;
    return this;
}

void RectMesh::draw() {
    // 计算所有旋转后的上下左右点
    POINT pts[5];
    // position(中心点) rotateAngle width height计算新的坐标点
    pts[0] = EasyXUtils::getRotatePoint(this->position,
                                        {this->position.x - this->getWidth() / 2,
                                         this->position.y - this->getHeight() / 2},
                                        this->rotateAngle);
    pts[1] = EasyXUtils::getRotatePoint(this->position,
                                        {this->position.x + this->getWidth() / 2,
                                         this->position.y - this->getHeight() / 2},
                                        this->rotateAngle);
    pts[2] = EasyXUtils::getRotatePoint(this->position,
                                        {this->position.x + this->getWidth() / 2,
                                         this->position.y + this->getHeight() / 2},
                                        this->rotateAngle);
    pts[3] = EasyXUtils::getRotatePoint(this->position,
                                        {this->position.x - this->getWidth() / 2,
                                         this->position.y + this->getHeight() / 2},
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
    return int(this->height * this->scale);
}

int RectMesh::getWidth() const {
    return int(this->width * this->scale);
}

RectMesh::RectMesh(const POINT &position, int width, int height) : GeometryMesh(position), width(width),
                                                                   height(height) {

}

POINT RectMesh::getLeftTop() const {
    return EasyXUtils::getRotatePoint(this->position,
                                      {this->position.x - this->getWidth() / 2,
                                       this->position.y - this->getHeight() / 2},
                                      this->rotateAngle);
}

POINT RectMesh::getRightBottom() const {
    return EasyXUtils::getRotatePoint(this->position,
                                      {this->position.x + this->getWidth() / 2,
                                       this->position.y + this->getHeight() / 2},
                                      this->rotateAngle);
}

POINT RectMesh::getLeftBottom() const {
    return EasyXUtils::getRotatePoint(this->position,
                                      {this->position.x - this->getWidth() / 2,
                                       this->position.y + this->getHeight() / 2},
                                      this->rotateAngle);
}

POINT RectMesh::getRightTop() const {
    return EasyXUtils::getRotatePoint(this->position,
                                      {this->position.x + this->getWidth() / 2,
                                       this->position.y - this->getHeight() / 2},
                                      this->rotateAngle);
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
    getlinestyle(&this->lastLineStyle);
    setlinestyle(this->lineStyle, this->lineThickness);
    setfillcolor(this->color);
    setlinecolor(this->color);
}

void GeometryMesh::afterDraw() {
    setfillcolor(this->tempColor);
    setlinecolor(this->tempColor);
    setlinestyle(&this->lastLineStyle);
}

GeometryMesh::GeometryMesh(const POINT &position) : fill(false), color(WHITE), tempColor(color), Mesh(position) {
    this->lineStyle = PS_SOLID;
    this->lineThickness = 1;
    this->lastLineStyle = {};
}

GeometryMesh::GeometryMesh() : GeometryMesh(POINT{}) {}

GeometryMesh *GeometryMesh::setLineStyle(int style) {
    this->lineStyle = style;
    return this;
}

GeometryMesh *GeometryMesh::setLineThickness(int thickness) {
    assert(thickness > 0);
    this->lineThickness = thickness;
    return this;
}

CircleMesh::CircleMesh(const POINT &position, int radius) : GeometryMesh(position), radius(radius) {

}

CircleMesh *CircleMesh::setRadius(int r) {
    this->radius = r;
    return this;
}

int CircleMesh::getRadius() const {
    return int(this->radius * this->scale);
}

void CircleMesh::draw() {
    // 圆不用旋转，直接绘图
    this->isFill() ? fillcircle(this->position.x, this->position.y, this->getRadius()) :
    circle(this->position.x, this->position.y, this->getRadius());
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

AnimationStateMachine *AnimationStateMachine::setScale(double s) {
    for (auto &[state, animation]: this->animations) {
        animation->getMesh().setScale(s);
    }
    return this;
}

AnimationStateMachine *AnimationStateMachine::offsetAllPosition(const POINT &offset) {
    for (auto &[state, animation]: this->animations) {
        animation->getMesh().offsetPosition(offset);
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
    this->lastScale = 1;
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
    return int(this->width * scale);
}

int BaseImageMesh::getHeight() const {
    return int(this->height * scale);
}

void ImageMesh::draw() {
    if (this->asset == nullptr) return;
    IMAGE tempImg;
    rotateimage(&tempImg, this->asset->getImage(), this->getRotateAngle());
    EasyXUtils::putImageAlpha(position, getWidth(), getHeight(), tempImg, isMirrorX(), isMirrorY());
}

//ImageMesh *ImageMesh::setScale(int s) {
//    Mesh::setScale(s);
//    if (s != this->lastScale) {
//        this->asset->resizeImage(this->getWidth(), this->getHeight());
//        this->lastScale = s;
//    }
//    return this;
//}

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
//
//MultiImageMesh *MultiImageMesh::setScale(int s) {
//    Mesh::setScale(s);
//    if (s != this->lastScale) {
//        for (Asset *asset: this->assetManager) {
//            dynamic_cast<ImageAsset *>(asset)->resizeImage(this->getWidth(), getHeight());
//        }
//        this->lastScale = s;
//    }
//    return this;
//}

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

CameraDefaultMesh::CameraDefaultMesh(int width, int height) : RectMesh(width, height) {
    this->innerMesh = RectMesh(width / 3, height / 3);
    setColor(RGB(208, 16, 76))->setLineStyle(PS_DASH)->setLineThickness(2)->setScale(.98);
}

void CameraDefaultMesh::draw() {
    RectMesh::draw();
    this->innerMesh.setPosition(this->getPosition());
    line(getLeftTop().x, getLeftTop().y, this->innerMesh.getLeftTop().x, this->innerMesh.getLeftTop().y);
    line(getLeftBottom().x, getLeftBottom().y, this->innerMesh.getLeftBottom().x,
         this->innerMesh.getLeftBottom().y);
    line(getRightTop().x, getRightTop().y, this->innerMesh.getRightTop().x, this->innerMesh.getRightTop().y);
    line(getRightBottom().x, getRightBottom().y, this->innerMesh.getRightBottom().x,
         this->innerMesh.getRightBottom().y);
    this->innerMesh.draw();
}

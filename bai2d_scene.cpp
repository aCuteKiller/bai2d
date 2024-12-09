//
// Created by 22190 on 2024/12/9.
//

#include <cassert>
#include "bai2d_scene.h"

Camera::Camera(int w, int h) : Object(w, h) {
    assert(w > 0 && h > 0);
    this->collision = new RectangleCollision(w, h);
    this->getMesh().castTo<GeometryMesh>()->setColor(BLUE)->setLineStyle(PS_DASH)->setLineThickness(2)->setScale(.98);
    this->renderSelf = true;
}

void Camera::render(BaseObjectManager &objectManager) {
    resetAspectRatio();
    // 获取在相机视口内的物体
    std::vector<Object *> toRenderObjs = getObjectsWhichInCameraView(objectManager);
    // 将物体mesh和碰撞mesh的坐标偏移到屏幕上 (0,0)为起点，（getwidth,getheight)为终点
    // 获取相机位置与原点的偏移量
    POINT offset = this->getOriginOffset();
    offsetObjectMesh(&toRenderObjs, offset);
    for (auto object: toRenderObjs) {
        object->draw();
    }
    if (this->getParentObj())
        this->getMesh().offsetPosition(offset);
    if (this->renderSelf) {
        this->draw();
    }
}

void Camera::resetAspectRatio() const {
// 首先根据depth(景深) getWidth()窗口宽度,getHeight()窗口高度,相机宽度，相机高度，计算setaspectratio(float *pxasp,float *pyasp)的缩放比;
    float pxasp = float(getwidth()) / float(this->getWidth());
    float pyasp = float(getheight()) / float(this->getHeight());
    setaspectratio(pxasp, pyasp);
}

std::vector<Object *> Camera::getObjectsWhichInCameraView(BaseObjectManager &objectManager) {
    auto objects = objectManager.getAllInOneVectorWithSortAndUnique();
    std::vector<Object *> toRenderObjs;
    for (auto object: objects) {
        if (object == this)continue;
        Object *obj = object->castTo<Object>();
        if (this->getCollision()->isCollision(obj->getCollision())) {
            toRenderObjs.push_back(obj);
        }
    }
    return toRenderObjs;
}

void Camera::offsetObjectMesh(std::vector<Object *> *objects, POINT offset) {
    for (auto object: *objects) {
        object->getMesh().offsetPosition(offset);
        object->getCollision()->getMesh().offsetPosition(offset);
        try {
            Actor *actor = object->castTo<Actor>();
            if (actor != nullptr) {
                actor->getAnimationStateMachine().offsetAllPosition(offset);
            }
        } catch (std::exception &e) {}
    }
}

Camera *Camera::setRenderSelf(bool b) {
    this->renderSelf = b;
    return this;
}

POINT Camera::getOriginOffset() {
    return POINT{0 - (this->getPosition().x - this->getWidth() / 2),
                 0 - (this->getPosition().y - this->getHeight() / 2)};
}

Camera *Camera::moveToTargetPosition(POINT point) {
    if (!this->getParentObj()) {
        POINT offset = getOriginOffset();
        offset.x = -offset.x;
        offset.y = -offset.y;
        const POINT &nPosition = EasyXUtils::getOffsetPoint(point, offset);
        setPosition(nPosition);
    }
    return this;
}

Camera *Camera::offsetRefViewCenter(POINT &point, int minSpeed, int maxSpeed, int step) {
    assert(step <= 500 && step >= 1 && maxSpeed >= minSpeed && minSpeed >= 0 && maxSpeed <= 20);
    if (!this->getParentObj()) {
        // 根据鼠标远近加减速
        const POINT &center = getViewCenter();
        auto dx = float(point.x - center.x);
        auto dy = float(point.y - center.y);
        float length = std::sqrt(dx * dx + dy * dy);
        float speed = std::min(std::max(length / (float) step, (float) minSpeed), (float) maxSpeed);
        POINT towards{point.x - center.x, point.y - center.y};
        double angle = atan2(towards.y, towards.x);
        int x = towards.x != 0 ? static_cast<int>(round(speed * cos(angle))) : 0;
        int y = towards.y != 0 ? static_cast<int>(round(speed * sin(angle))) : 0;
        offsetPosition(x, y);
    }
    return this;
}

POINT Camera::getViewCenter() {
    auto point = POINT(getPosition());
    return EasyXUtils::getOffsetPoint(point, getOriginOffset());
}

Scene::Scene(int w, int h) {
    this->camera = new Camera(w, h);
}

void Scene::render() {
    this->update();
    this->camera->render(this->objectManager);
}

Scene *Scene::addObject(Object *object) {
    this->objectManager.addObject(object);
    return this;
}

Scene *Scene::removeObject(Object *object) {
    this->objectManager.removeObject(object);
    return this;
}

Scene *Scene::update() {
    this->objectManager.updateAll();
    return this;
}

Scene::~Scene() {
    const std::vector<BaseObject *> &vector = this->objectManager.getAllInOneVectorWithSortAndUnique();
    for (auto object: vector) {
        delete object;
    }
    delete this->camera;
}

Camera &Scene::getCamera() {
    return *this->camera;
}

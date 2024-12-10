//
// Created by 22190 on 2024/12/8.
//
#include <cassert>
#include "bai2d_collision.h"

RectangleCollision::RectangleCollision(int width, int height) : RectangleCollision(POINT{}, width, height) {}


RectangleCollision::RectangleCollision(const POINT &position, int width, int height) : CollisionAble(
        CollisionCategory::RECTANGLE, position, new RectMesh(position, width, height)) {
    this->width = width;
    this->height = height;
    auto *pMesh = new RectMesh(position, this->getWidth(), this->getHeight());
    pMesh->setColor(RED);
    this->mesh = pMesh;
    pMesh = nullptr;
}

POINT RectangleCollision::getLeftTop() const {
    return {this->position.x - this->getWidth() / 2, this->position.y - this->getHeight() / 2};
}

POINT RectangleCollision::getLeftBottom() const {
    return {this->position.x - this->getWidth() / 2, this->position.y + this->getHeight() / 2};
}

POINT RectangleCollision::getRightTop() const {
    return {this->position.x + this->getWidth() / 2, this->position.y - this->getHeight() / 2};
}

POINT RectangleCollision::getRightBottom() const {
    return {this->position.x + this->getWidth() / 2, this->position.y + this->getHeight() / 2};
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
    return int(this->width * this->getScale());
}

int RectangleCollision::getHeight() const {
    return int(this->height * this->getScale());
}

void RectangleCollision::check() {
    COLORREF memory = getlinecolor();
    setlinecolor(RED);
    CollisionAble::check();
    setlinecolor(memory);
}

bool RectangleCollision::isAbove(CollisionAble *c) {
    return CollisionLogicHandler::isAboveFirstRect(*this, *c);
}

bool RectangleCollision::isBelow(CollisionAble *c) {
    return CollisionLogicHandler::isBelowFirstRect(*this, *c);
}

CollisionCategory CollisionAble::getCategory() const {
    return this->category;
}

CollisionAble::CollisionAble(CollisionCategory category, const POINT &position, Mesh *m) : category(category) {
    if (m == nullptr) return;
    this->mesh = m;
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

CollisionAble *CollisionAble::setScale(double s) {
    assert(s > 0);
    this->scale = s;
    this->mesh->setScale(s);
    return this;
}

double CollisionAble::getScale() const {
    return this->scale;
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

bool CollisionLogicHandler::isAboveFirstRect(const RectangleCollision &c1, const CollisionAble &c2) {
    switch (c2.getCategory()) {
        case CollisionCategory::CIRCLE:
            return false;
        case CollisionCategory::RECTANGLE:
            return isAbove(c1, dynamic_cast<const RectangleCollision &>(c2));
        default:
            return false;
    }
}

bool CollisionLogicHandler::isBelowFirstRect(const RectangleCollision &c1, const CollisionAble &c2) {
    switch (c2.getCategory()) {
        case CollisionCategory::CIRCLE:
            return false;
        case CollisionCategory::RECTANGLE:
            return isBelow(c1, dynamic_cast<const RectangleCollision &>(c2));
        default:
            return false;
    }
}

bool CollisionLogicHandler::isAbove(const RectangleCollision &c1, const RectangleCollision &c2) {
    if (isCollision(c1, c2)) {
        //判断c1是否在c2的上方 c1以下边线为基准，c2以上边线为基准
        return c1.getLeftBottom().y <= c2.getLeftTop().y &&
               c1.getLeftBottom().x <= c2.getRightBottom().x &&
               c1.getRightBottom().x >= c2.getLeftTop().x;
    }
    return false;
}

bool CollisionLogicHandler::isBelow(const RectangleCollision &c1, const RectangleCollision &c2) {
    return c1.getLeftTop().y >= c2.getLeftBottom().y &&
           c1.getLeftBottom().x <= c2.getRightBottom().x &&
           c1.getRightBottom().x >= c2.getLeftTop().x;
    return false;
}

#include "bai2d_collision.h"

//
// Created by 22190 on 2024/12/5.
//

#ifndef BAI2D_OBJECT_H
#define BAI2D_OBJECT_H

#pragma once

#include<string>
#include "bai2d_constants.h"
#include "bai2d_utils.h"
#include "graphics.h"
#include "bai2d_event_base.h"
#include <cmath>
#include <set>
#include <algorithm>
#include "bai2d_asset.h"
#include "bai2d_mesh.h"
#include "bai2d_collision.h"

class BaseObject;

class BaseObjectManager;

enum ObjectAnchor {
    LEFT_TOP,
    LEFT_BOTTOM,
    RIGHT_TOP,
    RIGHT_BOTTOM,
    CENTER,
};

class BaseObject : public EventAble {
private:
    bool visible;
    POINT position{};
    int renderingIndex{};
    int rotateAngle;
    BaseObjectManager *attachObjs;
    BaseObject *parentObj;
    POINT parentOffset{};
    ObjectAnchor anchor;
    int width;
    int height;
    double scale;
protected:
    static const int DEFAULT_WIDTH;
    static const int DEFAULT_HEIGHT;

    void updateRealPosition();

    BaseObject *setWidth(int w);

public:

    explicit BaseObject();

    explicit BaseObject(const POINT &position);

    explicit BaseObject(const POINT &position, int w, int h);

    virtual ~BaseObject();

    virtual void draw() = 0;

    BaseObject *setVisible(bool b);

    BaseObject *setRenderingIndex(int index);

    [[nodiscard]] int getRenderingIndex() const;

    BaseObject *setPosition(const POINT &point);

    BaseObject *setPosition(int x, int y);

    BaseObject *offsetPosition(int x, int y);

    virtual BaseObject *update();

    BaseObject *setRotateAngle(int angle);

    [[nodiscard]] POINT getPosition();

    [[nodiscard]] int getRotateAngle() const;

    [[nodiscard]] int getIsVisible() const;

    [[nodiscard]] bool isVisible() const;

    std::vector<BaseObject *> getAllOwnObjects();

    BaseObject *attach(BaseObject *obj);

    void detach(BaseObject *obj);

    BaseObject *getParentObj();

    void setParentOffset(int x, int y);

    [[nodiscard]] POINT getParentOffset() const;

    BaseObject *setAnchor(ObjectAnchor oa);

    [[nodiscard]] ObjectAnchor getAnchor() const;

    [[nodiscard]] int getWidth() const;

    BaseObject *setHeight(int h);

    [[nodiscard]] int getHeight() const;

    POINT getAnchorOffset();

    virtual BaseObject *setScale(double s);

    [[nodiscard]] double getScale() const;

};

class Object : public BaseObject {
private:

protected:
    bool isCheckCollision = false;
    Mesh *mesh;
    CollisionAble *collision;
public:
    explicit Object();

    ~Object() override;

    explicit Object(const POINT &position);

    explicit Object(const POINT &position, RectMesh *mesh);

    explicit Object(const POINT &position, RectMesh *mesh, CollisionAble *collision);

    explicit Object(const POINT &position, int w, int h, RectMesh *mesh, CollisionAble *collision);

    Object *update() override;

    Object *setMesh(Mesh *m);

    Object *setCollision(CollisionAble *c);

    [[nodiscard]] Mesh &getMesh() const;

    [[nodiscard]] CollisionAble *getCollision() const;

    [[nodiscard]] bool isCollision(Object *object) const;

    void draw() override;

    Object *setIsCheckCollision(bool b);

    Object *setScale(double s) override;
};

class Actor : public Object {
private:
    AnimationStateMachine animationStateMachine;
public:
    Actor *update() override;

    void draw() override;

    Actor *setScale(double s) override;

    AnimationStateMachine &getAnimationStateMachine();
};

class Pawn : public Actor {
private:
    const static int ACTION_FRAME_INTERVAL;
    const static int DEFAULT_JUMP_HEIGHT;
    const static int DEFAULT_IN_THE_AIR_TIME;
    const static int DEFAULT_JUMP_SPEED;
    POINT towards{};
    int speed;
    long long lastMoveTime;
    POINT beforeJumpPosition{};
    bool jumping;
    bool falling;
    int jumpHeight;
    int maxJumpTimes;
    int hasJumpTimes;
    int jumpSpeed;
    int realJumpHeight;
    int inTheAirTime;
    long long startInTheAir;

    void jumpOneStep();

public:
    Pawn();

    Pawn *update() override;

    virtual void updateAction();

    virtual bai::slot moveUp(const SlotArgs &args);

    virtual bai::slot moveDown(const SlotArgs &args);

    virtual bai::slot moveLeft(const SlotArgs &args);

    virtual bai::slot moveRight(const SlotArgs &args);

    virtual bai::slot resetVectorX(const SlotArgs &args);

    virtual bai::slot resetVectorY(const SlotArgs &args);

    virtual void toRight() {};

    virtual void toLeft() {};

    virtual void toUp() {};

    virtual void toDown() {};

    virtual void move() {};

    virtual void idle() {};

    virtual void startJump() {};

    virtual void jumpLoop() {};

    virtual void fallingDown() {};

    virtual void endJump() {};

    void setSpeed(int s);

    [[nodiscard]] int getSpeed() const;

    [[nodiscard]] bool isJumping() const;

    [[nodiscard]] bool isFalling() const;

    Pawn *setJumpHeight(int height);

    Pawn *setMaxJumpTimes(int times);

    Pawn *setJumpSpeed(int s);

    bai::slot jump(const SlotArgs &args);

    [[nodiscard]] POINT getTowards() const;

};

// 组件 TODO
class Weight : public BaseObject {
private:
    int width;
    int height;
public:
    void draw() override;

};

class BaseObjectManager {
private:
    std::vector<BaseObject *> objects;

public:
    BaseObjectManager();

    ~BaseObjectManager();

    [[nodiscard]] bool isEmpty() const;

    bool isContains(BaseObject *object) const;

    void addObject(BaseObject *object);

    void removeObject(BaseObject *object);

    void removeObject(int index);

    void removeAll();

    void showAll();

    // 添加 begin() 和 end() 成员函数
    std::vector<BaseObject *>::iterator begin() {
        return objects.begin();
    }

    std::vector<BaseObject *>::iterator end() {
        return objects.end();
    }

    [[nodiscard]] std::vector<BaseObject *>::const_iterator begin() const {
        return objects.begin();
    }

    [[nodiscard]] std::vector<BaseObject *>::const_iterator end() const {
        return objects.end();
    }

    std::vector<BaseObject *> getAllInOneVector();

    std::vector<BaseObject *> getAllInOneVectorWithSortAndUnique();

};


#endif / BAI2D_OBJECT_H

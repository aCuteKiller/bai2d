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
#include <algorithm>]
#include "bai2d_asset.h"

class BaseObject;

class Mesh;

static POINT getRotatePoint(POINT center, POINT point, int angle) {
    double radian = angle * PI / 180;
    double x = point.x - center.x;
    double y = point.y - center.y;
    double x1 = x * cos(radian) - y * sin(radian);
    double y1 = x * sin(radian) + y * cos(radian);
    return {(int) (x1 + center.x), (int) (y1 + center.y)};
}

static POINT getOffsetPoint(POINT origin, POINT offset) {
    return {origin.x + offset.x, origin.y + offset.y};
}

class Mesh;

class MirrorAble {

public:
    enum class ToWardsX {
        LEFT,
        RIGHT
    };

    enum class ToWardsY {
        UP,
        DOWN
    };
private:
    MirrorAble::ToWardsX xTowards;
    MirrorAble::ToWardsY yTowards;
public:

    MirrorAble();;

    MirrorAble *setMirrorX(ToWardsX x);

    MirrorAble *setMirrorY(ToWardsY y);

    [[nodiscard]] ToWardsX getTowardsX() const;

    [[nodiscard]] ToWardsY getTowardsY() const;

    [[nodiscard]] bool isMirrorX() const;

    [[nodiscard]] bool isMirrorY() const;
};

class Mesh : public MirrorAble {
protected:
    POINT position{};
    int rotateAngle;
public:
    explicit Mesh();

    explicit Mesh(const POINT &position);

    [[maybe_unused]] Mesh(const Mesh &mesh);

    virtual ~Mesh();

    virtual void draw() = 0;

    void show();

    virtual void beforeDraw() {};

    virtual void afterDraw() {};

    Mesh *setPosition(const POINT &point);

    Mesh *setPosition(int x, int y);

    Mesh *setRotateAngle(int angle);

    [[nodiscard]] POINT getPosition() const;

    [[nodiscard]] int getRotateAngle() const;

    template<class T>
    T *castTo() {
        try {
            return dynamic_cast<T *>(this);
        } catch (std::bad_cast &e) {
            std::cout << "error: " << e.what() << std::endl;
            throw std::runtime_error(e.what());
        }
    }

};

class GeometryMesh : public Mesh {
private:
    bool fill;
    COLORREF color, tempColor;
public:
    explicit GeometryMesh() : GeometryMesh(POINT{}) {};

    explicit GeometryMesh(const POINT &position) : fill(false), color(WHITE), tempColor(color), Mesh(position) {};

    GeometryMesh *setFill(bool b);

    [[nodiscard]] bool isFill() const;

    GeometryMesh *setColor(COLORREF c);

    void beforeDraw() override;

    void afterDraw() override;
};

class RectMesh : public GeometryMesh {
protected:
    int width;
    int height;

public:
    explicit RectMesh(int width, int height) : RectMesh(POINT{0, 0}, width, height) {};

    explicit RectMesh(const POINT &position, int width, int height);

    RectMesh *setWidth(int w);

    RectMesh *setHeight(int h);

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] int getHeight() const;

    void draw() override;
};

class CircleMesh : public GeometryMesh {
protected:
    int radius;
public:
    explicit CircleMesh(int radius) : CircleMesh(POINT{0, 0}, radius) {};

    explicit CircleMesh(const POINT &position, int radius);

    CircleMesh *setRadius(int r);

    [[nodiscard]] int getRadius() const;

    void draw() override;
};

/**
 * 带图片的图形
 * 朝向基准是右下
 */
class BaseImageMesh : public Mesh {
private:
    int width;
    int height;
public:

    explicit BaseImageMesh(int width, int height);

    BaseImageMesh *setWidth(int w);

    BaseImageMesh *setHeight(int h);

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] int getHeight() const;

};

class ImageMesh : public BaseImageMesh {
private:
    ImageAsset *asset;

public:
    explicit ImageMesh(const std::string &assetName, int width, int height);

    void draw() override;
};

class MultiImageMesh : public BaseImageMesh {
private:
    AssetManager assetManager;
    int currentIndex;
    bool isLoop;
    int playInterval;
    long long lastTime;
public:
    explicit MultiImageMesh(int width, int height);

    ~MultiImageMesh() override;

    MultiImageMesh *addAsset(const std::string &assetName);

    MultiImageMesh *setPlayInterval(int i);

    MultiImageMesh *setIsLoop(bool isLoop);

    MultiImageMesh *resetCurrentIndex();

    void draw() override;

    void beforeDraw() override;
};

class ObjectStateAnimation {
private:
    Mesh *mesh;
public:
    explicit ObjectStateAnimation(Mesh *mesh);

    Mesh &getMesh();

    virtual void play();
};

class AnimationStateMachine : public MirrorAble {
private:
    std::map<byte, ObjectStateAnimation *> animations;
    byte currentState;
public:
    AnimationStateMachine();

    ~AnimationStateMachine();

    void addAnimation(byte state, ObjectStateAnimation *animation);

    void removeAnimation(byte state);

    ObjectStateAnimation *getCurrentAnimation();

    AnimationStateMachine *setCurrentState(byte stateEnum);

    void play();
};

enum class CollisionCategory {
    RECTANGLE,
    CIRCLE,
};

class CollisionAble {
protected:
    POINT position{};
    CollisionCategory category;
    Mesh *mesh;

    void setMesh(Mesh *m);

public:
    virtual ~CollisionAble();

    explicit CollisionAble(CollisionCategory category, const POINT &position);

    explicit CollisionAble(CollisionCategory category, const POINT &position, Mesh *m);

    virtual bool isCollision(CollisionAble *object) = 0;

    virtual void check();

    [[nodiscard]] CollisionCategory getCategory() const;

    [[nodiscard]] POINT getPosition() const;

    CollisionAble *setPosition(const POINT &point);

    [[nodiscard]] Mesh &getMesh() const;

    template<class T>
    T *castTo() {
        try {
            return (T *) (this);
        } catch (std::bad_cast &e) {
            std::cout << "error: " << e.what() << std::endl;
            throw std::runtime_error(e.what());
        }
    }

};

class RectangleCollision : public CollisionAble {
protected:
    int width;
    int height;
public:
    RectangleCollision(const POINT &position, int width, int height);

    bool isCollision(CollisionAble *c) override;

    RectangleCollision *setWidth(int w);

    RectangleCollision *setHeight(int h);

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] int getHeight() const;

    [[nodiscard]] POINT getLeftTop() const;

    [[nodiscard]] POINT getLeftBottom() const;

    [[nodiscard]] POINT getRightTop() const;

    [[nodiscard]] POINT getRightBottom() const;

    void check() override;
};

class CollisionLogicHandler {
public:
    static bool isCollisionFirstRect(const RectangleCollision &c1, const CollisionAble &c2);

    static bool isCollision(const RectangleCollision &c1, const RectangleCollision &c2);
};

class ObjectManager;

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
    ObjectManager *attachObjs;
    BaseObject *parentObj;
    POINT parentOffset{};
    ObjectAnchor anchor;
    int width;
    int height;
protected:
    static const int DEFAULT_WIDTH;
    static const int DEFAULT_HEIGHT;

    void updateRealPosition();

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

    BaseObject *setWidth(int w);

    [[nodiscard]] int getWidth() const;

    BaseObject *setHeight(int h);

    [[nodiscard]] int getHeight() const;

    POINT getAnchorOffset();

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

    [[nodiscard]] CollisionAble &getCollision() const;

    [[nodiscard]] bool isCollision(Object &object) const;

    void draw() override;

    Object *setIsCheckCollision(bool b);
};

class StaticObject : public Object {
protected:
};

class ObjectWithState : public Object {
private:
    AnimationStateMachine animationStateMachine;
public:
    ObjectWithState *update() override;

    void draw() override;

    AnimationStateMachine &getAnimationStateMachine();
};

class MoveableObject : public ObjectWithState {
private:
    static int ACTION_FRAME_INTERVAL;
    POINT moveVector{};
    int speed = 0;
    long long lastMoveTime = 0;

public:
    MoveableObject() : ObjectWithState() {}

    MoveableObject *update() override;

    virtual void updateAction();

    virtual bai::slot moveUp(const SlotArgs &args);

    virtual bai::slot moveDown(const SlotArgs &args);

    virtual bai::slot moveLeft(const SlotArgs &args);

    virtual bai::slot moveRight(const SlotArgs &args);

    virtual bai::slot resetVectorX();

    virtual bai::slot resetVectorY();

    virtual void afterMove() {};

    virtual bai::signal moveStop() {};

    void setSpeed(int s);

    [[nodiscard]] int getSpeed() const;

};

// 组件 TODO
class Weight : public BaseObject {
private:
    int width;
    int height;
public:
    void draw() override;

};

class ObjectManager {
private:
    std::vector<BaseObject *> objects;

public:
    ObjectManager();

    ~ObjectManager();

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

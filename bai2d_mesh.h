//
// Created by 22190 on 2024/12/8.
//

#ifndef BAI2D_MESH_H
#define BAI2D_MESH_H
#pragma once

#include <map>
#include "bai2d_asset.h"


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

    MirrorAble();

    MirrorAble *setMirrorX(ToWardsX x);

    MirrorAble *setMirrorY(ToWardsY y);

    [[nodiscard]] ToWardsX getTowardsX() const;

    [[nodiscard]] ToWardsY getTowardsY() const;

    [[nodiscard]] bool isMirrorX() const;

    [[nodiscard]] bool isMirrorY() const;
};

class Mesh : public MirrorAble {
private:
protected:
    POINT position{};
    int rotateAngle;
    double scale;
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

    virtual Mesh *setScale(double s);

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
private:
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
protected:
    int lastScale;
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

class ReversePlayAble {
private:
    bool isReversePlay;
public:
    ReversePlayAble();

    ReversePlayAble *setIsReverse(bool reverse);

    [[nodiscard]] bool isReverse() const;
};

class MultiImageMesh : public BaseImageMesh, ReversePlayAble {
private:
    AssetManager assetManager;
    int currentIndex;
    bool isLoop;
    int playInterval;
    long long lastTime;

    void forwardPlay();

    void backwardPlay();

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

class ObjectStateAnimation : public ReversePlayAble {
private:
    Mesh *mesh;
public:
    explicit ObjectStateAnimation(Mesh *mesh);

    Mesh &getMesh();

    void forwardPlay();

    void reversePlay();

    void play();

};

class AnimationStateMachine : public MirrorAble {
private:
    std::map<byte, ObjectStateAnimation *> animations;
    byte currentState;
    byte lastState;
public:
    AnimationStateMachine();

    ~AnimationStateMachine();

    void addAnimation(byte state, ObjectStateAnimation *animation);

    void removeAnimation(byte state);

    ObjectStateAnimation *getCurrentAnimation();

    AnimationStateMachine *setCurrentState(byte stateEnum);

    AnimationStateMachine *resetPlayPace();

    void play();

    AnimationStateMachine *setAllPosition(const POINT &point);

    AnimationStateMachine *setAllRotateAngle(int angle);

    AnimationStateMachine *updateAll(const POINT &point, int angle);

    AnimationStateMachine *setScale(double s);

};


#endif //BAI2D_MESH_H

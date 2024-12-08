//
// Created by 22190 on 2024/12/8.
//

#ifndef BAI2D_COLLISION_H
#define BAI2D_COLLISION_H

#include "bai2d_mesh.h"

enum class CollisionCategory {
    RECTANGLE,
    CIRCLE,
};

class CollisionAble {
private:
    int scale = 1;
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

    virtual bool isAbove(CollisionAble *object) = 0;

    virtual bool isBelow(CollisionAble *object) = 0;

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

    CollisionAble *setScale(int s);

    [[nodiscard]] int getScale() const;

};

class RectangleCollision : public CollisionAble {
protected:
    int width;
    int height;
public:
    RectangleCollision(int width, int height);

    RectangleCollision(const POINT &position, int width, int height);

    bool isCollision(CollisionAble *c) override;

    bool isAbove(CollisionAble *c) override;

    bool isBelow(CollisionAble *c) override;

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

    static bool isAboveFirstRect(const RectangleCollision &c1, const CollisionAble &c2);

    static bool isBelowFirstRect(const RectangleCollision &c1, const CollisionAble &c2);

    static bool isAbove(const RectangleCollision &c1, const RectangleCollision &c2);

    static bool isBelow(const RectangleCollision &c1, const RectangleCollision &c2);
};

#endif //BAI2D_COLLISION_H

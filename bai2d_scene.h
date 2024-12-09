//
// Created by 22190 on 2024/12/9.
//

#ifndef BAI2D_SCENE_H
#define BAI2D_SCENE_H

#include <vector>
#include "bai2d_input.h"
#include "bai2d_object.h"

class Camera : public Object {
private:
    void resetAspectRatio() const;

    bool renderSelf;
public:
    Camera(int w, int h);

    void render(BaseObjectManager &objectManager);

    std::vector<Object *> getObjectsWhichInCameraView(BaseObjectManager &objectManager);

    static void offsetObjectMesh(std::vector<Object *> *objects, POINT offset);

    Camera *setRenderSelf(bool b);

    POINT getOriginOffset();

    Camera *moveToTargetPosition(POINT point);

    POINT getViewCenter();

    Camera *offsetRefViewCenter(POINT &point, int step);
};

class Scene {
private:
    BaseObjectManager objectManager;
    Camera *camera;

    Scene *update();

public:
    Scene(int w, int h);

    ~Scene();

    void render();

    Scene *addObject(Object *object);

    Scene *removeObject(Object *object);

    Camera &getCamera();
};


#endif //BAI2D_SCENE_H

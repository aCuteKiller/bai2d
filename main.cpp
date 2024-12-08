#include "bai2d_event_base.h"
#include "bai2d_input.h"
#include "bai2d_registered_input.h"
#include "bai2d_object.h"

void mainLoop();

void mainLoopFrameHandle();

// 根据移动方向镜像动画 TODO
MultiImageMesh *
getMultiImageMesh(const std::string &dirName, const std::string &fileName, int endIndex, int width, int height,
                  int playInterval, bool isLoop = true) {
    auto *pMesh = new MultiImageMesh(width, height);
    pMesh->setPlayInterval(playInterval)->setIsLoop(isLoop);
    for (int i = 0; i <= endIndex; i++) {
        std::string path = "D:/Users/22190/Desktop/55/Persian Warrior/PNG/PNG Sequences/" + dirName + "/";
        std::string name = fileName;
        name += StrUtils::numToStrFillZero(i, 3) += ".png";
        path += name;
        GlobalAssetManager::getInstance().registeredImageAsset(name, path.c_str(), width, height);
        pMesh->addAsset(name);
    }
    return pMesh;
}

enum struct TestObjectStateEnum : byte {
    IDLE,
    RUNNING,
    DYING
};

class TestObj : public MoveableObject {
private:
public:
    bai::slot resetVectorX() override {
        if (!(KEY_A.isPressing() || KEY_D.isPressing()))
            MoveableObject::resetVectorX();
    }

    bai::slot resetVectorY() override {
        if (!(KEY_W.isPressing() || KEY_S.isPressing()))
            MoveableObject::resetVectorY();
    }

    void afterMove() override {
        this->getAnimationStateMachine().setCurrentState(byte(TestObjectStateEnum::RUNNING));
    }

    bai::signal moveStop() override {
        this->getAnimationStateMachine().setCurrentState(byte(TestObjectStateEnum::IDLE));
    }

    bai::slot dying() {
        this->getAnimationStateMachine().setCurrentState(byte(TestObjectStateEnum::DYING));
    }

    void moveUp(const SlotArgs &args) override {
        this->getAnimationStateMachine().setMirrorY(MirrorAble::ToWardsY::UP);
        MoveableObject::moveUp(args);
    }

    void moveDown(const SlotArgs &args) override {
        this->getAnimationStateMachine().setMirrorY(MirrorAble::ToWardsY::DOWN);
        MoveableObject::moveDown(args);
    }

    void moveLeft(const SlotArgs &args) override {
        this->getAnimationStateMachine().setMirrorX(MirrorAble::ToWardsX::LEFT);
        MoveableObject::moveLeft(args);
    }

    void moveRight(const SlotArgs &args) override {
        this->getAnimationStateMachine().setMirrorX(MirrorAble::ToWardsX::RIGHT);
        MoveableObject::moveRight(args);
    }
};

TestObj testObj1, testObj2, testObj3;
ObjectManager objectManager;

int main() {
    // 设置控制台输出代码页为UTF-8
    SetConsoleOutputCP(CP_UTF8);

    initgraph(640, 480);

    GlobalInputEventManager &globalInputEventManager = GlobalInputEventManager::getInstance();

    testObj1.attach(&testObj2);
    testObj2.setPosition(50, 50);
    testObj2.setParentOffset(40, 40);
    testObj3.setParentOffset(-20, 20);
    testObj3.setPosition(300, 300);
//    testObj2.attach(&testObj3);
    testObj2.setPosition(100, 100);
//    testObj3.setRotateAngle(30);
//    testObj1.setRotateAngle(45);

    setbkcolor(WHITE);

    auto *pDyingMesh = getMultiImageMesh("Dying", "Dying_", 14, 200, 200, 50, false);
    auto *pIdleMesh = getMultiImageMesh("Idle", "Idle_", 17, 200, 200, 100);
    auto *pRunningMesh = getMultiImageMesh("Running", "Running_", 11, 200, 200, 50);

//    testObj1.setMesh(new ImageMesh("Dying_001.png", 90, 90));
    testObj1.getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::DYING),
                                                     new ObjectStateAnimation(pDyingMesh));
    testObj1.getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::IDLE),
                                                     new ObjectStateAnimation(pIdleMesh));
    testObj1.getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::RUNNING),
                                                     new ObjectStateAnimation(pRunningMesh));
    testObj1.getAnimationStateMachine().setCurrentState(byte(TestObjectStateEnum::IDLE));
    testObj1.setRenderingIndex(10);


    testObj2.setAnchor(ObjectAnchor::RIGHT_BOTTOM);
    testObj1.setPosition(200, 200);
    testObj2.setMesh(new CircleMesh(50));
    testObj2.getMesh().castTo<GeometryMesh>()->setColor(BLACK);
    testObj2.getMesh().castTo<GeometryMesh>()->setColor(BLACK);
    dynamic_cast<GeometryMesh &> (testObj2.getMesh()).setFill(true);
    objectManager.addObject(&testObj1);
    objectManager.addObject(&testObj3);

    testObj1.setSpeed(3);
//    testObj1.setRotateAngle(45);
    testObj1.setIsCheckCollision(false)->getCollision().getMesh().castTo<GeometryMesh>()->setColor(RED);
    testObj1.getCollision().castTo<RectangleCollision>()->setWidth(60)->setHeight(60);

    globalInputEventManager.connect(KEY_W, SIGNAL(&KeyInput::pressed), testObj1, SLOT(&TestObj::moveUp));
    globalInputEventManager.connect(KEY_S, SIGNAL(&KeyInput::pressed), testObj1, SLOT(&TestObj::moveDown));
    globalInputEventManager.connect(KEY_A, SIGNAL(&KeyInput::pressed), testObj1, SLOT(&TestObj::moveLeft));
    globalInputEventManager.connect(KEY_D, SIGNAL(&KeyInput::pressed), testObj1, SLOT(&TestObj::moveRight));
    globalInputEventManager.connect(KEY_Q, SIGNAL(&KeyInput::pressed), testObj1, SLOT(&TestObj::dying));

    globalInputEventManager.connect(KEY_W, SIGNAL(&KeyInput::released), testObj1, SLOT(&TestObj::resetVectorY));
    globalInputEventManager.connect(KEY_S, SIGNAL(&KeyInput::released), testObj1, SLOT(&TestObj::resetVectorY));
    globalInputEventManager.connect(KEY_A, SIGNAL(&KeyInput::released), testObj1, SLOT(&TestObj::resetVectorX));
    globalInputEventManager.connect(KEY_D, SIGNAL(&KeyInput::released), testObj1, SLOT(&TestObj::resetVectorX));

    const std::future<void> &future = globalInputEventManager.runInputEventLoop();
    mainLoop();

    closegraph();
    return 0;
}

void mainLoop() {
    auto lastUpdateTime = std::chrono::high_resolution_clock::now();
    while (true) {
        if (!IS_ENGINE_RUNNING) break;
        auto now = std::chrono::high_resolution_clock::now();
        auto spacing = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime).count();
        if (spacing >= BAI_ENGINE_STANDARD_FRAME_SPACING) {
            REAL_RENDERING_FPS = static_cast<int>(1000 / spacing);
            lastUpdateTime = now;
            mainLoopFrameHandle();
        } else {
            int sleepTime = static_cast<int>(BAI_ENGINE_STANDARD_FRAME_SPACING - spacing);
            if (sleepTime > 0) {
                auto start = std::chrono::high_resolution_clock::now();
                auto end = start + std::chrono::milliseconds(sleepTime);
                do {
                    std::this_thread::yield();
                } while (std::chrono::high_resolution_clock::now() < end);
            }
        }
    }
}

void mainLoopFrameHandle() {
    BeginBatchDraw();
    cleardevice();

    objectManager.showAll();

    const TCHAR *text;
    text = ("FPS: " + std::to_string(REAL_RENDERING_FPS)).c_str();
    std::wstring fpsText = std::to_wstring(REAL_RENDERING_FPS);
    COLORREF temp = gettextcolor();
    settextcolor(GREEN);
    outtextxy(10, 10, text);
    settextcolor(temp);

    EndBatchDraw();
}
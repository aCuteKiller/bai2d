#include "bai2d_event_base.h"
#include "bai2d_input.h"
#include "bai2d_registered_input.h"
#include "bai2d_object.h"
#include "bai2d_shortcut_func.h"

void mainLoop();

void mainLoopFrameHandle();

enum struct TestObjectStateEnum : byte {
    IDLE,
    RUNNING,
    DYING,
    FALLING,
    START_JUMP,
    JUMP_LOOP,
};

class TestObj : public Pawn {
private:
public:
    bai::slot resetVectorX(const SlotArgs &args) override {
        if (!(KEY_A.isPressing() || KEY_D.isPressing()))
            Pawn::resetVectorX(args);
        else if (KEY_A.isPressing()) {
            this->moveLeft(args);
        } else if (KEY_D.isPressing()) {
            this->moveRight(args);
        }
    }

    bai::slot resetVectorY(const SlotArgs &args) override {
        if (!(KEY_W.isPressing() || KEY_S.isPressing()))
            Pawn::resetVectorY(args);
        else if (KEY_W.isPressing()) {
            this->moveUp(args);
        } else if (KEY_S.isPressing()) {
            this->moveDown(args);
        }
    }

    void toRight() override {
        this->getAnimationStateMachine().setMirrorX(MirrorAble::ToWardsX::RIGHT);
    }

    void toLeft() override {
        this->getAnimationStateMachine().setMirrorX(MirrorAble::ToWardsX::LEFT);
    }

    void startJump() override {
        this->getAnimationStateMachine().setCurrentState(byte(TestObjectStateEnum::START_JUMP));
    }

    void fallingDown() override {
        this->getAnimationStateMachine().setCurrentState(byte(TestObjectStateEnum::FALLING));
    }

    void jumpLoop() override {
        this->getAnimationStateMachine().setCurrentState(byte(TestObjectStateEnum::JUMP_LOOP));
    }

    void move() override {
        if (!this->isJumping())this->getAnimationStateMachine().setCurrentState(byte(TestObjectStateEnum::RUNNING));
    }

    void idle() override {
        if (!this->isJumping())this->getAnimationStateMachine().setCurrentState(byte(TestObjectStateEnum::IDLE));
    }

    void dying() {
        this->getAnimationStateMachine().setCurrentState(byte(TestObjectStateEnum::DYING));
    }
};

TestObj testObj1, testObj2, testObj3;
BaseObjectManager objectManager;

int main() {
    // 设置控制台输出代码页为UTF-8
    SetConsoleOutputCP(CP_UTF8);

    initgraph(640, 480);

    GlobalInputEventManager &globalInputEventManager = GlobalInputEventManager::getInstance();

//    testObj1.attach(&testObj2);
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
    auto *pFallingMesh = getMultiImageMesh("Falling Down", "Falling Down_", 5, 200, 200, 50, false);
    auto *pStartJumpMesh = getMultiImageMesh("Jump Start", "Jump Start_", 5, 200, 200, 50, false);
    auto *pJumpLoopMesh = getMultiImageMesh("Jump Loop", "Jump Loop_", 5, 200, 200, 50);

//    testObj1.setMesh(new ImageMesh("Dying_001.png", 90, 90));
    testObj1.getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::DYING),
                                                     new ObjectStateAnimation(pDyingMesh));
    testObj1.getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::IDLE),
                                                     new ObjectStateAnimation(pIdleMesh));
    testObj1.getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::RUNNING),
                                                     new ObjectStateAnimation(pRunningMesh));
    testObj1.getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::START_JUMP),
                                                     new ObjectStateAnimation(pStartJumpMesh));
    testObj1.getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::FALLING),
                                                     new ObjectStateAnimation(pFallingMesh));
    testObj1.getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::JUMP_LOOP),
                                                     new ObjectStateAnimation(pJumpLoopMesh));

    testObj1.getAnimationStateMachine().setCurrentState(byte(TestObjectStateEnum::IDLE));
    testObj1.setRenderingIndex(10);
    testObj1.setMaxJumpTimes(2);

    testObj2.setAnchor(ObjectAnchor::RIGHT_BOTTOM);
    testObj1.setPosition(400, 400);
    testObj2.setMesh(new CircleMesh(50));
    testObj2.getMesh().castTo<GeometryMesh>()->setColor(BLACK);
    testObj2.getMesh().castTo<GeometryMesh>()->setColor(BLACK);
    dynamic_cast<GeometryMesh &> (testObj2.getMesh()).setFill(true);
    objectManager.addObject(&testObj1);
    objectManager.addObject(&testObj3);

    testObj1.setSpeed(3);
//    testObj1.setRotateAngle(45);
    testObj1.setIsCheckCollision(true)->getCollision()->getMesh().castTo<GeometryMesh>()->setColor(RED);
    testObj1.getCollision()->castTo<RectangleCollision>()->setWidth(60)->setHeight(100);

//    globalInputEventManager.connect(KEY_W, SIGNAL(&KeyInput::pressed), testObj1, SLOT(&TestObj::moveUp));
//    globalInputEventManager.connect(KEY_S, SIGNAL(&KeyInput::pressed), testObj1, SLOT(&TestObj::moveDown));
    globalInputEventManager.connect(KEY_A, SIGNAL(&KeyInput::pressed), testObj1, SLOT(&TestObj::moveLeft));
    globalInputEventManager.connect(KEY_D, SIGNAL(&KeyInput::pressed), testObj1, SLOT(&TestObj::moveRight));
    globalInputEventManager.connect(KEY_Q, SIGNAL(&KeyInput::pressed), testObj1, SLOT(&TestObj::dying));
    globalInputEventManager.connect(KEY_SPACE, SIGNAL(&KeyInput::pressed), testObj1, SLOT(&TestObj::jump));

//    globalInputEventManager.connect(KEY_W, SIGNAL(&KeyInput::released), testObj1, SLOT(&TestObj::resetVectorY));
//    globalInputEventManager.connect(KEY_S, SIGNAL(&KeyInput::released), testObj1, SLOT(&TestObj::resetVectorY));
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
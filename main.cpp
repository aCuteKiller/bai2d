#include "bai2d_event_base.h"
#include "bai2d_input.h"
#include "bai2d_registered_input.h"
#include "bai2d_object.h"
#include "bai2d_shortcut_func.h"
#include "bai2d_scene.h"

// TODO->垃圾回收、任务机制、物理、图层

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 640
Scene scene(WINDOW_WIDTH, WINDOW_HEIGHT);

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

class TestPlayer : public Pawn {
private:
public:
    TestPlayer() : Pawn() {};

    CREATE_FUNC(TestPlayer);

    bai::slot resetVectorX(const SlotArgs &args) override {
        if (KEY_A.isPressing()) {
            this->moveLeft(args);
        } else if (KEY_D.isPressing()) {
            this->moveRight(args);
        } else {
            Pawn::resetVectorX(args);
        }
    }

    bai::slot resetVectorY(const SlotArgs &args) override {
        if (KEY_W.isPressing()) {
            this->moveUp(args);
        } else if (KEY_S.isPressing()) {
            this->moveDown(args);
        } else {
            Pawn::resetVectorY(args);
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
        this->getAnimationStateMachine().setCurrentState(byte(TestObjectStateEnum::DYING))->resetPlayPace();
    }

    bai::slot scale(const SlotArgs &args) {
        this->setScale(this->getScale() + 0.1);
    }

    bai::slot shrink(const SlotArgs &args) {
        this->setScale(this->getScale() - 0.1);
    }

    bai::slot attachCamera(const SlotArgs &args) {
        if (scene.getCamera().getParentObj()) {
            this->detach(&scene.getCamera());
        } else {
            this->attach(&scene.getCamera());
        }
    }

    bai::slot cameraMoveToMouse(const SlotArgs &args) {
        if (!scene.getCamera().getParentObj()) {
            InputInfo *inputInfo = const_cast<SlotArgs &>(args).castTo<InputInfo>();
            scene.getCamera().moveToTargetPosition(inputInfo->clickPosition);
        }
    }

    bai::slot cameraOffsetRefViewCenter(const SlotArgs &args) {
        if (!scene.getCamera().getParentObj() && MOUSE_MIDDLE.isPressing()) {
            InputInfo *inputInfo = const_cast<SlotArgs &>(args).castTo<InputInfo>();
            scene.getCamera().offsetRefViewCenter(inputInfo->clickPosition, 1, 10, 20, 100);
        }
    }
};

int main() {
    // 设置控制台输出代码页为UTF-8
    SetConsoleOutputCP(CP_UTF8);
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    setbkcolor(WHITE);
    GlobalInputEventManager &globalInputEventManager = GlobalInputEventManager::getInstance();

    scene.getCamera().setRenderSelf(true);

    TestPlayer *player = CREATE(TestPlayer);
    player->setPosition(100, 100)->setWidth(100)->setHeight(100);
    player->setSpeed(5);
    player->attach(&scene.getCamera());
    player->disableAnimationStateMachine();

    Object *pObject = CREATE(Object);
    pObject->setPosition(200, 200);
    pObject->setMesh(new RectMesh(500, 10))->getMesh().castTo<GeometryMesh>()->setColor(RED);
    pObject->setCollision(new RectangleCollision(500, 10));

    scene.addObject(pObject);
    scene.addObject(player);

    CREATE(TestPlayer);


    std::string srcDir = "D:/Users/22190/Desktop/55/Persian Warrior/PNG/";
    auto *pDyingMesh = getMultiImageMesh(srcDir, "PNG Sequences/Dying/Dying_", 14, player->getWidth(),
                                         player->getHeight(), 50,
                                         false);
    auto *pIdleMesh = getMultiImageMesh(srcDir, "PNG Sequences/Idle/Idle_", 17, player->getWidth(), player->getHeight(),
                                        100);
    auto *pRunningMesh = getMultiImageMesh(srcDir, "PNG Sequences/Running/Running_", 11, player->getWidth(),
                                           player->getHeight(), 50);
    auto *pFallingMesh = getMultiImageMesh(srcDir, "PNG Sequences/Falling Down/Falling Down_", 5, player->getWidth(),
                                           player->getHeight(), 50, false);
    auto *pStartJumpMesh = getMultiImageMesh(srcDir, "PNG Sequences/Jump Start/Jump Start_", 5, player->getWidth(),
                                             player->getHeight(), 50, false);
    auto *pJumpLoopMesh = getMultiImageMesh(srcDir, "PNG Sequences/Jump Loop/Jump Loop_", 5, player->getWidth(),
                                            player->getHeight(), 50);

    player->getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::DYING),
                                                    new ObjectStateAnimation(pDyingMesh));
    player->getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::IDLE),
                                                    new ObjectStateAnimation(pIdleMesh));
    player->getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::RUNNING),
                                                    new ObjectStateAnimation(pRunningMesh));
    player->getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::START_JUMP),
                                                    new ObjectStateAnimation(pStartJumpMesh));
    player->getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::FALLING),
                                                    new ObjectStateAnimation(pFallingMesh));
    player->getAnimationStateMachine().addAnimation(byte(TestObjectStateEnum::JUMP_LOOP),
                                                    new ObjectStateAnimation(pJumpLoopMesh));

    player->getAnimationStateMachine().setCurrentState(byte(TestObjectStateEnum::IDLE));

    globalInputEventManager.connect(KEY_A, SIGNAL(&KeyInput::pressed), *player, SLOT(&TestPlayer::moveLeft));
    globalInputEventManager.connect(KEY_D, SIGNAL(&KeyInput::pressed), *player, SLOT(&TestPlayer::moveRight));
    globalInputEventManager.connect(KEY_V, SIGNAL(&KeyInput::pressed), *player, SLOT(&TestPlayer::dying));
    globalInputEventManager.connect(KEY_SPACE, SIGNAL(&KeyInput::pressed), *player, SLOT(&TestPlayer::jump));
    globalInputEventManager.connect(KEY_E, SIGNAL(&KeyInput::pressed), *player, SLOT(&TestPlayer::scale));
    globalInputEventManager.connect(KEY_Q, SIGNAL(&KeyInput::pressed), *player, SLOT(&TestPlayer::shrink));
    globalInputEventManager.connect(KEY_F, SIGNAL(&KeyInput::pressed), *player, SLOT(&TestPlayer::attachCamera));
    globalInputEventManager.connect(MOUSE_LEFT, SIGNAL(&MouseInput::pressed), *player,
                                    SLOT(&TestPlayer::cameraMoveToMouse));
    globalInputEventManager.connect(MOUSE_MOVING, SIGNAL(&MouseInput::moving), *player,
                                    SLOT(&TestPlayer::cameraOffsetRefViewCenter));
    globalInputEventManager.connect(MOUSE_MOVING, SIGNAL(&MouseInput::silence), *player,
                                    SLOT(&TestPlayer::cameraOffsetRefViewCenter));

    globalInputEventManager.connect(KEY_A, SIGNAL(&KeyInput::released), *player, SLOT(&TestPlayer::resetVectorX));
    globalInputEventManager.connect(KEY_D, SIGNAL(&KeyInput::released), *player, SLOT(&TestPlayer::resetVectorX));

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

    scene.render();


    const TCHAR *text;
    text = ("FPS: " + std::to_string(REAL_RENDERING_FPS)).c_str();
    std::wstring fpsText = std::to_wstring(REAL_RENDERING_FPS);
    COLORREF temp = gettextcolor();
    settextcolor(GREEN);
    outtextxy(10, 10, text);
    settextcolor(temp);

    EndBatchDraw();

    AutoReleasePool::getInstance().tryRelease();
}
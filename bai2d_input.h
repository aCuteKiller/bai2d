//
// Created by 22190 on 2024/12/4.
// 知识点：
// 1. [[nodiscard]]->[[nodiscard]] 是 C++17 引入的一个属性，用于提示编译器，函数的返回值不应该被忽略。
//                  当一个带有 [[nodiscard]] 属性的函数被调用时，如果其返回值没有被使用，编译器会发出警告或错误，提醒开发者注意返回值的重要性。
// 2. explicit 关键字->作用：explicit 关键字用于修饰构造函数，防止隐式类型转换和复制初始化。
//                  目的：避免意外的隐式转换，提高代码的安全性和可读性。
//

#ifndef BAI2D_INPUT_H
#define BAI2D_INPUT_H

#include "bai2d_event_base.h"
#include "bai2d_constants.h"
#include "bai2d_utils.h"
#include "graphics.h"
#include <thread>
#include <future>
#include <ctime>
#include <chrono>
#include <map>

#define REGISTERED_KEY(keyCode) (KeyInput&)InputManager::getInstance().registeredInput(InputCategory::KEYBOARD, (byte) (keyCode))
#define REGISTERED_MOUSE(mouseButton) (MouseInput&)(InputManager::getInstance().registeredInput(InputCategory::MOUSE, static_cast<byte>(mouseButton)))

// 鼠标的输入枚举
enum class MouseButtonEnum : byte {
    LEFT = 1,
    MIDDLE = 2,
    RIGHT = 3,
};
static std::map<USHORT, MouseButtonEnum> MOUSE_BUTTON_V_CODE_TABLE({
                                                                           {WM_LBUTTONUP,   MouseButtonEnum::LEFT},
                                                                           {WM_MBUTTONUP,   MouseButtonEnum::MIDDLE},
                                                                           {WM_RBUTTONUP,   MouseButtonEnum::RIGHT},
                                                                           {WM_LBUTTONDOWN, MouseButtonEnum::LEFT},
                                                                           {WM_MBUTTONDOWN, MouseButtonEnum::MIDDLE},
                                                                           {WM_RBUTTONDOWN, MouseButtonEnum::RIGHT},
                                                                   });
enum InputCategory : byte {
    KEYBOARD,
    MOUSE
};

/**
 * 输入基类
 * 创建时间：2024/12/4
 * 封装了输入的信号，并且可以注册到事件管理器中
 * 并且可以获取输入的状态
 */
class InputBase : public EventAble {
protected:
    bool isActive;
    byte code;
    int categoryCode;
    bool lastState;
    bool currentState;
    long long pressingStart;
public:
    explicit InputBase(byte code, int categoryCode);

    [[nodiscard]] bool getIsActive() const;

    void setIsActive(bool isActive_);

    void update(bool state);

    bai::signal pressed();

    bai::signal released();

    bai::signal pressing();

    bai::signal releasing();

    [[nodiscard]] bool isPressed() const;

    [[nodiscard]] bool isReleased() const;

    [[nodiscard]] bool isPressing() const;

    [[nodiscard]] bool isReleasing() const;

    [[nodiscard]] int getCategoryCode() const;

    [[nodiscard]] int getCode() const;

    [[nodiscard]] short getPressingDuration() const;
};

/**
 * 键盘和鼠标的输入
 * 创建时间：2024/12/4
 * 封装了输入的信号，并且可以注册到事件管理器中
 * 并且可以获取输入的状态
 */
class KeyInput : public InputBase {
private:
public:
    explicit KeyInput(byte code) : InputBase(code, InputCategory::KEYBOARD) {}
};

/**
 * 鼠标的输入
 * 创建时间：2024/12/4
 * 封装了输入的信号，并且可以注册到事件管理器中
 * 并且可以获取输入的状态
 */
class MouseInput : public InputBase {
private:
    UCHAR scrollState;
public:
    static UCHAR SCROLL_UP_STATE;
    static UCHAR SCROLL_DOWN_STATE;

    explicit MouseInput(byte code) : InputBase(code, InputCategory::MOUSE), scrollState(false) {}

    bai::signal click();

    bai::signal wheelScrollUp();

    bai::signal wheelScrollDown();

    [[nodiscard]] bool isScrollUp() const;

    [[nodiscard]] bool isScrollDown() const;

    void updateScrollState(UCHAR state);

    [[nodiscard]] UCHAR getScrollState() const;
};


/**
 * 全局输入管理器
 * 创建时间：2024/12/4
 * 管理所有的输入
 */
class InputManager {
private:
    static InputManager instance;
    std::vector<InputBase *> inputs;

    InputManager();

public:
    ~InputManager();

    static InputManager &getInstance();

    InputBase &registeredInput(InputCategory category, byte code);

    InputBase &getInput(InputCategory category, byte code);

    [[nodiscard]]bool isRegistered(InputCategory category, byte code) const;

    void updateInput(InputCategory category, byte code, bool state);

    void resetAllIsActive(bool isActive);

    void updateFalseToNoActive();

    std::vector<InputBase *> &getAllInputs();

    void updateMouseScrollInput(UCHAR scrollState);

    void updateTrueToPress();
};


class GlobalInputEventManager : public BaseEventManager {
private:
    static GlobalInputEventManager instance;
    InputManager &inputManager;

    GlobalInputEventManager() : inputManager(InputManager::getInstance()) {};

    void inputEventLoop();

    void handleOneFrame();

    InputInfo updateInputState();

    void updateInputLogic(const InputInfo &inputInfo);

    void updateCode(InputCategory category, byte vCode, bool state);

public:
    static GlobalInputEventManager &getInstance();

    std::future<void> runInputEventLoop();
};

#endif // BAI2D_INPUT_H

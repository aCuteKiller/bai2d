//
// Created by 22190 on 2024/12/4.
//

#include <algorithm>
#include "bai2d_input.h"

void InputBase::update(bool state) {
    this->lastState = this->currentState;
    this->currentState = state;
    if (this->isPressed())this->pressingStart = TimeUtils::getNowMilliseconds();
    else if (this->isReleased())this->pressingStart = 0;
}

bai::signal InputBase::pressed() {

}

bai::signal InputBase::released() {

}

bool InputBase::isPressed() const {
    return !this->lastState && this->currentState;
}

bool InputBase::isReleased() const {
    return this->lastState && !this->currentState;
}

bai::signal InputBase::pressing() {

}

bai::signal InputBase::releasing() {

}

bool InputBase::isPressing() const {
    return this->lastState && this->currentState;
}

bool InputBase::isReleasing() const {
    return !this->lastState && !this->currentState;
}

int InputBase::getCategoryCode() const {
    return this->categoryCode;
}

int InputBase::getCode() const {
    return this->code;
}

bool InputBase::getIsActive() const {
    return isActive;
}

void InputBase::setIsActive(bool isActive_) {
    InputBase::isActive = isActive_;
}

InputBase::InputBase(byte code, int categoryCode) : code(code), lastState(false), currentState(false),
                                                    categoryCode(categoryCode), isActive(false), pressingStart(0) {}

short InputBase::getPressingDuration() const {
    if (this->pressingStart == 0)
        return 0;
    return static_cast<short>(TimeUtils::getNowMilliseconds() - this->pressingStart);
}

bai::signal MouseInput::click() {

}

bai::signal MouseInput::wheelScrollUp() {

}

bai::signal MouseInput::wheelScrollDown() {

}

bool MouseInput::isScrollUp() const {
    return this->scrollState == SCROLL_UP_STATE;
}

bool MouseInput::isScrollDown() const {
    return this->scrollState == SCROLL_DOWN_STATE;
}

UCHAR MouseInput::SCROLL_UP_STATE = 1;
UCHAR MouseInput::SCROLL_DOWN_STATE = 2;

void MouseInput::updateScrollState(UCHAR state) {
    this->scrollState = state;
}

UCHAR MouseInput::getScrollState() const {
    return this->scrollState;
}

bai::signal MouseInput::moving() {

}

bai::signal MouseInput::silence() {

}

UCHAR MouseInput::getMoveState() const {
    return this->moveState;
}

void MouseInput::setMoveState(UCHAR state) {
    this->moveState = state;
}

UCHAR MouseInput::MOVING_STATE = 0x01;
UCHAR MouseInput::STATIC_STATE = 0x02;

InputManager::InputManager() {
    this->inputs = std::vector<InputBase *>();
}

InputManager &InputManager::getInstance() {
    return instance;
}

InputBase &InputManager::registeredInput(InputCategory category, byte code) {
    return getInput(category, code);
}

bool InputManager::isRegistered(InputCategory category, byte code) const {
    return std::any_of(this->inputs.begin(), this->inputs.end(), [category, code](const InputBase *input) {
        return input->getCategoryCode() == category && input->getCode() == code;
    });
}

void InputManager::updateInput(InputCategory category, byte code, bool state) {
    for (InputBase *input: this->inputs) {
        if (input->getCategoryCode() == category && input->getCode() == code) {
            input->update(state);
            input->setIsActive(true);
        }
    }
}

InputBase &InputManager::getInput(InputCategory category, byte code) {
    for (InputBase *input: this->inputs) {
        if (input->getCategoryCode() == category && input->getCode() == code) {
            return *input;
        }
    }
    switch (category) {
        case KEYBOARD:
            this->inputs.push_back(new KeyInput(code));
            break;
        case MOUSE:
            this->inputs.push_back(new MouseInput(code));
            break;
    }
    return *this->inputs.back();
}


InputManager InputManager::instance;

InputManager::~InputManager() {
    for (InputBase *input: this->inputs) {
        delete input;
    }
}

void InputManager::setAllIsActive(bool isActive) {
    for (InputBase *input: this->inputs) {
        input->setIsActive(isActive);
    }
}

void InputManager::updateFalseToNoActive() {
    for (InputBase *input: this->inputs) {
        if (!input->getIsActive()) {
            input->update(false);
            if (input->getCategoryCode() == InputCategory::MOUSE) {
                auto *pInput = (MouseInput *) input;
                pInput->updateScrollState(false);
                pInput->setMoveState(false);
            }
        }
    }
}

std::vector<InputBase *> &InputManager::getAllInputs() {
    return this->inputs;
}

void InputManager::updateTrueToPress() {
    for (InputBase *input: this->inputs) {
        if (!input->getIsActive() && (input->isPressed() || input->isPressing())) {
            input->update(true);
            input->setIsActive(true);
        }
    }
}

void InputManager::updateMouseScrollInput(UCHAR scrollState) {
    for (InputBase *input: this->inputs) {
        if (input->getCategoryCode() == InputCategory::MOUSE) {
            auto *mouseInput = (MouseInput *) (input);
            mouseInput->updateScrollState(scrollState);
            mouseInput->setIsActive(true);
        }
    }
}

void InputManager::updateToMouseMoving() {
    for (InputBase *input: this->inputs) {
        if (input->getCategoryCode() == InputCategory::MOUSE) {
            auto *mouseInput = (MouseInput *) (input);
            mouseInput->setMoveState(MouseInput::MOVING_STATE);
            mouseInput->setIsActive(true);
        }
    }
}

void InputManager::updateToMouseStatic() {
    for (InputBase *input: this->inputs) {
        if (input->getCategoryCode() == InputCategory::MOUSE) {
            auto *mouseInput = (MouseInput *) (input);
            mouseInput->setMoveState(MouseInput::STATIC_STATE);
            mouseInput->setIsActive(true);
        }
    }
}

std::future<void> GlobalInputEventManager::runInputEventLoop() {
    return std::async(std::launch::async, [this] { this->inputEventLoop(); });
}

/*
 * 踩坑了！
 * std::this_thread::sleep_for的精度不精确，会导致线程多睡眠几到及时毫秒
 * 这里采用以下形式解决->
 * auto start = std::chrono::high_resolution_clock::now();
    auto end = start + std::chrono::milliseconds(milliseconds);
    do {
        std::this_thread::yield();
    } while (std::chrono::high_resolution_clock::now() < end);
 */
void GlobalInputEventManager::inputEventLoop() {
    /*
//      精度不准确
    long long int now, spacing,lastUpdateTime = 0;
    while (true) {
        if (!IS_ENGINE_RUNNING) break;

        now = TimeUtils::getNowMilliseconds();
        spacing = now - lastUpdateTime;

        if (spacing >= BAI_ENGINE_STANDARD_FRAME_SPACING) {
            std::cout << "fps: " << 1000 / spacing << std::endl;
            lastUpdateTime = now;
            handleOneFrame();
        } else {
            int sleepTime = static_cast<int>(BAI_ENGINE_STANDARD_FRAME_SPACING - spacing);
            if (sleepTime > 0) {
                long long int start = TimeUtils::getNowMilliseconds();
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
                std::cout << "sleepTime: " << sleepTime << std::endl;
                std::cout << "realSleepTime: " << TimeUtils::getNowMilliseconds() - start << std::endl;
            }
        }
    }*/
    auto lastUpdateTime = std::chrono::high_resolution_clock::now();
    while (true) {
        if (!IS_ENGINE_RUNNING) break;
        auto now = std::chrono::high_resolution_clock::now();
        auto spacing = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime).count();
        if (spacing >= BAI_ENGINE_STANDARD_FRAME_SPACING) {
            lastUpdateTime = now;
            handleOneFrame();
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

GlobalInputEventManager &GlobalInputEventManager::getInstance() {
    return instance;
}

void GlobalInputEventManager::handleOneFrame() {
    this->updateInputLogic(this->updateInputState());
}

void GlobalInputEventManager::updateCode(InputCategory category, byte vCode, bool state) {
    for (short code = 0; code < 256; code++) {
        if (inputManager.isRegistered(category, vCode)) {
            inputManager.updateInput(category, vCode, state);
            break;
        }
    }
}

InputInfo GlobalInputEventManager::updateInputState() {
// 更新状态
    inputManager.setAllIsActive(false);
    ExMessage exMessage{};
    if (peekmessage(&exMessage, EX_MOUSE | EX_KEY)) {
        switch (exMessage.message) {
            case WM_KEYUP:
                updateCode(InputCategory::KEYBOARD, exMessage.vkcode, false);
                break;
            case WM_KEYDOWN:
                updateCode(InputCategory::KEYBOARD, exMessage.vkcode, true);
                break;
            case WM_LBUTTONUP :
            case WM_RBUTTONUP :
            case WM_MBUTTONUP:
                updateCode(InputCategory::MOUSE, (byte) MOUSE_BUTTON_V_CODE_TABLE[exMessage.message], false);
                break;
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MBUTTONDOWN:
                updateCode(InputCategory::MOUSE, (byte) MOUSE_BUTTON_V_CODE_TABLE[exMessage.message], true);
                break;
            case WM_MOUSEWHEEL:
                // 在这里处理鼠标滚轮消息
                if (exMessage.wheel > 0) {
                    this->inputManager.updateMouseScrollInput(MouseInput::SCROLL_UP_STATE);
                } else if (exMessage.wheel < 0) {
                    this->inputManager.updateMouseScrollInput(MouseInput::SCROLL_DOWN_STATE);
                }
                break;
        }
        if (lastMousePosition.x != exMessage.x || lastMousePosition.y != exMessage.y) {
            this->inputManager.updateToMouseMoving();
            lastMousePosition = POINT{exMessage.x, exMessage.y};
            this->lastMoveTime = TimeUtils::getNowMilliseconds();
        }
    }
    if (TimeUtils::getNowMilliseconds() - this->lastMoveTime >= 50) {
        this->inputManager.updateToMouseStatic();
    }
    // 如果没有更新，且上一帧是按下和持续按下，则进true；
    inputManager.updateTrueToPress();
    inputManager.updateFalseToNoActive();

    InputInfo inputInfo;
    inputInfo.clickPosition = lastMousePosition;

    return inputInfo;
}

void GlobalInputEventManager::updateInputLogic(const InputInfo &inputInfo) {
    const auto &args = dynamic_cast<const SlotArgs &>(inputInfo);
// 处理逻辑
    for (InputBase *input: inputManager.getAllInputs()) {
        if (input->isPressed()) {
            this->notify(*input, SIGNAL(&InputBase::pressed), args);
        } else if (input->isPressing()) {
            this->notify(*input, SIGNAL(&InputBase::pressing), args);
        } else if (input->isReleased()) {
            this->notify(*input, SIGNAL(&InputBase::released), args);
            this->notify(*input, SIGNAL(&MouseInput::click), args);
        } else if (input->isReleasing()) {
            this->notify(*input, SIGNAL(&InputBase::releasing), args);
        }
        if (input->getCategoryCode() == InputCategory::MOUSE) {
            auto *pMouseInput = (MouseInput *) input;
            if (pMouseInput->isScrollUp()) {
                this->notify(*pMouseInput, SIGNAL(&MouseInput::wheelScrollUp), args);
            } else if (pMouseInput->isScrollDown())
                this->notify(*pMouseInput, SIGNAL(&MouseInput::wheelScrollDown), args);
            if (pMouseInput->getMoveState() == MouseInput::MOVING_STATE) {
                this->notify(*pMouseInput, SIGNAL(&MouseInput::moving), args);
            } else if (pMouseInput->getMoveState() == MouseInput::STATIC_STATE) {
                this->notify(*pMouseInput, SIGNAL(&MouseInput::silence), args);
            }
        }
    }
}

GlobalInputEventManager GlobalInputEventManager::instance;

void InputRegistrationTable::registerKey(int alias, byte keyCode) {
    inputTable[alias] = &REGISTERED_KEY(keyCode);
}

void InputRegistrationTable::registerMouse(int alias, byte keyCode) {
    inputTable[alias] = &REGISTERED_MOUSE(keyCode);
}

template<class T>
typename std::enable_if<std::is_base_of<InputBase, T>::value, T>::type *InputRegistrationTable::getInput(int alias) {
    return dynamic_cast<T *>(inputTable[alias]);
}

//
// Created by 22190 on 2024/12/4.
//

#ifndef BAI2D_REGISTERED_INPUT_H
#define BAI2D_REGISTERED_INPUT_H
#pragma once

#include "bai2d_input.h"

// 键盘按键
// 以下注册了一些常用的游戏按键
static KeyInput &KEY_W = REGISTERED_KEY('W');
static KeyInput &KEY_A = REGISTERED_KEY('A');
static KeyInput &KEY_S = REGISTERED_KEY('S');
static KeyInput &KEY_D = REGISTERED_KEY('D');
static KeyInput &KEY_E = REGISTERED_KEY('E');
static KeyInput &KEY_Q = REGISTERED_KEY('Q');
static KeyInput &KEY_B = REGISTERED_KEY('B');
static KeyInput &KEY_V = REGISTERED_KEY('V');
static KeyInput &KEY_C = REGISTERED_KEY('C');
static KeyInput &KEY_G = REGISTERED_KEY('G');
static KeyInput &KEY_H = REGISTERED_KEY('H');
static KeyInput &KEY_TAB = REGISTERED_KEY(VK_TAB);
static KeyInput &KEY_SPACE = REGISTERED_KEY(VK_SPACE);
static KeyInput &KEY_SHIFT = REGISTERED_KEY(VK_SHIFT);
static KeyInput &KEY_CONTROL = REGISTERED_KEY(VK_CONTROL);
// 鼠标按键
static MouseInput &MOUSE_LEFT = REGISTERED_MOUSE(MouseButtonEnum::LEFT);
static MouseInput &MOUSE_RIGHT = REGISTERED_MOUSE(MouseButtonEnum::RIGHT);
static MouseInput &MOUSE_MIDDLE = REGISTERED_MOUSE(MouseButtonEnum::MIDDLE);
static MouseInput &MOUSE_SCROLL = REGISTERED_MOUSE(MouseButtonEnum::MIDDLE);

#endif //BAI2D_REGISTERED_INPUT_H

//
// Created by 22190 on 2024/12/4.
//

#ifndef BAI2D_UTILS_H
#define BAI2D_UTILS_H
#pragma once

#include <chrono>
#include "graphics.h"
#include <iostream>
#include <cmath>

#pragma comment (lib, "Gdiplus.lib")

class TimeUtils {
public:
    static long long getNowMilliseconds() {
        auto currentTimePoint = std::chrono::system_clock::now();
        auto durationSinceEpoch = currentTimePoint.time_since_epoch();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(durationSinceEpoch);
        return milliseconds.count();
    }
};

class BaiMath {
public:
    static int getNumSign(int num) {
        return num > 0 ? 1 : num < 0 ? -1 : 0;
    }
};

class EasyXUtils {
public:
    static IMAGE mirrorX(IMAGE &originImg) {
        IMAGE targetImg(originImg.getwidth(), originImg.getheight());
        targetImg.Resize(originImg.getwidth(), originImg.getheight());
        DWORD *originBuffer = GetImageBuffer(&originImg);
        DWORD *targetBuffer = GetImageBuffer(&targetImg);
        for (int i = 0; i < originImg.getheight(); i++) {
            for (int j = 0; j < originImg.getwidth(); j++) {
                targetBuffer[i * originImg.getwidth() + j] = originBuffer[i * originImg.getwidth() +
                                                                          (originImg.getwidth() - j - 1)];
            }
        }
        return targetImg;
    }

    static IMAGE mirrorY(IMAGE &originImg) {
        IMAGE targetImg(originImg.getwidth(), originImg.getheight());
        targetImg.Resize(originImg.getwidth(), originImg.getheight());
        DWORD *originBuffer = GetImageBuffer(&originImg);
        DWORD *targetBuffer = GetImageBuffer(&targetImg);
        for (int i = 0; i < originImg.getheight(); i++) {
            for (int j = 0; j < originImg.getwidth(); j++) {
                targetBuffer[i * originImg.getwidth() + j] = originBuffer[
                        (originImg.getheight() - i - 1) * originImg.getwidth() + j];
            }
        }
        return targetImg;
    }

    static IMAGE scaleImage(IMAGE &originImg, double scale) {
        if (scale == 1)return originImg;
        IMAGE targetImg(int(originImg.getwidth() * scale), int(originImg.getheight() * scale));
        DWORD *originBuffer = GetImageBuffer(&originImg);
        DWORD *targetBuffer = GetImageBuffer(&targetImg);
        // 将原图像像素绘制到目标图像对应放大后的坐标位置
        for (int i = 0; i < targetImg.getheight(); i++) {
            for (int j = 0; j < targetImg.getwidth(); j++) {
                targetBuffer[i * targetImg.getwidth() + j] = originBuffer[
                        (int) (i / scale) * originImg.getwidth() + (int) (j / scale)];
            }
        }
        return targetImg;
    }

    static void
    putImageAlpha(POINT position, int dw, int dh, IMAGE &originImg, bool isMirrorX = false, bool isMirrorY = false) {
        std::cout << double(dw) / originImg.getwidth() << std::endl;
        IMAGE imgTemp(originImg);
        if (isMirrorX) imgTemp = mirrorX(originImg);
        if (isMirrorY) imgTemp = mirrorY(imgTemp);
        imgTemp = scaleImage(imgTemp, double(dw) / originImg.getwidth());
        IMAGE maskImg;
        DWORD *d1;
        maskImg = imgTemp;
        d1 = GetImageBuffer(&maskImg);
        float h, s, l;
        for (int i = 0; i < maskImg.getheight() * maskImg.getwidth(); i++) {
            RGBtoHSL(BGR(d1[i]), &h, &s, &l);
            if (l < 0.03) {
                d1[i] = BGR(WHITE);
            }
            if (d1[i] != BGR(WHITE)) {
                d1[i] = 0;
            }
        }
        putimage(position.x - dw / 2, position.y - dh / 2, dw, dh, &maskImg, 0, 0, SRCAND);
        putimage(position.x - dw / 2, position.y - dh / 2, dw, dh, &imgTemp, 0, 0, SRCPAINT);
    }

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
};

class StrUtils {
public:
    static std::string numToStrFillZero(int num, int len) {
        std::string str = std::to_string(num);
        while (str.length() < len) {
            str = str.insert(0, "0");
        }
        return str;
    }
};

#endif //BAI2D_UTILS_H

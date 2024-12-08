//
// Created by 22190 on 2024/12/8.
//

#ifndef BAI2D_SHORTCUT_FUNC_H
#define BAI2D_SHORTCUT_FUNC_H

#include "bai2d_mesh.h"
#include "bai2d_asset.h"

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

#endif //BAI2D_SHORTCUT_FUNC_H

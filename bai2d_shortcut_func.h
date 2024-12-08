//
// Created by 22190 on 2024/12/8.
//

#ifndef BAI2D_SHORTCUT_FUNC_H
#define BAI2D_SHORTCUT_FUNC_H

#include "bai2d_mesh.h"
#include "bai2d_asset.h"

MultiImageMesh *
getMultiImageMesh(const std::string &dirName, const std::string &fileNamePrefix, int endIndex, int width, int height,
                  int playInterval, bool isLoop = true) {
    auto *pMesh = new MultiImageMesh(width, height);
    pMesh->setPlayInterval(playInterval)->setIsLoop(isLoop);
    for (int i = 0; i <= endIndex; i++) {
        std::string path = dirName + "/";
        std::string name = fileNamePrefix;
        name += StrUtils::numToStrFillZero(i, 3) += ".png";
        path += name;
        std::cout << path << std::endl;
        GlobalAssetManager::getInstance().registeredImageAsset(name, path, width, height);
        pMesh->addAsset(name);
    }
    return pMesh;
}

#endif //BAI2D_SHORTCUT_FUNC_H

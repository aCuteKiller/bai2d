//
// Created by 22190 on 2024/12/7.
//

#ifndef BAI2D_ASSET_H
#define BAI2D_ASSET_H

#pragma once;

#include <vector>
#include <set>
#include "graphics.h"
#include "string"
#include "bai2d_constants.h"
#include "bai2d_utils.h"

class Asset {
protected:
    std::string name;
    long long createTime;
    long long updateTIme;

    static std::string getLegitimateName(const std::string &name);

public:
    explicit Asset(const std::string &name);

    Asset(const Asset &asset);

    Asset *setName(const std::string &n);

    [[nodiscard]] std::string getName() const;

    [[nodiscard]] long long getCreateTime() const;

    [[nodiscard]] long long getUpdateTime() const;

    template<typename T>
    typename std::enable_if<std::is_base_of<Asset, T>::value, T>::type *
    castTo() {
        try {
            return (T *) this;
        } catch (std::bad_cast &e) {
            std::cout << "error: " << e.what() << std::endl;
            throw std::runtime_error(e.what());
        }
    }
};

class ImageAsset : public Asset {
protected:
    IMAGE *image;
    LPCTSTR pImgFile;
    int nWidth = 0;
    int nHeight = 0;
public:
    explicit ImageAsset(const std::string &name,
                        LPCTSTR pImgFile,
                        int nWidth = 0,
                        int nHeight = 0,
                        bool bResize = false);

    virtual ~ImageAsset();

    [[nodiscard]] IMAGE *getImage() const;

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] int getHeight() const;

    [[nodiscard]] LPCTSTR getFilePath() const;
};

class AssetManager {
private:
    std::vector<Asset *> assets;
public:
    AssetManager();

    ~AssetManager();

    AssetManager *addAsset(Asset *asset);

    Asset *getAsset(const std::string &name);

    void removeAsset(const std::string &name);

    void removeAllAssets();

    std::vector<Asset *>::iterator begin();

    std::vector<Asset *>::iterator end();

    bool isAssetExist(const std::string &name);

    Asset *getAsset(int index);

    bool isEmpty();

    unsigned long long size();
};

class GlobalAssetManager {
private:
    static GlobalAssetManager *instance;
    AssetManager assetManager;

    GlobalAssetManager();

public:
    static GlobalAssetManager &getInstance();

    void registeredImageAsset(const std::string &name,
                              LPCTSTR pImgFile,
                              int nWidth = 0,
                              int nHeight = 0,
                              bool bResize = false);

    ImageAsset &getImageAsset(const std::string &name);
};


#endif //BAI2D_ASSET_H

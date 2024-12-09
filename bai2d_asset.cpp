//
// Created by 22190 on 2024/12/7.
//

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include "bai2d_asset.h"


Asset::Asset(const std::string &name) {
    std::string newName = getLegitimateName(name);
    this->name = name;
    this->createTime = this->updateTIme = TimeUtils::getNowMilliseconds();
}

std::string Asset::getLegitimateName(const std::string &name) {
    return name.substr(0, name.size() > MAX_ASSET_NAME_LENGTH ? MAX_ASSET_NAME_LENGTH : name.size());
}

Asset *Asset::setName(const std::string &n) {
    std::string newName = getLegitimateName(n);
    this->name = n;
    this->updateTIme = TimeUtils::getNowMilliseconds();
    return this;
}

std::string Asset::getName() const {
    return this->name;
}

long long Asset::getCreateTime() const {
    return this->createTime;
}

long long Asset::getUpdateTime() const {
    return this->updateTIme;
}

Asset::Asset(const Asset &asset) {
    this->name = asset.name;
    this->createTime = asset.createTime;
    this->updateTIme = asset.updateTIme;
}

ImageAsset::ImageAsset(const std::string &name, const std::string &pImgFile, int nWidth, int nHeight)
        : Asset(name) {
    this->nHeight = nHeight;
    this->nWidth = nWidth;
    this->pImgFile = pImgFile;
    this->image = new IMAGE();
    loadimage(this->image, pImgFile.c_str(), nWidth, nHeight, false);
}

int ImageAsset::getWidth() const {
    return this->nWidth;
}

int ImageAsset::getHeight() const {
    return this->nHeight;
}

std::string ImageAsset::getFilePath() const {
    return this->pImgFile;
}

ImageAsset::~ImageAsset() = default;

IMAGE *ImageAsset::getImage() const {
    return this->image;
}

ImageAsset *ImageAsset::resizeImage(int w, int h) {
    this->nHeight = w;
    this->nWidth = h;
    loadimage(this->image, this->pImgFile.c_str(), w, h, true);
    return this;
}

AssetManager *AssetManager::addAsset(Asset *asset) {
    if (!this->isAssetExist(asset->getName()))
        this->assets.push_back(asset);
    return this;
}

Asset *AssetManager::getAsset(const std::string &name) {
    for (auto asset: this->assets) {
        if (asset->getName() == name)
            return asset;
    }
    throw std::invalid_argument("Asset not found");
}

void AssetManager::removeAsset(const std::string &name) {
    for (auto asset: this->assets) {
        if (asset->getName() == name) {
            this->assets.push_back(asset);
            return;
        }
    }
}

void AssetManager::removeAllAssets() {
    this->assets.clear();
}

bool AssetManager::isAssetExist(const std::string &name) {
    return std::any_of(this->assets.begin(), this->assets.end(), [&name](const Asset *asset) {
        return asset->getName() == name;
    });
}

std::vector<Asset *>::iterator AssetManager::begin() {
    return this->assets.begin();
}

std::vector<Asset *>::iterator AssetManager::end() {
    return this->assets.end();
}

AssetManager::AssetManager() {
    this->assets = std::vector<Asset *>();
}

AssetManager::~AssetManager() {
    for (auto asset: this->assets) {
        delete asset;
    }
}

bool AssetManager::isEmpty() {
    return this->assets.empty();
}

Asset *AssetManager::getAsset(int index) {
    return this->assets[index];
}

unsigned long long AssetManager::size() {
    return assets.size();
}

GlobalAssetManager::GlobalAssetManager() {
    this->assetManager = AssetManager();
}

GlobalAssetManager &GlobalAssetManager::getInstance() {
    if (!GlobalAssetManager::instance)
        GlobalAssetManager::instance = new GlobalAssetManager();
    return *GlobalAssetManager::instance;
}

GlobalAssetManager *GlobalAssetManager::instance = nullptr;

void GlobalAssetManager::registeredImageAsset(const std::string &name, const std::string& pImgFile, int nWidth, int nHeight) {
    this->assetManager.addAsset(new ImageAsset(name, pImgFile, nWidth, nHeight));
}

ImageAsset &GlobalAssetManager::getImageAsset(const std::string &name) {
    try {
        return (ImageAsset &) (*this->assetManager.getAsset(name));
    } catch (std::invalid_argument &e) {
        throw std::invalid_argument("ImageAsset not found");
    }
}

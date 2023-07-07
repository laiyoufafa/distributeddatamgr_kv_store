/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef VIRTUAL_ASSETLOADER_H
#define VIRTUAL_ASSETLOADER_H
#include <mutex>
#include "iAssetLoader.h"

namespace DistributedDB {
class VirtualAssetLoader : public IAssetLoader {
public:
    VirtualAssetLoader() = default;
    ~VirtualAssetLoader() override = default;

    DBStatus Download(const std::string &tableName, const std::string &gid, const Type &prefix,
        std::map<std::string, Assets> &assets) override;

    DBStatus RemoveLocalAssets(const std::vector<Asset> &assets) override;

    void SetDownloadStatus(DBStatus status);
private:
    std::mutex dataMutex_;
    DBStatus downloadStatus_ = OK;
};
}
#endif // VIRTUAL_ASSETLOADER_H

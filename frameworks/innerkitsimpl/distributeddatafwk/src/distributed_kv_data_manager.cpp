/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#define LOG_TAG "DistributedKvDataManager"
#include "distributed_kv_data_manager.h"

#include "communication_provider.h"
#include "constant.h"
#include "dds_trace.h"
#include "device_status_change_listener_client.h"
#include "ikvstore_data_service.h"
#include "kvstore_service_death_notifier.h"
#include "log_print.h"
#include "refbase.h"
#include "store_manager.h"

namespace OHOS {
namespace DistributedKv {
using namespace OHOS::DistributedDataDfx;
DistributedKvDataManager::DistributedKvDataManager()
{}

DistributedKvDataManager::~DistributedKvDataManager()
{}

Status DistributedKvDataManager::GetSingleKvStore(const Options &options, const AppId &appId, const StoreId &storeId,
                                                  std::shared_ptr<SingleKvStore> &singleKvStore)
{
    DdsTrace trace(std::string(LOG_TAG "::") + std::string(__FUNCTION__),
        TraceSwitch::BYTRACE_ON | TraceSwitch::TRACE_CHAIN_ON);

    singleKvStore = nullptr;
    if (!storeId.IsValid()) {
        ZLOGE("invalid storeId.");
        return Status::INVALID_ARGUMENT;
    }
    if (options.baseDir.empty()) {
        ZLOGE("base dir empty.");
        return Status::INVALID_ARGUMENT;
    }
    KvStoreServiceDeathNotifier::SetAppId(appId);

    Status status = Status::INVALID_ARGUMENT;
    singleKvStore = StoreManager::GetInstance().GetKVStore(appId, storeId, options, status);
    return status;
}

Status DistributedKvDataManager::GetAllKvStoreId(const AppId &appId, std::vector<StoreId> &storeIds)
{
    DdsTrace trace(std::string(LOG_TAG "::") + std::string(__FUNCTION__));

    KvStoreServiceDeathNotifier::SetAppId(appId);
    return StoreManager::GetInstance().GetStoreIds(appId, storeIds);
}

Status DistributedKvDataManager::CloseKvStore(const AppId &appId, const StoreId &storeId)
{
    DdsTrace trace(std::string(LOG_TAG "::") + std::string(__FUNCTION__),
        TraceSwitch::BYTRACE_ON | TraceSwitch::TRACE_CHAIN_ON);

    KvStoreServiceDeathNotifier::SetAppId(appId);
    if (!storeId.IsValid()) {
        ZLOGE("invalid storeId.");
        return Status::INVALID_ARGUMENT;
    }
    KvStoreServiceDeathNotifier::SetAppId(appId);

    return StoreManager::GetInstance().CloseKVStore(appId, storeId);
}

Status DistributedKvDataManager::CloseKvStore(const AppId &appId, std::shared_ptr<SingleKvStore> &kvStorePtr)
{
    DdsTrace trace(std::string(LOG_TAG "::") + std::string(__FUNCTION__),
        TraceSwitch::BYTRACE_ON | TraceSwitch::TRACE_CHAIN_ON);

    if (kvStorePtr == nullptr) {
        ZLOGE("kvStorePtr is nullptr.");
        return Status::INVALID_ARGUMENT;
    }
    KvStoreServiceDeathNotifier::SetAppId(appId);
    StoreId storeId = kvStorePtr->GetStoreId();
    kvStorePtr = nullptr;

    return StoreManager::GetInstance().CloseKVStore(appId, storeId);
}

Status DistributedKvDataManager::CloseAllKvStore(const AppId &appId)
{
    DdsTrace trace(std::string(LOG_TAG "::") + std::string(__FUNCTION__),
        TraceSwitch::BYTRACE_ON | TraceSwitch::TRACE_CHAIN_ON);

    KvStoreServiceDeathNotifier::SetAppId(appId);
    return StoreManager::GetInstance().CloseAllKVStore(appId);
}

Status DistributedKvDataManager::DeleteKvStore(const AppId &appId, const StoreId &storeId, const std::string &path)
{
    DdsTrace trace(std::string(LOG_TAG "::") + std::string(__FUNCTION__),
        TraceSwitch::BYTRACE_ON | TraceSwitch::TRACE_CHAIN_ON);

    if (!storeId.IsValid()) {
        ZLOGE("invalid storeId.");
        return Status::INVALID_ARGUMENT;
    }
    if (path.empty()) {
        ZLOGE("path empty");
        return Status::INVALID_ARGUMENT;
    }
    KvStoreServiceDeathNotifier::SetAppId(appId);

    return StoreManager::GetInstance().Delete(appId, storeId, path);
}

Status DistributedKvDataManager::DeleteAllKvStore(const AppId &appId, const std::string &path)
{
    DdsTrace trace(std::string(LOG_TAG "::") + std::string(__FUNCTION__),
        TraceSwitch::BYTRACE_ON | TraceSwitch::TRACE_CHAIN_ON);
    if (path.empty()) {
        ZLOGE("path empty");
        return Status::INVALID_ARGUMENT;
    }
    KvStoreServiceDeathNotifier::SetAppId(appId);

    std::vector<StoreId> storeIds;
    Status status = GetAllKvStoreId(appId, storeIds);
    if (status != SUCCESS) {
        return status;
    }
    for (auto &storeId : storeIds) {
        status = StoreManager::GetInstance().Delete(appId, storeId, path);
        if (status != SUCCESS) {
            return status;
        }
    }
    return SUCCESS;
}

void DistributedKvDataManager::RegisterKvStoreServiceDeathRecipient(
    std::shared_ptr<KvStoreDeathRecipient> kvStoreDeathRecipient)
{
    ZLOGD("begin");
    if (kvStoreDeathRecipient == nullptr) {
        ZLOGW("Register KvStoreService Death Recipient input is null.");
        return;
    }
    KvStoreServiceDeathNotifier::AddServiceDeathWatcher(kvStoreDeathRecipient);
}

void DistributedKvDataManager::UnRegisterKvStoreServiceDeathRecipient(
    std::shared_ptr<KvStoreDeathRecipient> kvStoreDeathRecipient)
{
    ZLOGD("begin");
    if (kvStoreDeathRecipient == nullptr) {
        ZLOGW("UnRegister KvStoreService Death Recipient input is null.");
        return;
    }
    KvStoreServiceDeathNotifier::RemoveServiceDeathWatcher(kvStoreDeathRecipient);
}

Status DistributedKvDataManager::GetLocalDevice(DeviceInfo &localDevice)
{
    sptr<IKvStoreDataService> kvDataServiceProxy = KvStoreServiceDeathNotifier::GetDistributedKvDataService();
    if (kvDataServiceProxy == nullptr) {
        ZLOGE("proxy is nullptr.");
        return Status::ERROR;
    }

    return kvDataServiceProxy->GetLocalDevice(localDevice);
}

Status DistributedKvDataManager::GetDeviceList(std::vector<DeviceInfo> &deviceInfoList, DeviceFilterStrategy strategy)
{
    sptr<IKvStoreDataService> kvDataServiceProxy = KvStoreServiceDeathNotifier::GetDistributedKvDataService();
    if (kvDataServiceProxy == nullptr) {
        ZLOGE("proxy is nullptr.");
        return Status::ERROR;
    }

    return kvDataServiceProxy->GetRemoteDevices(deviceInfoList, strategy);
}

static std::map<DeviceStatusChangeListener *, sptr<IDeviceStatusChangeListener>> deviceObservers_;
static std::mutex deviceObserversMapMutex_;
Status DistributedKvDataManager::StartWatchDeviceChange(std::shared_ptr<DeviceStatusChangeListener> observer)
{
    sptr<DeviceStatusChangeListenerClient> ipcObserver = new(std::nothrow) DeviceStatusChangeListenerClient(observer);
    if (ipcObserver == nullptr) {
        ZLOGW("new DeviceStatusChangeListenerClient failed");
        return Status::ERROR;
    }
    sptr<IKvStoreDataService> kvDataServiceProxy = KvStoreServiceDeathNotifier::GetDistributedKvDataService();
    if (kvDataServiceProxy == nullptr) {
        ZLOGE("proxy is nullptr.");
        return Status::ERROR;
    }
    Status status = kvDataServiceProxy->StartWatchDeviceChange(ipcObserver, observer->GetFilterStrategy());
    if (status == Status::SUCCESS) {
        {
            std::lock_guard<std::mutex> lck(deviceObserversMapMutex_);
            deviceObservers_.insert({observer.get(), ipcObserver});
        }
        return Status::SUCCESS;
    }
    ZLOGE("watch failed.");
    return Status::ERROR;
}

Status DistributedKvDataManager::StopWatchDeviceChange(std::shared_ptr<DeviceStatusChangeListener> observer)
{
    sptr<IKvStoreDataService> kvDataServiceProxy = KvStoreServiceDeathNotifier::GetDistributedKvDataService();
    if (kvDataServiceProxy == nullptr) {
        ZLOGE("proxy is nullptr.");
        return Status::ERROR;
    }
    std::lock_guard<std::mutex> lck(deviceObserversMapMutex_);
    auto it = deviceObservers_.find(observer.get());
    if (it == deviceObservers_.end()) {
        ZLOGW(" not start watch device change.");
        return Status::ERROR;
    }
    Status status = kvDataServiceProxy->StopWatchDeviceChange(it->second);
    if (status == Status::SUCCESS) {
        deviceObservers_.erase(it->first);
    } else {
        ZLOGW("stop watch failed code=%d.", static_cast<int>(status));
    }
    return status;
}
}  // namespace DistributedKv
}  // namespace OHOS

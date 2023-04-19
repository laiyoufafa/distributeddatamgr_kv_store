/*
* Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef DISTRIBUTED_DATA_TASK_EXECUTOR_H
#define DISTRIBUTED_DATA_TASK_EXECUTOR_H
#include <map>

#include "executor_pool.h"
namespace OHOS {
class TaskExecutor {
public:
    using TaskId = ExecutorPool::TaskId;
    using Task = std::function<void()>;
    using Duration = std::chrono::steady_clock::duration;
    static constexpr TaskId INVALID_TASK_ID = static_cast<uint64_t>(0l);
    static constexpr Duration INVALID_DURATION = std::chrono::milliseconds(0);
    static constexpr uint64_t UNLIMITED_TIMES = std::numeric_limits<uint64_t>::max();

    static TaskExecutor &GetInstance()
    {
        static TaskExecutor instance;
        return instance;
    }

    TaskId Execute(const Task &task, Duration delay = INVALID_DURATION)
    {
        if (pool_ == nullptr) {
            return INVALID_TASK_ID;
        }
        return pool_->Execute(task, delay);
    }

    TaskId Schedule(const Task &task, Duration interval)
    {
        return Schedule(task, interval, INVALID_DURATION, UNLIMITED_TIMES);
    }

    TaskId Schedule(const Task &task, Duration interval, Duration delay, uint64_t times = UNLIMITED_TIMES)
    {
        if (pool_ == nullptr) {
            return INVALID_TASK_ID;
        }
        return pool_->Schedule(task, delay, interval, times);
    }

    bool Remove(TaskId taskId, bool wait = false)
    {
        if (pool_ == nullptr) {
            return false;
        }
        return pool_->Remove(taskId, wait);
    }

    TaskId Reset(TaskId taskId, Duration interval, Duration delay = INVALID_DURATION)
    {
        if (pool_ == nullptr) {
            return INVALID_TASK_ID;
        }
        return pool_->Reset(taskId, delay, interval);
    }

private:
    size_t MAX_THREADS = 12;
    size_t MIN_THREADS = 5;
    TaskExecutor()
    {
        pool_ = std::make_shared<ExecutorPool>(MAX_THREADS, MIN_THREADS);
    }

    ~TaskExecutor()
    {
        pool_ = nullptr;
    }

    std::shared_ptr<ExecutorPool> pool_;
};
} // namespace OHOS::DistributedKv
#endif // DISTRIBUTED_DATA_TASK_EXECUTOR_H

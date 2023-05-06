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

#include "task_executor.h"

namespace OHOS {
TaskExecutor::TaskExecutor()
{
    pool_ = std::make_shared<ExecutorPool>(MAX_THREADS, MIN_THREADS);
}

TaskExecutor::~TaskExecutor()
{
    pool_ = nullptr;
}

TaskExecutor &TaskExecutor::GetInstance()
{
    static TaskExecutor instance;
    return instance;
};

TaskExecutor::TaskId TaskExecutor::Execute(const Task &task)
{
    if (pool_ == nullptr) {
        return INVALID_TASK_ID;
    }
    return pool_->Execute(task);
}

TaskExecutor::TaskId TaskExecutor::Execute(const Task &task, Duration delay)
{
    if (pool_ == nullptr) {
        return INVALID_TASK_ID;
    }
    return pool_->Execute(task, delay);
}

TaskExecutor::TaskId TaskExecutor::Schedule(const Task &task, Duration interval)
{
    return Schedule(task, interval, INVALID_DURATION, UNLIMITED_TIMES);
}

TaskExecutor::TaskId TaskExecutor::Schedule(const Task &task, Duration interval, Duration delay, uint64_t times)
{
    if (pool_ == nullptr) {
        return INVALID_TASK_ID;
    }
    return pool_->Schedule(task, delay, interval, times);
}

bool TaskExecutor::Remove(TaskExecutor::TaskId taskId, bool wait)
{
    if (pool_ == nullptr) {
        return false;
    }
    return pool_->Remove(taskId, wait);
}

TaskExecutor::TaskId TaskExecutor::Reset(TaskExecutor::TaskId taskId, Duration interval)
{
    if (pool_ == nullptr) {
        return INVALID_TASK_ID;
    }
    return pool_->Reset(taskId, interval);
}
} // namespace OHOS

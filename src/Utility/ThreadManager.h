#pragma once
#include <thread>
class ThreadManager {
public:
    ThreadManager() : isFinished(false) {}

    template <typename Function, typename... Args>
    void StartAsyncTask(Function&& func, Args&&... args) {
        asyncThread = std::thread([this, func = std::forward<Function>(func), args = std::make_tuple(std::forward<Args>(args)...)] {
            std::apply(func, args);
            SetFinished(true);
        });
    }

    bool CheckFinished() {
        return isFinished;
    }

    void WaitForCompletion() {
        if (asyncThread.joinable()) {
            asyncThread.join();
        }
    }

private:
    std::thread asyncThread;
    bool isFinished;

    void SetFinished(bool value) {
        isFinished = value;
    }
};
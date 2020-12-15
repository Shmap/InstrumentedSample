#pragma once

#include "stdafx.h"
#include "IttMacro.h"

#include <atomic>

class InstrumentedWorker
{
public:
    InstrumentedWorker() = default;
    ~InstrumentedWorker() = default;

    void StartWorking();
    void StopWorking();

private:
    void SimpleFunction();

    std::atomic<bool> m_is_stopped = false;
};
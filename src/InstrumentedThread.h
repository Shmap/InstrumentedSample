#pragma once

#include "stdafx.h"
#include "IttMacro.h"

#include <atomic>

class InstrumentedWorker
{
public:
    InstrumentedWorker();
    ~InstrumentedWorker() = default;

    void StartWorking();
    void StopWorking();

private:
    void WaitAndBusyThread(uint32_t milliseconds);

    void FirstFunction();
    void SecondFunction(uint32_t number);
    void FunctionWithAllMetadata();

    std::atomic<bool>      m_is_stopped = false;
    Itt::Counter<uint32_t> m_counter;
};
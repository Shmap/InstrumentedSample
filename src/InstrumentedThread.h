#pragma once

#include "stdafx.h"
#include "IttMacro.h"

#include <atomic>
#include <ittnotify.h>

class CustomClock
{
public:
    CustomClock();
    __itt_clock_domain* GetClockDomain() { return m_clock_domain; }

private:
    __itt_clock_domain* m_clock_domain;
};

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
    void RegionFunction();
    void FunctionWithAllMetadata();

    std::atomic<bool>      m_is_stopped = false;
    Itt::Counter<uint32_t> m_counter;
    CustomClock            m_custom_clock;
};
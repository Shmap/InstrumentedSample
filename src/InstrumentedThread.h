#pragma once

#include "stdafx.h"
#include "IttMacro.h"

#include <atomic>
#include <ittnotify.h>

namespace DefaultMetadata
{
    constexpr const double      metadata_double = 100.100;
    constexpr const float       metadata_float = 100.1f;
    constexpr const int16_t     metadata_int16_t = -100;
    constexpr const uint16_t    metadata_uint16_t = 100u;
    constexpr const int32_t     metadata_int32_t = -100;
    constexpr const uint32_t    metadata_uint32_t = 100;
    constexpr const int64_t     metadata_int64_t = -100;
    constexpr const uint64_t    metadata_uint64_t = 100u;
    constexpr const char*       metadata_string = "metadata string";
}

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
    void SecondFunctionLeft();
    void SecondFunctionRight();
    void RegionFunction();
    void FunctionWithAllMetadata();
    void FunctionWithRelation(__itt_id parent_task_id);

    std::atomic<bool>      m_is_stopped = false;
    Itt::Counter<uint64_t> m_counter;
    CustomClock            m_custom_clock;
};
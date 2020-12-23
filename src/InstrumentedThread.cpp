#include "InstrumentedThread.h"
#include <ittnotify.h>

CustomClock::CustomClock()
{
    m_clock_domain = __itt_clock_domain_create([](__itt_clock_info* clock_info, void* data)
    {
        clock_info->clock_base = 0u;
        clock_info->clock_freq = 500u;
    }, nullptr);
}

InstrumentedWorker::InstrumentedWorker()
    : m_counter("Counter 10-5", g_default_itt_domain_name)
{
}

void InstrumentedWorker::StartWorking()
{
    ITT_THREAD_NAME("Instrumented thread");
    ITT_DOMAIN_INIT();
    while (!m_is_stopped)
    {
        ITT_MARKER(Itt::Marker::Scope::Thread, "Stack delimiter");
        ITT_MARKER_ARG("double",   DefaultMetadata::metadata_double);
        ITT_MARKER_ARG("float",    DefaultMetadata::metadata_float);
        ITT_MARKER_ARG("int16_t",  DefaultMetadata::metadata_int16_t);
        ITT_MARKER_ARG("uint16_t", DefaultMetadata::metadata_uint16_t);
        ITT_MARKER_ARG("int32_t",  DefaultMetadata::metadata_int32_t);
        ITT_MARKER_ARG("uint32_t", DefaultMetadata::metadata_uint32_t);
        ITT_MARKER_ARG("int64_t",  DefaultMetadata::metadata_int64_t);
        ITT_MARKER_ARG("uint64_t", DefaultMetadata::metadata_uint64_t);
        ITT_MARKER_ARG("string",   DefaultMetadata::metadata_string);

        FirstFunction();
    }
}

void InstrumentedWorker::StopWorking()
{
    m_is_stopped = true;
}

void InstrumentedWorker::WaitAndBusyThread(uint32_t milliseconds)
{
    uint64_t some_number = 0;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    while (true)
    {
        some_number++;
        if (std::chrono::steady_clock::now() - start > std::chrono::milliseconds(milliseconds))
            break;
    }
}

void InstrumentedWorker::FirstFunction()
{
    ITT_SCOPE_TASK_CUSTOM_CLOCK("First function on stack", m_custom_clock.GetClockDomain());
    static uint64_t number = 0;
    ITT_FUNCTION_ARG("Call No", number);
    m_counter.SetValue(10);

    RegionFunction();

    WaitAndBusyThread(250);
    SecondFunctionLeft();
    WaitAndBusyThread(250);
    SecondFunctionRight();
    WaitAndBusyThread(250);
    number++;
}

void InstrumentedWorker::SecondFunctionLeft()
{
    ITT_SCOPE_TASK("Second function on stack left");
    m_counter.SetValue(5);
    FunctionWithAllMetadata();
    WaitAndBusyThread(100);
}

void InstrumentedWorker::SecondFunctionRight()
{
    ITT_SCOPE_TASK("Second function on stack right");
    FunctionWithRelation(__itt_scope_item.GetId()); // TODO: do something with ITT_SCOPE macro to avoid uncler using of members created in macro
    WaitAndBusyThread(100);
}

void InstrumentedWorker::RegionFunction()
{
    ITT_SCOPE_REGION("Region 1");
    WaitAndBusyThread(100);
}

void InstrumentedWorker::FunctionWithAllMetadata()
{
    ITT_SCOPE_TASK("Metadata check");
    ITT_FUNCTION_ARG("double",   DefaultMetadata::metadata_double);
    ITT_FUNCTION_ARG("float",    DefaultMetadata::metadata_float);
    ITT_FUNCTION_ARG("int16_t",  DefaultMetadata::metadata_int16_t);
    ITT_FUNCTION_ARG("uint16_t", DefaultMetadata::metadata_uint16_t);
    ITT_FUNCTION_ARG("int32_t",  DefaultMetadata::metadata_int32_t);
    ITT_FUNCTION_ARG("uint32_t", DefaultMetadata::metadata_uint32_t);
    ITT_FUNCTION_ARG("int64_t",  DefaultMetadata::metadata_int64_t);
    ITT_FUNCTION_ARG("uint64_t", DefaultMetadata::metadata_uint64_t);
    ITT_FUNCTION_ARG("string",   DefaultMetadata::metadata_string);

    WaitAndBusyThread(100);
}

void InstrumentedWorker::FunctionWithRelation(__itt_id parent_task_id)
{
    ITT_SCOPE_TASK("Relation check");
    __itt_relation_add_to_current(__itt_domain_instance, __itt_relation::__itt_relation_is_child_of, parent_task_id);
    WaitAndBusyThread(100);
}

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
    while (!m_is_stopped)
    {
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
    //ITT_SCOPE_TASK("First function on stack");
    static uint64_t number = 0;
    ITT_FUNCTION_ARG("Call No", number);
    m_counter.SetValue(10);

    RegionFunction();

    WaitAndBusyThread(250);
    SecondFunction(1);
    WaitAndBusyThread(250);
    SecondFunction(2);
    WaitAndBusyThread(250);
    number++;

    // After supporting in GPU, remove metadata from here
    ITT_MARKER(Itt::Marker::Scope::Thread, "Thread scope marker");
    ITT_MARKER_ARG("Scope", "Thread");

    double      metadata_double   = 100.100;
    float       metadata_float    = 100.1f;
    int16_t     metadata_int16_t  = -100;
    uint16_t    metadata_uint16_t = 100u;
    int32_t     metadata_int32_t  = -100;
    uint32_t    metadata_uint32_t = 100;
    int64_t     metadata_int64_t  = -100;
    uint64_t    metadata_uint64_t = 100u;
    std::string metadata_string   = "metadata string";

    ITT_MARKER_ARG("double",   metadata_double);
    ITT_MARKER_ARG("float",    metadata_float);
    ITT_MARKER_ARG("int16_t",  metadata_int16_t);
    ITT_MARKER_ARG("uint16_t", metadata_uint16_t);
    ITT_MARKER_ARG("int32_t",  metadata_int32_t);
    ITT_MARKER_ARG("uint32_t", metadata_uint32_t);
    ITT_MARKER_ARG("int64_t",  metadata_int64_t);
    ITT_MARKER_ARG("uint64_t", metadata_uint64_t);
    ITT_MARKER_ARG("string",   metadata_string);
}

void InstrumentedWorker::SecondFunction(uint32_t number)
{
    ITT_SCOPE_TASK("Second function on stack");
    ITT_FUNCTION_ARG("Func No", number);
    m_counter.SetValue(5);
    FunctionWithAllMetadata();
    FunctionWithRelation(__itt_scope_item.GetId()); // TODO: do something with ITT_SCOPE_MACRO to avoid uncler using of members created in macro
    WaitAndBusyThread(100);
}

void InstrumentedWorker::RegionFunction()
{
    ITT_SCOPE_REGION("Region 1");
    WaitAndBusyThread(100);
}

void InstrumentedWorker::FunctionWithAllMetadata()
{
    ITT_SCOPE_TASK("Metadata checks");

    double      metadata_double   = 100.100;
    float       metadata_float    = 100.1f;
    int16_t     metadata_int16_t  = -100;
    uint16_t    metadata_uint16_t = 100u;
    int32_t     metadata_int32_t  = -100;
    uint32_t    metadata_uint32_t = 100;
    int64_t     metadata_int64_t  = -100;
    uint64_t    metadata_uint64_t = 100u;
    std::string metadata_string   = "metadata string";

    ITT_FUNCTION_ARG("double",   metadata_double);
    ITT_FUNCTION_ARG("float",    metadata_float);
    ITT_FUNCTION_ARG("int16_t",  metadata_int16_t);
    ITT_FUNCTION_ARG("uint16_t", metadata_uint16_t);
    ITT_FUNCTION_ARG("int32_t",  metadata_int32_t);
    ITT_FUNCTION_ARG("uint32_t", metadata_uint32_t);
    ITT_FUNCTION_ARG("int64_t",  metadata_int64_t);
    ITT_FUNCTION_ARG("uint64_t", metadata_uint64_t);
    ITT_FUNCTION_ARG("string",   metadata_string);

    WaitAndBusyThread(100);

    ITT_MARKER(Itt::Marker::Scope::Task, "Metadata task marker");
    ITT_MARKER_ARG("Scope", "Task");
    ITT_MARKER_ARG("double", metadata_double);
    ITT_MARKER_ARG("float", metadata_float);
    ITT_MARKER_ARG("int16_t", metadata_int16_t);
    ITT_MARKER_ARG("uint16_t", metadata_uint16_t);
    ITT_MARKER_ARG("int32_t", metadata_int32_t);
    ITT_MARKER_ARG("uint32_t", metadata_uint32_t);
    ITT_MARKER_ARG("int64_t", metadata_int64_t);
    ITT_MARKER_ARG("uint64_t", metadata_uint64_t);
    ITT_MARKER_ARG("string", metadata_string);
}

void InstrumentedWorker::FunctionWithRelation(__itt_id parent_task_id)
{
    ITT_SCOPE_TASK("Function with relation");
    __itt_relation_add_to_current(__itt_domain_instance, __itt_relation::__itt_relation_is_child_of, parent_task_id);
    WaitAndBusyThread(100);
}

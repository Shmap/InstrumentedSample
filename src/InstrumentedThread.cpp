#include "InstrumentedThread.h"
#include <ittnotify.h>

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
    ITT_SCOPE_TASK("First function on stack");
    static uint64_t number = 0;
    ITT_FUNCTION_ARG("Call No", number);

    WaitAndBusyThread(250);
    SecondFunction(1);
    WaitAndBusyThread(250);
    SecondFunction(2);
    WaitAndBusyThread(250);
    number++;
}

void InstrumentedWorker::SecondFunction(uint32_t number)
{
    ITT_SCOPE_TASK("Second function on stack");
    ITT_FUNCTION_ARG("Func No", number);
    FunctionWithLotOfMetadata();
    WaitAndBusyThread(100);
}

void InstrumentedWorker::FunctionWithLotOfMetadata()
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
}

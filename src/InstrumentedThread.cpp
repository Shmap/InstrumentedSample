#include "InstrumentedThread.h"
#include <ittnotify.h>

void InstrumentedWorker::StartWorking()
{
    ITT_THREAD_NAME("Instrumented thread");

    while (!m_is_stopped)
    {
        SimpleFunction();
    }
}

void InstrumentedWorker::StopWorking()
{
    m_is_stopped = true;
}

void InstrumentedWorker::SimpleFunction()
{
    ITT_SCOPE_TASK("Checking function");
    static uint32_t number = 0;
    ITT_FUNCTION_ARG("Call No", number);
    Sleep(200);
    number++;
}

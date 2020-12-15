/******************************************************************************

Copyright 2019-2020 Evgeny Gorodetskiy

Licensed under the Apache License, Version 2.0 (the "License"),
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*******************************************************************************/

#pragma once

#include <stdint.h>
#include <string>
#include <type_traits>
#include <thread>
#include <nowide/convert.hpp>

#define INTEL_ITTNOTIFY_API_PRIVATE
#include <ittnotify.h>

#include <stdint.h>

#ifdef _WIN32
#define UNICODE_AGNOSTIC(name) name##A
#else
#define UNICODE_AGNOSTIC(name) name
#endif

constexpr const char* g_default_itt_domain_name = "Instrumented Sample";

namespace Itt
{

    class Event
    {
    public:
        Event(const __itt_domain* p_domain, __itt_string_handle* p_name) noexcept
            : m_id(__itt_id_make(const_cast<__itt_domain*>(p_domain), reinterpret_cast<unsigned long long>(p_name)))
            , m_p_domain(p_domain)
        { }

        template<class T>
        typename std::enable_if<std::is_floating_point<T>::value, void>::type AddArg(__itt_string_handle* p_name, const T& value) const noexcept
        {
            double double_value = static_cast<double>(value);
            __itt_metadata_add(m_p_domain, m_id, p_name, __itt_metadata_double, 1, &double_value);
        }

        void AddArg(__itt_string_handle* p_name, int64_t value) const noexcept
        {
            __itt_metadata_add(m_p_domain, m_id, p_name, __itt_metadata_s64, 1, &value);
        }

        void AddArg(__itt_string_handle* p_name, const char* value) const noexcept
        {
#if ITT_PLATFORM==ITT_PLATFORM_WIN && (defined(UNICODE) || defined(_UNICODE))
            // string value must be converted to wchar_t
            __itt_metadata_str_add(m_p_domain, m_id, p_name, nowide::widen(value).c_str(), 0);
#else
            __itt_metadata_str_add(m_p_domain, m_id, p_name, value, 0);
#endif
        }

        void AddArg(__itt_string_handle* p_name, void const* const p_value) const noexcept
        {
            __itt_metadata_add(m_p_domain, m_id, p_name, __itt_metadata_unknown, 1, const_cast<void*>(p_value));
        }

        void AddArg(__itt_string_handle* p_name, const std::string& value) const noexcept
        {
            AddArg(p_name, value.c_str());
        }

    protected:
        __itt_id            m_id = __itt_null;
        const __itt_domain* m_p_domain;
    };

    class Marker : public Event
    {
    public:
        enum class Scope : int
        {
            Global = __itt_scope_global,
            Process = __itt_scope_track_group,
            Thread = __itt_scope_track,
            Task = __itt_scope_task, //means a task that will long until another marker with task scope in this thread occurs
        };

        Marker(const __itt_domain* p_domain, const char* p_name, Scope scope) noexcept
            : Marker(p_domain, UNICODE_AGNOSTIC(__itt_string_handle_create)(p_name), scope)
        { }

        void Notify() const noexcept
        {
            __itt_marker(m_p_domain, m_id, m_p_name, m_scope);
        }

    private:
        Marker(const __itt_domain* p_domain, __itt_string_handle* p_itt_name, Scope scope) noexcept
            : Event(p_domain, p_itt_name)
            , m_p_name(p_itt_name)
            , m_scope(static_cast<__itt_scope>(scope))
        { }

        __itt_string_handle* m_p_name;
        __itt_scope          m_scope;
    };

    template<bool bRegion = true>
    class Task : public Event
    {
    public:
        Task(const __itt_domain* p_domain, __itt_string_handle* p_name) noexcept
            : Event(p_domain, p_name)
        {
            if (bRegion)
            {
                __itt_region_begin(m_p_domain, m_id, __itt_null, p_name);
            }
            else
            {
                __itt_task_begin(m_p_domain, m_id, __itt_null, p_name);
            }
        }

        ~Task()
        {
            if (bRegion)
            {
                __itt_region_end(m_p_domain, m_id);
            }
            else
            {
                __itt_task_end(m_p_domain);
            }
        }
    };

    template<typename Type> inline __itt_metadata_type GetMetadataType(Type)     noexcept { return __itt_metadata_unknown; }
    template<>              inline __itt_metadata_type GetMetadataType(double)   noexcept { return __itt_metadata_double; }
    template<>              inline __itt_metadata_type GetMetadataType(float)    noexcept { return __itt_metadata_float; }
    template<>              inline __itt_metadata_type GetMetadataType(int16_t)  noexcept { return __itt_metadata_s16; }
    template<>              inline __itt_metadata_type GetMetadataType(uint16_t) noexcept { return __itt_metadata_u16; }
    template<>              inline __itt_metadata_type GetMetadataType(int32_t)  noexcept { return __itt_metadata_s32; }
    template<>              inline __itt_metadata_type GetMetadataType(uint32_t) noexcept { return __itt_metadata_u32; }
    template<>              inline __itt_metadata_type GetMetadataType(int64_t)  noexcept { return __itt_metadata_s64; }
    template<>              inline __itt_metadata_type GetMetadataType(uint64_t) noexcept { return __itt_metadata_u64; }

    template<typename ValueType>
    class Counter
    {
    public:
        Counter(const char* p_name, const char* p_domain) noexcept
            : m_id(UNICODE_AGNOSTIC(__itt_counter_create_typed)(p_name, p_domain, GetMetadataType(ValueType{})))
        { }
        Counter(Counter&& other) : m_id(std::move(other.m_id)) {}
        ~Counter() { __itt_counter_destroy(m_id); }

        void SetValue(ValueType value) const noexcept { __itt_counter_set_value(m_id, &value); }
        void IncrementDelta(uint64_t delta) const noexcept { __itt_counter_inc_delta(m_id, delta); }
        void DecrementDelta(uint64_t delta) const noexcept { __itt_counter_dec_delta(m_id, delta); }
        void Increment() const noexcept { __itt_counter_inc(m_id); }
        void Decrement() const noexcept { __itt_counter_inc(m_id); }

    private:
        __itt_counter m_id;
    };

#define ITT_DOMAIN_LOCAL(/*const char* */domain)\
    static const __itt_domain* __itt_domain_instance = UNICODE_AGNOSTIC(__itt_domain_create)(domain)

#define ITT_DOMAIN_GLOBAL(/*const char* */domain)\
    const char* __itt_domain_name = domain;\
    __itt_domain* __itt_domain_instance = nullptr

#define ITT_DOMAIN_EXTERN()\
    extern const char* __itt_domain_name;\
    extern __itt_domain* __itt_domain_instance

#define ITT_DOMAIN_INIT()\
    if (!__itt_domain_instance && __itt_domain_name)\
        __itt_domain_instance = UNICODE_AGNOSTIC(__itt_domain_create)(__itt_domain_name)

#define ITT_SCOPE(region, name)\
    static __itt_string_handle* __itt_scope_name = UNICODE_AGNOSTIC(__itt_string_handle_create)(name);\
    ITT_DOMAIN_INIT();\
    Itt::Task<region> __itt_scope_item(__itt_domain_instance, __itt_scope_name)

#define ITT_SCOPE_TASK(/*const char* */name) ITT_SCOPE(false, name)
#define ITT_SCOPE_REGION(/*const char* */name) ITT_SCOPE(true, name)

#define ITT_MARKER(/*Itt::Marker::Scope*/scope, /*const char* */name) \
    ITT_DOMAIN_INIT(); \
    static const Itt::Marker __itt_marker_item(__itt_domain_instance, name, scope); \
    __itt_marker_item.Notify()

#define ITT_ARG(item_variable, /*const char* */name, /*number or string*/ value) { \
    static __itt_string_handle* __itt_arg_name = UNICODE_AGNOSTIC(__itt_string_handle_create)(name); \
    item_variable.AddArg(__itt_arg_name, value); \
}

#define ITT_MARKER_ARG(/*const char* */name, /*number or string*/ value) \
    ITT_ARG(__itt_marker_item, name, value)

#define ITT_FUNCTION_ARG(/*const char* */name, /*number or string*/ value) \
    ITT_ARG(__itt_scope_item, name, value)

#define ITT_FUNCTION_TASK() \
    ITT_SCOPE_TASK(__FUNCTION__); \
    ITT_FUNCTION_ARG("__file__", __FILE__); \
    ITT_FUNCTION_ARG("__line__", __LINE__)

#define ITT_GLOBAL_MARKER(/*const char* */name) ITT_MARKER(Itt::Marker::Scope::Global, name)
#define ITT_PROCESS_MARKER(/*const char* */name) ITT_MARKER(Itt::Marker::Scope::Process, name)
#define ITT_THREAD_MARKER(/*const char* */name) ITT_MARKER(Itt::Marker::Scope::Thread, name)
#define ITT_TASK_MARKER(/*const char* */name) ITT_MARKER(Itt::Marker::Scope::Task, name)

#define ITT_FUNCTION_MARKER(/*Itt::Marker::Scope*/scope) ITT_MARKER(scope, __FUNCTION__);
#define ITT_FUNCTION_GLOBAL_MARKER() ITT_FUNCTION_MARKER(Itt::Marker::Scope::Global)
#define ITT_FUNCTION_PROCESS_MARKER() ITT_FUNCTION_MARKER(Itt::Marker::Scope::Process)
#define ITT_FUNCTION_THREAD_MARKER() ITT_FUNCTION_MARKER(Itt::Marker::Scope::Thread)
#define ITT_FUNCTION_TASK_MARKER() ITT_FUNCTION_MARKER(Itt::Marker::Scope::Task)

#define ITT_COUNTER_TYPE(value_type) Itt::Counter<value_type>
#define ITT_COUNTER_INIT(/*const char* */name, /*const char* */domain) name, domain
#define ITT_COUNTER_VALUE(counter_var, value) counter_var.SetValue(value)

    class ScopeTrack
    {
    public:
        ScopeTrack(__itt_track* track)
        {
            __itt_set_track(track);
        }
        ~ScopeTrack()
        {
            __itt_set_track(nullptr);
        }
    };

    //'group' defines virtual process (null means current process), track defines virtual thread
#define ITT_SCOPE_TRACK(/*const char* */group, /*const char* */ track)\
    static __itt_track* itt_track_name = __itt_track_create(__itt_track_group_create(((group) ? UNICODE_AGNOSTIC(__itt_string_handle_create)(group) : nullptr), __itt_track_group_type_normal), UNICODE_AGNOSTIC(__itt_string_handle_create)(track), __itt_track_type_normal);\
    Itt::ScopeTrack itt_track(itt_track_name);

#define ITT_THREAD_NAME(/*const char* */name) \
    UNICODE_AGNOSTIC(__itt_thread_set_name)(name)

} // namespace Itt

ITT_DOMAIN_EXTERN();

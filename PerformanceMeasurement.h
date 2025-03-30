#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>

enum class PerformanceMeasurementUnit
{
    NanoSecond  ,
    MicroSecond ,
    MilliSecond ,
    CentiSecond ,
    Second      ,
    Minute
};


#ifndef DO_NOT_MEASURE_PERFORMANCE

#define BENCHMARK_JSON_BEGIN_SEESION(name) BenchmarkJson::beginSession(name)
#define BENCHMARK_JSON_END_SEESION(name) BenchmarkJson::endSession()

// Helper macros for counting arguments
#define MEASURE_PERFORMANCE_AS_OBJECT_COUNT(...) \
MEASURE_PERFORMANCE_AS_OBJECT_COUNT_(__VA_ARGS__, 6, 5, 4, 3, 2, 1)
#define MEASURE_PERFORMANCE_AS_OBJECT_COUNT_(_1, _2, _3, _4, _5, _6, N, ...) N

// Macro dispatcher
#define MEASURE_PERFORMANCE_AS_OBJECT(...) \
MEASURE_PERFORMANCE_AS_OBJECT_CHOOSER(MEASURE_PERFORMANCE_AS_OBJECT_COUNT(__VA_ARGS__))(__VA_ARGS__)

// Individual cases
#define MEASURE_PERFORMANCE_AS_OBJECT_CHOOSER(N) \
    MEASURE_PERFORMANCE_AS_OBJECT_CHOOSER_(N)
#define MEASURE_PERFORMANCE_AS_OBJECT_CHOOSER_(N) \
    MEASURE_PERFORMANCE_AS_OBJECT_##N

// Case 1: Just event name (string)
#define MEASURE_PERFORMANCE_AS_OBJECT_1(eventName) \
    PerformanceMeasurement eventName(#eventName)

// Case 2: Event name + unit
#define MEASURE_PERFORMANCE_AS_OBJECT_2(eventName, unit) \
    PerformanceMeasurement eventName(#eventName, unit)

// Case 3: Event name + unit + save flag
#define MEASURE_PERFORMANCE_AS_OBJECT_3(eventName, unit, saveResultToJsonFile) \
PerformanceMeasurement eventName(#eventName, unit, saveResultToJsonFile)

// Case 4: Event name + unit + save flag + print flag
#define MEASURE_PERFORMANCE_AS_OBJECT_4(eventName, unit, saveResultToJsonFile, printLogOnDestruction) \
PerformanceMeasurement eventName(#eventName, unit, saveResultToJsonFile, printLogOnDestruction)

// Case 5: Unit + save flag + print flag (no event name)
#define MEASURE_PERFORMANCE_AS_OBJECT_5(unit, saveResultToJsonFile, printLogOnDestruction) \
PerformanceMeasurement eventName(unit, saveResultToJsonFile, printLogOnDestruction)

// Case 6: Event name + save flag + print flag (special case)
#define MEASURE_PERFORMANCE_AS_OBJECT_6(eventName, saveResultToJsonFile, printLogOnDestruction) \
PerformanceMeasurement eventName(#eventName, saveResultToJsonFile, printLogOnDestruction)


//

// Helper macros for unique variable names
#define CONCAT(a, b) CONCAT_(a, b)
#define CONCAT_(a, b) a##b

// Helper macros for counting arguments
#define MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_ARG_COUNT(...) \
MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_ARG_COUNT_(__VA_ARGS__, 6, 5, 4, 3, 2, 1)
#define MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_ARG_COUNT_(_1, _2, _3, _4, _5, _6, N, ...) N

// Macro for measuring entire scope performance
#define MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE(...) \
MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_CHOOSER(MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_ARG_COUNT(__VA_ARGS__))(__VA_ARGS__)

#define MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_CHOOSER(N) \
    MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_CHOOSER_(N)
#define MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_CHOOSER_(N) \
    MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_##N

// Scope measurement cases
#define MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_1(eventName) \
    PerformanceMeasurement CONCAT(_perf_meas_, __LINE__){eventName}

#define MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_2(eventName, unit) \
    PerformanceMeasurement CONCAT(_perf_meas_, __LINE__)(eventName, unit)

// TODO: MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_3 conflicts with MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_5 and MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_6
#define MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_3(eventName, unit, saveResult) \
    PerformanceMeasurement CONCAT(_perf_meas_, __LINE__)(eventName, unit, saveResult)

#define MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_4(eventName, unit, saveResult, printLog) \
    PerformanceMeasurement CONCAT(_perf_meas_, __LINE__)(eventName, unit, saveResult, printLog)

#define MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_5(unit, saveResult, printLog) \
    PerformanceMeasurement CONCAT(_perf_meas_, __LINE__)(unit, saveResult, printLog)

#define MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE_6(eventName, saveResult, printLog) \
    PerformanceMeasurement CONCAT(_perf_meas_, __LINE__)(eventName, saveResult, printLog)


//


#define MEASURE_PERFORMANCE_FOR_FUNCTION_CHOOSER(N) \
    MEASURE_PERFORMANCE_FOR_FUNCTION_CHOOSER_(N)
#define MEASURE_PERFORMANCE_FOR_FUNCTION_CHOOSER_(N) \
    MEASURE_PERFORMANCE_FOR_FUNCTION_##N

// Compiler detection
#if defined(_MSC_VER)
#define FUNCTION_NAME_WITH_SIGNATURE __FUNCSIG__
#elif defined(__GNUC__) || defined(__clang__)
#define FUNCTION_NAME_WITH_SIGNATURE __PRETTY_FUNCTION__
#else
#define FUNCTION_NAME_WITH_SIGNATURE __func__
#endif

#define GET_MACRO(_0, _1, _2, _3, NAME, ...) NAME

#define MEASURE_PERFORMANCE_FOR_FUNCTION(...) \
GET_MACRO(_0, ##__VA_ARGS__, \
          MEASURE_PERFORMANCE_FOR_FUNCTION_3, \
          MEASURE_PERFORMANCE_FOR_FUNCTION_2, \
          MEASURE_PERFORMANCE_FOR_FUNCTION_1, \
          MEASURE_PERFORMANCE_FOR_FUNCTION_0)(__VA_ARGS__)

#define MEASURE_PERFORMANCE_FOR_FUNCTION_0() \
PerformanceMeasurement CONCAT(_func_perf_, __LINE__)(FUNCTION_NAME_WITH_SIGNATURE)

#define MEASURE_PERFORMANCE_FOR_FUNCTION_1(unit) \
    PerformanceMeasurement CONCAT(_func_perf_, __LINE__)(FUNCTION_NAME_WITH_SIGNATURE, unit)

#define MEASURE_PERFORMANCE_FOR_FUNCTION_2(unit, saveResult) \
    PerformanceMeasurement CONCAT(_func_perf_, __LINE__)(FUNCTION_NAME_WITH_SIGNATURE, unit, saveResult)

#define MEASURE_PERFORMANCE_FOR_FUNCTION_3(unit, saveResult, printLog) \
    PerformanceMeasurement CONCAT(_func_perf_, __LINE__)(FUNCTION_NAME_WITH_SIGNATURE, unit, saveResult, printLog)

#else

#define MEASURE_PERFORMANCE_AS_OBJECT(varName, ...)
#define MEASURE_PERFORMANCE_FOR_ENTIRE_SCOPE(...)
#define MEASURE_PERFORMANCE_FOR_FUNCTION(...)
#define BENCHMARK_JSON_BEGIN_SEESION(name)
#define BENCHMARK_JSON_END_SEESION(name)

#endif

struct ProfileResult
{
    std::string& name;
    int64_t      dur;
    int64_t      start;
    int64_t      end;
    uint64_t     threadId;
};

class BenchmarkJson
{
    BenchmarkJson(const BenchmarkJson&)            = delete;
    BenchmarkJson& operator=(const BenchmarkJson&) = delete;
    BenchmarkJson(BenchmarkJson &&)                = delete;
    BenchmarkJson& operator=(BenchmarkJson &&)     = delete;
    BenchmarkJson()                                = delete;
    ~BenchmarkJson()                               = delete;

public:
    static void beginSession(const std::string& filepath = "results.json")
    {
        _outputStream.open(filepath);
        writeHeader();
    }

    static void endSession()
    {
        writeFooter();
        _outputStream.close();
        _profileCount = 0;
    }

    static void writeProfile(const ProfileResult& result)
    {
        if (_profileCount++ > 0)
        {
            _outputStream << ",";
        }

        std::replace(result.name.begin(), result.name.end(), '"', '\'');

        _outputStream << "{\"cat\":\"event\"";
        _outputStream << ",\"dur\":" << (result.dur);
        _outputStream << ",\"name\":\"" << result.name << "\"";
        _outputStream << ",\"ph\":\"X\"";
        _outputStream << ",\"pid\":0";
        _outputStream << ",\"tid\":" << result.threadId;
        _outputStream << ",\"ts\":" << result.start;
        _outputStream << ",\"beginTimePoint\":" << result.start;
        _outputStream << ",\"endTimePoint\":" << result.end;
        _outputStream << "}";

        _outputStream.flush();
    }

    static void writeHeader()
    {
        _outputStream << "{\"otherData\": {},\"traceEvents\":[";
        _outputStream.flush();
    }

    static void writeFooter()
    {
        _outputStream << "]}";
        _outputStream.flush();
    }

private:
    static uint32_t      _profileCount;
    static std::ofstream _outputStream;
};
uint32_t BenchmarkJson::_profileCount = 0;
std::ofstream BenchmarkJson::_outputStream{};


class PerformanceMeasurement
{
    PerformanceMeasurement(const PerformanceMeasurement&)            = delete;
    PerformanceMeasurement& operator=(const PerformanceMeasurement&) = delete;
    PerformanceMeasurement(PerformanceMeasurement &&)                = delete;
    PerformanceMeasurement& operator=(PerformanceMeasurement &&)     = delete;

private:
    inline void setEndTimePoint() noexcept
    {
        if(_isEndTimePointInitialized == false)
        {
            _end = std::chrono::high_resolution_clock::now();

            _isEndTimePointInitialized = true;
        }
    }

    inline auto getSpentTimeInMicroSecond() noexcept
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(_end - _begin).count();
    }

    inline auto getBeginTimePointInMicroSecond() const noexcept
    {
        return std::chrono::time_point_cast<std::chrono::microseconds>(_begin).time_since_epoch().count();
    }

    inline auto getEndTimePointInMicroSecond() const noexcept
    {
        return std::chrono::time_point_cast<std::chrono::microseconds>(_end).time_since_epoch().count();
    }

    inline std::string setUnitStr()
    {
        switch (_unit)
        {
            case PerformanceMeasurementUnit::NanoSecond:
            {
                return std::string("NanoSeconds");
            }
            case PerformanceMeasurementUnit::MicroSecond:
            {
                return std::string("MicroSeconds");
            }
            case PerformanceMeasurementUnit::MilliSecond:
            {
                return std::string("MilliSeconds");
            }
            case PerformanceMeasurementUnit::CentiSecond:
            {
                return std::string("CentiSeconds");
            }
            case PerformanceMeasurementUnit::Second:
            {
                return std::string("Seconds");
            }
            case PerformanceMeasurementUnit::Minute:
            {
                return std::string("Minutes");
            }
        }
    }

public:
    inline PerformanceMeasurement(std::string&& eventName , PerformanceMeasurementUnit unit = PerformanceMeasurementUnit::MicroSecond , bool saveResultToJsonFile = false , bool printLogOnDestruction = true) : PerformanceMeasurement(unit , printLogOnDestruction)
    {
        _eventName = std::move(eventName);
        _saveResultToJsonFile = saveResultToJsonFile;
    }

    inline PerformanceMeasurement(const std::string& eventName , PerformanceMeasurementUnit unit = PerformanceMeasurementUnit::MicroSecond , bool saveResultToJsonFile = false , bool printLogOnDestruction = true) : PerformanceMeasurement(unit , printLogOnDestruction)
    {
        _eventName = eventName;
        _saveResultToJsonFile = saveResultToJsonFile;
    }

    inline PerformanceMeasurement(const std::string& eventName , bool saveResultToJsonFile = false , bool printLogOnDestruction = true) : PerformanceMeasurement(PerformanceMeasurementUnit::MicroSecond , printLogOnDestruction)
    {
        _eventName = eventName;
        _saveResultToJsonFile = saveResultToJsonFile;
    }

    inline PerformanceMeasurement(PerformanceMeasurementUnit unit = PerformanceMeasurementUnit::MicroSecond , bool printLogOnDestruction = true) : _begin(std::chrono::high_resolution_clock::now()) , _unit(unit) , _saveResultToJsonFile(false) , _unitStr(setUnitStr()) , _printLogOnDestruction(printLogOnDestruction) , _canWriteToFile(true)
    {
    }

    inline auto getSpentTimeTillNow() noexcept
    {
        setEndTimePoint();

        if(_canWriteToFile && _saveResultToJsonFile)
        {
            BenchmarkJson::writeProfile({_eventName, getSpentTimeInMicroSecond() , getBeginTimePointInMicroSecond() , getEndTimePointInMicroSecond() , std::hash<std::thread::id>{}(std::this_thread::get_id())});
            _canWriteToFile = false;
        }

        switch (_unit)
        {
            case PerformanceMeasurementUnit::NanoSecond:
            {
                return std::chrono::duration<double , std::nano>(_end - _begin).count();
            }
            case PerformanceMeasurementUnit::MicroSecond:
            {
                return std::chrono::duration<double , std::micro>(_end - _begin).count();
            }
            case PerformanceMeasurementUnit::MilliSecond:
            {
                return std::chrono::duration<double , std::milli>(_end - _begin).count();
            }
            case PerformanceMeasurementUnit::CentiSecond:
            {
                return std::chrono::duration<double , std::centi>(_end - _begin).count();
            }
            case PerformanceMeasurementUnit::Second:
            {
                return std::chrono::duration<double , std::milli>(_end - _begin).count() / 1000.0;
            }
            case PerformanceMeasurementUnit::Minute:
            {
                constexpr auto toMinute = 1000.0 * 60.0;

                return std::chrono::duration<double , std::milli>(_end - _begin).count() / toMinute;
            }
        }
    }

    inline void log()
    {
        std::clog << "PerformanceMeasurement - " << _eventName << " execution time : " << getSpentTimeTillNow() << " " << _unitStr << "\n";
    }

    inline ~PerformanceMeasurement()
    {
        if(_printLogOnDestruction)
        {
            log();
        }

        if(_saveResultToJsonFile && _canWriteToFile)
        {
            setEndTimePoint();

            BenchmarkJson::writeProfile({_eventName, getSpentTimeInMicroSecond() , getBeginTimePointInMicroSecond() , getEndTimePointInMicroSecond() , std::hash<std::thread::id>{}(std::this_thread::get_id())});

            _canWriteToFile = false;
        }
    }

private:
    const std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> _begin;
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> _end;
    const PerformanceMeasurementUnit _unit;
    const std::string                _unitStr{};
    std::string                      _eventName{};
    bool                             _saveResultToJsonFile;
    const bool                       _printLogOnDestruction;
    bool                             _canWriteToFile;
    bool                             _isEndTimePointInitialized = false;
};

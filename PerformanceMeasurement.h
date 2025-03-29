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

/**** Not Completed ! ****/

// Helper macros for counting arguments
#define PERFORMANCE_MEASUREMENT_COUNT(...) \
PERFORMANCE_MEASUREMENT_COUNT_(__VA_ARGS__, 6, 5, 4, 3, 2, 1)
#define PERFORMANCE_MEASUREMENT_COUNT_(_1, _2, _3, _4, _5, _6, N, ...) N

// Macro dispatcher
#define PERFORMANCE_MEASUREMENT(varName, ...) \
PERFORMANCE_MEASUREMENT_CHOOSER(PERFORMANCE_MEASUREMENT_COUNT(__VA_ARGS__))(varName, __VA_ARGS__)

// Individual cases
#define PERFORMANCE_MEASUREMENT_CHOOSER(N) \
    PERFORMANCE_MEASUREMENT_CHOOSER_(N)
#define PERFORMANCE_MEASUREMENT_CHOOSER_(N) \
    PERFORMANCE_MEASUREMENT_##N

// Case 1: Just event name (string)
#define PERFORMANCE_MEASUREMENT_1(varName, eventName) \
    PerformanceMeasurement varName(#eventName)

// Case 2: Event name + unit
#define PERFORMANCE_MEASUREMENT_2(varName, eventName, unit) \
    PerformanceMeasurement varName(#eventName, unit)

// Case 3: Event name + unit + save flag
#define PERFORMANCE_MEASUREMENT_3(varName, eventName, unit, saveResultToJsonFile) \
    PerformanceMeasurement varName(#eventName, unit, saveResultToJsonFile)

// Case 4: Event name + unit + save flag + print flag
#define PERFORMANCE_MEASUREMENT_4(varName, eventName, unit, saveResultToJsonFile, printLogOnDestruction) \
    PerformanceMeasurement varName(#eventName, unit, saveResultToJsonFile, printLogOnDestruction)

// Case 5: Unit + save flag + print flag (no event name)
#define PERFORMANCE_MEASUREMENT_5(varName, unit, saveResultToJsonFile, printLogOnDestruction) \
    PerformanceMeasurement varName(unit, saveResultToJsonFile, printLogOnDestruction)

// Case 6: Event name + save flag + print flag (special case)
#define PERFORMANCE_MEASUREMENT_6(varName, eventName, saveResultToJsonFile, printLogOnDestruction) \
    PerformanceMeasurement varName(#eventName, saveResultToJsonFile, printLogOnDestruction)








// Helper macros for counting arguments
#define PERFORMANCE_MEASUREMENT_STRING_COUNT(...) \
PERFORMANCE_MEASUREMENT_STRING_COUNT_(__VA_ARGS__, 6, 5, 4, 3, 2, 1)
#define PERFORMANCE_MEASUREMENT_STRING_COUNT_(_1, _2, _3, _4, _5, _6, N, ...) N

// Macro dispatcher
#define PERFORMANCE_MEASUREMENT_STRING(...) \
PERFORMANCE_MEASUREMENT_STRING_CHOOSER(PERFORMANCE_MEASUREMENT_STRING_COUNT(__VA_ARGS__))(__VA_ARGS__)

// Individual cases
#define PERFORMANCE_MEASUREMENT_STRING_CHOOSER(N) \
    PERFORMANCE_MEASUREMENT_STRING_CHOOSER_(N)
#define PERFORMANCE_MEASUREMENT_STRING_CHOOSER_(N) \
    PERFORMANCE_MEASUREMENT_STRING_##N

// Case 1: Just event name (string)
#define PERFORMANCE_MEASUREMENT_STRING_1(eventName) \
    PerformanceMeasurement CONCAT(_perf_meas_, __LINE__)(eventName)

// Case 2: Event name + unit
#define PERFORMANCE_MEASUREMENT_STRING_2(eventName, unit) \
    PerformanceMeasurement CONCAT(_perf_meas_, __LINE__)(eventName, unit)

// Case 3: Event name + unit + save flag
#define PERFORMANCE_MEASUREMENT_STRING_3(eventName, unit, saveResultToJsonFile) \
    PerformanceMeasurement CONCAT(_perf_meas_, __LINE__)(eventName, unit, saveResultToJsonFile)

// Case 4: Event name + unit + save flag + print flag
#define PERFORMANCE_MEASUREMENT_STRING_4(eventName, unit, saveResultToJsonFile, printLogOnDestruction) \
    PerformanceMeasurement CONCAT(_perf_meas_, __LINE__)(eventName, unit, saveResultToJsonFile, printLogOnDestruction)

// Case 5: Unit + save flag + print flag (no event name)
#define PERFORMANCE_MEASUREMENT_STRING_5(unit, saveResultToJsonFile, printLogOnDestruction) \
    PerformanceMeasurement CONCAT(_perf_meas_, __LINE__)(unit, saveResultToJsonFile, printLogOnDestruction)

// Case 6: Event name + save flag + print flag
#define PERFORMANCE_MEASUREMENT_STRING_6(eventName, saveResultToJsonFile, printLogOnDestruction) \
    PerformanceMeasurement CONCAT(_perf_meas_, __LINE__)(eventName, saveResultToJsonFile, printLogOnDestruction)

// Helper for unique variable names
#define CONCAT(a, b) CONCAT_(a, b)
#define CONCAT_(a, b) a##b
/************************/



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

public:
    inline PerformanceMeasurement(std::string&& eventName , PerformanceMeasurementUnit unit = PerformanceMeasurementUnit::MicroSecond , bool saveResultToJsonFile = false , bool printLogOnDestruction = true) : PerformanceMeasurement(unit , printLogOnDestruction)
    {
        _saveResultToJsonFile = saveResultToJsonFile;
        _eventName = std::move(eventName);
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

    inline PerformanceMeasurement(PerformanceMeasurementUnit unit = PerformanceMeasurementUnit::MicroSecond , bool printLogOnDestruction = true) : _begin(std::chrono::high_resolution_clock::now()) , _unit(unit) , _saveResultToJsonFile(false) , _printLogOnDestruction(printLogOnDestruction) , _canWriteToFile(true)
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

    inline ~PerformanceMeasurement()
    {
        if(_printLogOnDestruction)
        {
            std::string unitStr;

            switch (_unit)
            {
                case PerformanceMeasurementUnit::NanoSecond:
                {
                    unitStr = std::string("NanoSeconds");
                    break;
                }
                case PerformanceMeasurementUnit::MicroSecond:
                {
                    unitStr = std::string("MicroSeconds");
                    break;
                }
                case PerformanceMeasurementUnit::MilliSecond:
                {
                    unitStr = std::string("MilliSeconds");
                    break;
                }
                case PerformanceMeasurementUnit::CentiSecond:
                {
                    unitStr = std::string("CentiSeconds");
                    break;
                }
                case PerformanceMeasurementUnit::Second:
                {
                    unitStr = std::string("Seconds");
                    break;
                }
                case PerformanceMeasurementUnit::Minute:
                {
                    unitStr = std::string("Minutes");
                    break;
                }
            }

            std::clog << "PerformanceMeasurement - " << _eventName << " execution time : " << getSpentTimeTillNow() << " " << unitStr << "\n";
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
    std::string                      _eventName{};
    bool                             _saveResultToJsonFile;
    const bool                       _printLogOnDestruction;
    bool                             _canWriteToFile;
    bool                             _isEndTimePointInitialized = false;
};

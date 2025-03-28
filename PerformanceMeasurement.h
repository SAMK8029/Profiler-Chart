#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <thread>

/**** Not Completed ! ****/

#define COUNT_ARGS(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define EXPAND_ARGS(args) COUNT_ARGS args
#define COUNT(...) EXPAND_ARGS((__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0))

#define MyMacro_1(name) PerformanceMeasurement name(std::string&& eventName , PerformanceMeasurementUnit unit = PerformanceMeasurementUnit::MicroSecond , bool saveResultToJsonFile = false , bool printLogOnDestruction = true) : PerformanceMeasurement(unit , saveResultToJsonFile , printLogOnDestruction)
#define MyMacro_2(name, count) std::cout << "Two arguments: " << name << ", " << count << std::endl

#define MyMacro_OVERLOAD(_1, _2, NAME, ...) NAME
#define MyMacro(...) MyMacro_OVERLOAD(__VA_ARGS__, MyMacro_2, MyMacro_1)(__VA_ARGS__)

/************************/

enum class PerformanceMeasurementUnit
{
    NanoSecond  ,
    MicroSecond ,
    MilliSecond ,
    CentiSecond ,
    Second      ,
    Minute
};

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

    void f(){}

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
    inline auto getSpentTimeInMicroSecond() noexcept
    {
        _end = std::chrono::high_resolution_clock::now();
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

    inline PerformanceMeasurement(std::string&& eventName , PerformanceMeasurementUnit unit = PerformanceMeasurementUnit::MicroSecond , bool saveResultToJsonFile = false , bool printLogOnDestruction = true) : PerformanceMeasurement(unit , saveResultToJsonFile , printLogOnDestruction)
    {
        _eventName = std::move(eventName);
    }

    inline PerformanceMeasurement(const std::string& eventName , PerformanceMeasurementUnit unit = PerformanceMeasurementUnit::MicroSecond , bool saveResultToJsonFile = false , bool printLogOnDestruction = true) : PerformanceMeasurement(unit , saveResultToJsonFile , printLogOnDestruction)
    {
        _eventName = eventName;
    }

    inline PerformanceMeasurement(const std::string& eventName , bool saveResultToJsonFile = false , bool printLogOnDestruction = true) : PerformanceMeasurement(PerformanceMeasurementUnit::MicroSecond , saveResultToJsonFile , printLogOnDestruction)
    {
        _eventName = eventName;
    }

    inline PerformanceMeasurement(PerformanceMeasurementUnit unit = PerformanceMeasurementUnit::MicroSecond , bool saveResultToJsonFile = false , bool printLogOnDestruction = true) : _begin(std::chrono::high_resolution_clock::now()) , _unit(unit) , _saveResultToJsonFile(saveResultToJsonFile) , _printLogOnDestruction(printLogOnDestruction) , _canWriteToFile(true)
    {
    }

    inline auto getSpentTimeTillNow() noexcept
    {
        if(_canWriteToFile && _saveResultToJsonFile)
        {
            BenchmarkJson::writeProfile({_eventName, getBeginTimePointInMicroSecond() , getEndTimePointInMicroSecond() , getSpentTimeInMicroSecond() , std::hash<std::thread::id>{}(std::this_thread::get_id())});
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
        if(_saveResultToJsonFile && _canWriteToFile)
        {
            BenchmarkJson::writeProfile({_eventName, getSpentTimeInMicroSecond() , getBeginTimePointInMicroSecond() , getEndTimePointInMicroSecond() , std::hash<std::thread::id>{}(std::this_thread::get_id())});

            _canWriteToFile = false;
        }

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
    }

private:
    const std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> _begin;
    std::chrono::time_point<std::chrono::high_resolution_clock, std::chrono::nanoseconds> _end;
    const PerformanceMeasurementUnit _unit;
    std::string                      _eventName{};
    const bool                       _saveResultToJsonFile;
    const bool                       _printLogOnDestruction;
    bool                             _canWriteToFile;
};


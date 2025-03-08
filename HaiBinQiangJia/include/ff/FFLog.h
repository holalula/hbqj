#ifndef __FFLOG_H
#define __FFLOG_H
#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <fstream>
#include <shlobj_core.h>
#include "../utils/SystemInfo.h"

#pragma warning( disable : 4996 )

namespace FFLog {
    enum LogType {
        info,
        warn,
        error
    };
    std::ostream& operator<<(std::ostream& os, LogType type) {
        switch (type) {
        case info: os << "[INFO]"; break;
        case warn: os << "[WARN]"; break;
        case error: os << "[ERRO]"; break;
        default: os.setstate(std::ios_base::failbit);
        }
        return os;
    }

    enum LogTarget {
        terminal,
        file,
    };
    struct LogProxy {
        void Wchar_tToString(std::string& szDst, wchar_t* wchar)
        {
            wchar_t* wText = wchar;
            DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);
            char* psText;
            psText = new char[dwNum];
            WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
            szDst = psText;
            delete[]psText;
        }
        std::string GetLocalAppDataPath() {
            WCHAR path[MAX_PATH];
            SHGetSpecialFolderPath(NULL, path, CSIDL_APPDATA, FALSE);
            std::string appdatapath;
            Wchar_tToString(appdatapath, path);
            return appdatapath;
        }

        std::string file_name = "log";
        const LogTarget log_target = terminal;

        LogType type;
        LogProxy(LogType type) : type(type) {
            file_name = GetLocalAppDataPath() + "\\HaiBinQiangJia\\hbqj.log";
        }

        std::string GetTimeNow() {
            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);

            std::ostringstream oss;
            oss << std::put_time(&tm, "%Y-%m-%d,%H:%M:%S");
            auto str = oss.str();

            return "[" + str + "]";
        }

        template<typename T>
        LogProxy& operator<< (const T& obj) {
            switch (log_target)
            {
            case terminal: 
                std::cout << type << GetTimeNow() << " ";
                std::cout << obj;
                break;
            case file:
                std::ofstream ostrm(file_name, std::ios::app);
                ostrm << type << GetTimeNow() << " ";
                ostrm << obj;
            }
            return *this;
        }

        LogProxy& operator<< (const char* cstr) {
            return (*this) << std::string(cstr);
        }

        // Handle << std::endl
        // https://stackoverflow.com/questions/1134388/stdendl-is-of-unknown-type-when-overloading-operator
        // this is the type of std::cout
        typedef std::basic_ostream<char, std::char_traits<char> > CoutType;

        // this is the function signature of std::endl
        typedef CoutType& (*StandardEndLine)(CoutType&);

        // define an operator<< to take in std::endl
        LogProxy& operator<<(StandardEndLine manip)
        {
            // call the function, but we cannot return it's value
            switch (log_target)
            {
            case terminal:
                manip(std::cout);
                break;
            case file:
                std::ofstream logstream(file_name, std::ios::app);
                manip(logstream);
                break;
            }
            return *this;
        }
    };

    LogProxy Log(LogType type) {
        return LogProxy(type);
    }

    LogProxy Log() {
        return LogProxy(LogType::info);
    }

    void LogSystemInfo() {
        LogProxy(LogType::info) << GetSystemInfo() << std::endl; 
    }

    template< typename T >
    std::string int_to_hex(T i)
    {
        std::stringstream stream;
        stream << "0x"
            << std::setfill('0') << std::setw(sizeof(T) * 2)
            << std::hex << i;
        return stream.str();
    }
};

#endif
#pragma once

class Debug
{
public:
    enum Level
    {
        DISABLE = 0,
        ERR,
        WARN,
        INFO,
        VERBOSE
    };

public:
    static void init();

    static void setLevel(Level level) { mLevel = level; }
    
    template<class ... Args>
    static inline void log(Level level, wchar_t const* fmt, Args ... args)
    {
        if ( level <= mLevel )
        {
            wprintf(fmt, args...);
        }
    }

    template<class ... Args>
    static inline void logErr(wchar_t const* fmt, Args ... args) { log(ERR, fmt, args...); }

    template<class ... Args>
    static inline void logWarn(wchar_t const* fmt, Args ... args) { log(WARN, fmt, args...); }

    template<class ... Args>
    static inline void logInfo(wchar_t const* fmt, Args ... args) { log(INFO, fmt, args...); }

    template<class ... Args>
    static inline void logVerbose(wchar_t const* fmt, Args ... args) { log(VERBOSE, fmt, args...); }

private:
    static Level mLevel;
};
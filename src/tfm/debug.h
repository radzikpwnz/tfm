// Copyright (c) Eltechs.
// All rights reserved.
//
// This code is licensed under the MIT License.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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
    static bool init();

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
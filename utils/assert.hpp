#pragma once 
#include <iostream> 
#include <source_location> 
#include <cstdlib> 
#include <format>


#if !defined(NDEBUG)
    #define SIM_DEBUG_MODE
#endif


namespace utils{
[[gnu::cold]]
inline void panic(char* condition_str, char* message, std::source_location location = std::source_location::current()){
    std::cerr << std::format("\n Critical FAILURE, Assertion failed! \n"
                              "  Condition : {}\n"
                              "  Message : {}\n"
                              "  Location : {}:{}\n"
                              "  Function : {}\n",
                              condition_str, message, location.file_name(), location.line(), location.function_name()) << std::endl;

                              std::abort();

}
}

#ifdef SIM_DEBUG_MODE
#define SIM_ASSERT(condition, message) \
do {    \
    if(!(condition)) [[unlikely]] {\
    utils::panic(#condition,message);\
}\
} while(false)
#else
#define SIM_ASSERT(condition, message) static_cast<void>(0)
#endif


#ifdef SIM_DEBUG_MODE
#define SIM_UNREACHABLE(message) utils::panic("Unreachable code reached", message)
#else
    #if defined(_MSC_VER)
        #define SIM_UNREACHABLE(msg) __assume(0)
    #elif defined(__GNUC__) || defined(__clang__)
        #define SIM_UNREACHABLE(msg) __builtin_unreachable()
    #else
        #define SIM_UNREACHABLE(msg) std::abort()
    #endif
#endif







#pragma once

#if defined(NK_DEBUG) || defined(NK_RELEASE)
    #define NK_ENABLE_ASSERTS
#endif

#if defined(NK_ENABLE_ASSERTS)

namespace nk {
    class Asserts {
    public:
        static void ReportAssertionFailure(
            std::string expression, std::string message, std::string file,
            i32 line
        );
    };
} // namespace nk

    //#if defined(NK_PLATFORM_LINUX)
    //    #define DebugBreak() __builtin_trap()
    //#elif defined(NK_PLATFORM_WINDOWS)
        #define DebugBreak() __debugbreak()
    //#else
    //    #error Engine only supports Windows and Linux!
    //#endif

    #define AssertMsg(expr, message)                                           \
        {                                                                      \
            if (expr) {                                                        \
            } else {                                                           \
                nk::Asserts::ReportAssertionFailure(                           \
                    #expr, message, __FILE__, __LINE__                         \
                );                                                             \
                DebugBreak();                                                  \
            }                                                                  \
        }
    #define Assert(expr) AssertMsg(expr, "")
    #define AssertKeep(expr) Assert(expr)
    #define AssertKeepMsg(expr, message) AssertMsg(expr, message)

    #if defined(NK_DEBUG)
        #define CoreAssertMsg(expr, message)                                   \
            {                                                                  \
                if (expr) {                                                    \
                } else {                                                       \
                    nk::Asserts::ReportAssertionFailure(                       \
                        #expr, message, __FILE__, __LINE__                     \
                    );                                                         \
                    DebugBreak();                                              \
                }                                                              \
            }
        #define CoreAssert(expr) CoreAssertMsg(expr, "")
        #define CoreAssertKeep(expr) CoreAssert(expr)
        #define CoreAssertKeepMsg(expr, message) CoreAssertMsg(expr, message)

    #else
        #define CoreAssert(expr)
        #define CoreAssertMsg(expr, message)
        #define CoreAssertKeep(expr) expr
        #define CoreAssertKeepMsg(expr, message) expr
    #endif

#else

    #define Assert(expr)
    #define AssertMsg(expr, message)
    #define AssertKeep(expr) expr
    #define AssertKeepMsg(expr, message) expr
    #define CoreAssert(expr)
    #define CoreAssertMsg(expr, message)
    #define CoreAssertKeep(expr) expr
    #define CoreAssertKeepMsg(expr, message) expr

#endif

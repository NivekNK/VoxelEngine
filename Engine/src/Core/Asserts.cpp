#include "nkpch.h"

#include "Asserts.h"

#ifdef NK_ENABLE_ASSERTS
namespace nk {
    void Asserts::ReportAssertionFailure(
        std::string expression, std::string message, std::string file, i32 line
    ) {
        if (!message.empty()) {
            CoreFatal(
                "Assertion Failure: {}, message: '{}', in file: {}, line: {}",
                expression, message, file, line
            );
        } else {
            CoreFatal(
                "Assertion Failure: {}, in file: {}, line: {}",
                expression, file, line
            );
        }
    }
}  // namespace nk
#endif

#include "nkpch.h"

#include "Layer.h"

namespace nk {
    Layer::Layer(const std::string& debug_name)
        : m_DebugName(std::move(debug_name)) {}
}  // namespace nk

#pragma once

#define BIT(x) (1 << x)

#define NK_BIND_EVENT_FN(fn)                                                   \
    [this](auto&& arg) -> bool { return fn(std::forward<decltype(arg)>(arg)); }

#define NK_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define NK_MIN(a,b) (((a) < (b)) ? (a) : (b))
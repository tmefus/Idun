#pragma once

#include <vector>

#include "value.hpp"
#include "interpreter.hpp"

struct LoxCallable : public LoxValue {
    // 参数数量
    virtual size_t arity() = 0;

    // 实际调用
    virtual LoxValuePtr call(Interpreter &interpreter, std::vector<LoxValuePtr> &args) = 0;
};
#include "lox_instance.hpp"

LoxValuePtr LoxClass::call(Interpreter &interpreter, std::vector<LoxValuePtr> &args) {
    auto instance = std::make_shared<LoxInstance>(shared_from_this());
    auto init = findMethod("init");
    if (init) init->bind(instance)->call(interpreter, args);
    return instance;
}
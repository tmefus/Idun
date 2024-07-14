#pragma once

#include <utility>

#include "callable.hpp"
#include "lox_function.hpp"

class LoxClass : public LoxCallable, public std::enable_shared_from_this<LoxClass> {
private:
    std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods_;

public:
    std::string name_;
    std::shared_ptr<LoxClass> super_;

    explicit LoxClass(
            std::string name_,
            std::shared_ptr<LoxClass> super_,
            std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods_
    ) : name_(std::move(name_)), super_{std::move(super_)}, methods_{std::move(methods_)} {};

    size_t arity() override {
        auto init = findMethod("init");
        if (!init) return 0;
        return init->arity();
    }

    LoxValuePtr call(Interpreter &interpreter, std::vector<LoxValuePtr> &args) override;

    std::shared_ptr<LoxFunction> findMethod(const std::string &name) {
        auto it = methods_.find(name);
        if (it != methods_.end()) {
            return it->second;
        }
        if (super_) {
            return super_->findMethod(name);
        }
        return nullptr;
    }

    std::ostream &operator<<(std::ostream &o) override {
        return o << "<class " << name_ << ">";
    };

    ~LoxClass() override = default;
};
#pragma once

#include "value.hpp"
#include "lox_class.hpp"

class LoxInstance : public LoxValue, public std::enable_shared_from_this<LoxInstance> {
private:
    std::shared_ptr<LoxClass> class_;
    std::unordered_map<std::string, LoxValuePtr> fields_;

public:
    explicit LoxInstance(const std::shared_ptr<LoxClass> &class_) : class_(class_) {}

    LoxValuePtr get(const std::shared_ptr<Token> &name) {
        auto it = fields_.find(name->lexeme);
        if (it != fields_.end()) {
            return it->second;
        }

        auto method = class_->findMethod(name->lexeme);
        if (method) return method->bind(shared_from_this());

        throw interpreter_error{name, "Undefined property '" + name->lexeme + "'."};
    }

    void set(const std::shared_ptr<Token> &name, const std::shared_ptr<LoxValue> &value) {
        // todo: 这里实现 不允许自由创建类的字段
        fields_.insert_or_assign(name->lexeme, value);
    }

    std::ostream &operator<<(std::ostream &o) override {
        o << "<Instance " << class_->name_ << ">";
        return o;
    }
};
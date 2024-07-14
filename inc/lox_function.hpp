#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <utility>
#include "callable.hpp"
#include "stmt.hpp"
#include "lox_exception.hpp"

class LoxFunction : public LoxCallable {
private:
    FunctionStmtPtr declaration_;
    std::shared_ptr<Environment> closure_;
    bool isInitializer_;

public:
    LoxFunction(FunctionStmtPtr declaration, std::shared_ptr<Environment> closure, bool isInitializer_)
            : declaration_{std::move(declaration)}, closure_{std::move(closure)}, isInitializer_{isInitializer_} {};

    size_t arity() override {
        return declaration_->params_->size();
    };

    LoxValuePtr call(Interpreter &interpreter, std::vector<LoxValuePtr> &args) override {
        auto env = std::make_shared<Environment>(closure_);

        // 这里将调用时传入的具体 参数值 与 参数变量 绑定
        for (int i = 0; i < args.size(); ++i) {
            env->define(declaration_->params_->at(i)->lexeme, args.at(i));
        }

        try {
            /* 这是处理函数返回值的方法。
             * 函数调用就是执行有自己作用域的 executeBlock，
             * 当函数执行到return语句（Interpreter::visitReturnStmt）时，会触发自定义错误。
             * 此时try会捕捉这个错误，然后在下面的（return retVal.value_）中返回 return的值
             * */
            interpreter.executeBlock(declaration_->body_, env);
        } catch (return_value &retVal) {
            if (isInitializer_) return closure_->getAt(0, "this");
            return retVal.value_;
        }

        if (isInitializer_) return closure_->getAt(0, "this");

        return std::make_shared<LoxNil>();
    };

    std::shared_ptr<LoxFunction> bind(const std::shared_ptr<LoxValue> &instance) {
        auto env = std::make_shared<Environment>(closure_);
        env->define("this", instance);
        return std::make_shared<LoxFunction>(declaration_, env, isInitializer_);
    }

    std::ostream &operator<<(std::ostream &o) override {
        o << "<function " << declaration_->name_->lexeme << ">";
        return o;
    };
};

struct NativePrint : public LoxCallable {
    size_t arity() override { return 1; }; // todo: 设计不定参数函数

    LoxValuePtr call(Interpreter &interpreter, std::vector<LoxValuePtr> &args) override {
        for (const auto &arg: args) {
            std::cout << *arg << std::endl;
        }
        return std::make_shared<LoxNil>();
    };

    std::ostream &operator<<(std::ostream &o) override {
        o << "<native-function print>";
        return o;
    };
};

struct NativeClock : public LoxCallable {
    size_t arity() override { return 0; };

    LoxValuePtr call(Interpreter &interpreter, std::vector<LoxValuePtr> &args) override {
        auto epoch = std::chrono::system_clock::now().time_since_epoch();
        auto ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
        return std::make_shared<LoxFloat>(((double) ms_since_epoch.count()) / 1000.0F);
    };

    std::ostream &operator<<(std::ostream &o) override {
        o << "<native-function clock>";
        return o;
    };
};
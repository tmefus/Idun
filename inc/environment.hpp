#pragma once

#include <unordered_map>
#include <string>
#include <utility>

#include "value.hpp"
#include "token.hpp"

struct Environment : public std::enable_shared_from_this<Environment> {
    std::shared_ptr<Environment> parentEnv;

    Environment() = default;

    explicit Environment(std::shared_ptr<Environment> parent) : parentEnv{std::move(parent)} {};

    void define(const std::string &name, LoxValuePtr value);

    LoxValuePtr get(const TokenPtr &token);

    LoxValuePtr getAt(int distance, const std::string &name);

    void assign(const TokenPtr &token, const LoxValuePtr &value);

    void assignAt(int distance, const TokenPtr &token, LoxValuePtr value);

private:
    std::unordered_map<std::string, LoxValuePtr> values;

    std::shared_ptr<Environment> ancestor(int distance);
};

struct EnvGuard {
    EnvGuard(std::shared_ptr<Environment> &currentEnv, std::shared_ptr<Environment> oldEnv)
            : newEnv_{currentEnv}, oldEnv_{std::move(oldEnv)} {}

    ~EnvGuard() {
        newEnv_ = oldEnv_;
    }

private:
    std::shared_ptr<Environment> &newEnv_;
    std::shared_ptr<Environment> oldEnv_;
};
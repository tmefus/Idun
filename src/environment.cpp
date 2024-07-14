#include "environment.hpp"
#include "lox_exception.hpp"

void Environment::define(const std::string &name, LoxValuePtr value) {
    values.insert_or_assign(name, value);
}

LoxValuePtr Environment::get(const TokenPtr &token) {
    auto it = values.find(token->lexeme);
    if (it != values.end()) return it->second;

    if (parentEnv) return parentEnv->get(token);

    throw interpreter_error{token, "Undefined variable '" + token->lexeme + '\''};
}

LoxValuePtr Environment::getAt(int distance, const std::string &name) {
    return ancestor(distance)->values.find(name)->second;
}

void Environment::assign(const TokenPtr &token, const LoxValuePtr &value) {
    auto it = values.find(token->lexeme);

    if (it != values.end()) {
        it->second = value;
        return;
    }

    if (parentEnv) {
        parentEnv->assign(token, value);
        return;
    }

    throw interpreter_error{token, "Undefined variable '" + token->lexeme + '\''};
}

void Environment::assignAt(int distance, const TokenPtr &token, LoxValuePtr value) {
    ancestor(distance)->values.insert_or_assign(token->lexeme, value);
}

std::shared_ptr<Environment> Environment::ancestor(int distance) {
    auto env = shared_from_this();

    for (int i = 0; i < distance; ++i) {
        env = env->parentEnv;
    }

    return env;
}


#pragma once

#include <stdexcept>
#include <utility>


struct parsing_error : public std::runtime_error {
    explicit parsing_error(const std::string &what) : std::runtime_error{what} {}
};

struct interpreter_error : public std::runtime_error {
    TokenPtr token_;

    interpreter_error(TokenPtr token, const std::string &msg) : token_{std::move(token)}, std::runtime_error{msg} {}
};

struct return_value : public std::runtime_error {
    LoxValuePtr value_;

    explicit return_value(LoxValuePtr value) : value_{std::move(value)}, std::runtime_error{""} {}
};

struct continue_loop : public std::runtime_error {
    explicit continue_loop() : std::runtime_error{""} {}
};

struct break_loop : public std::runtime_error {
    explicit break_loop() : std::runtime_error{""} {}
};
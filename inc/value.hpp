#pragma once

#include <string>
#include <memory>

class LoxValue {
public:
    friend std::ostream &operator<<(std::ostream &o, LoxValue &value) {
        value.operator<<(o);
        return o;
    };

    virtual std::ostream &operator<<(std::ostream &o) = 0;

    virtual ~LoxValue() = default;
};

using LoxValuePtr = std::shared_ptr<LoxValue>;

struct LoxString : public LoxValue {
    std::string value_;

    explicit LoxString(std::string value) : value_{std::move(value)} {}

    ~LoxString() override = default;

    std::ostream &operator<<(std::ostream &o) override {
        return o << value_;
    };
};

struct LoxInt : public LoxValue {
    int64_t value_;

    explicit LoxInt(int64_t value) : value_{value} {}

    ~LoxInt() override = default;

    std::ostream &operator<<(std::ostream &o) override {
        return o << value_;
    }
};

struct LoxFloat : public LoxValue {
    double value_;

    explicit LoxFloat(double value) : value_{value} {}

    ~LoxFloat() override = default;

    std::ostream &operator<<(std::ostream &o) override {
        return o << value_;
    };
};

struct LoxBool : public LoxValue {
    bool value_;

    explicit LoxBool(bool value) : value_{value} {}

    ~LoxBool() override = default;

    std::ostream &operator<<(std::ostream &o) override {
        return value_ ? o << "true" : o << "false";
    };
};

struct LoxNil : public LoxValue {
    ~LoxNil() override = default;

    std::ostream &operator<<(std::ostream &o) override {
        return o << "nil";
    };
};


#include "interpreter.hpp"

#include <iostream>
#include <utility>
#include <cmath>
#include <algorithm>
#include "lox_exception.hpp"
#include "lox_instance.hpp"

#define CAST(TO_TYPE, FROM_VAL) std::dynamic_pointer_cast<TO_TYPE>(FROM_VAL)

Interpreter::Interpreter() : result(std::make_shared<LoxNil>()), global(std::make_shared<Environment>()), env(global) {
    global->define("print", std::make_shared<NativePrint>());
    global->define("clock", std::make_shared<NativeClock>());
}

// 访问赋值表达式
void Interpreter::visitAssignExpr(AssignExprPtr expr) {
    auto right = evaluate(expr->value_);
    auto it = locals.find(expr);
    if (it != locals.end()) {
        env->assignAt(it->second, expr->name_, result);
    } else {
        global->assign(expr->name_, result);
    }
}

void Interpreter::visitBinaryExpr(BinaryExprPtr expr) {
    auto left = evaluate(expr->left_);
    auto right = evaluate(expr->right_);

    switch (expr->op_->type) {
        case TokenType::MINUS: {
            checkNumberOps(expr->op_, left, right);
            if (isFloat(left) || isFloat(right)) {
                result = std::make_shared<LoxFloat>(getFloat(left) - getFloat(right));
            } else {
                result = std::make_shared<LoxInt>(getInt(left) - getInt(right));
            }
            break;
        }
        case TokenType::SLASH: {
            checkNumberOps(expr->op_, left, right);
            if (isFloat(left) || isFloat(right)) {
                auto val = getFloat(right);
                if (val == 0) throw interpreter_error{expr->op_, "Division by 0"};
                result = std::make_shared<LoxFloat>(getFloat(left) / val);
            } else {
                auto val = getInt(right);
                if (val == 0) throw interpreter_error{expr->op_, "Division by 0"};
                result = std::make_shared<LoxInt>(getInt(left) / val);
            }
            break;
        }
        case TokenType::STAR: {
            checkNumberOps(expr->op_, left, right);
            if (isFloat(left) || isFloat(right)) {
                result = std::make_shared<LoxFloat>(getFloat(left) * getFloat(right));
            } else {
                result = std::make_shared<LoxInt>(getInt(left) * getInt(right));
            }
            break;
        }
        case TokenType::POWER: {
            checkNumberOps(expr->op_, left, right);
            result = std::make_shared<LoxFloat>(pow(getFloat(left), getFloat(right)));
            break;
        }
        case TokenType::MOD: {
            checkNumberOps(expr->op_, left, right);
            if (isFloat(left) || isFloat(right)) {
                auto val = getFloat(right);
                if (val == 0) throw interpreter_error{expr->op_, "Remainder by 0 is undefined"};
                result = std::make_shared<LoxFloat>(fmod(getFloat(left), getFloat(right)));
            } else {
                auto val = getInt(right);
                if (val == 0) throw interpreter_error{expr->op_, "Remainder by 0 is undefined"};
                result = std::make_shared<LoxInt>(getInt(left) % getInt(right));
            }
            break;
        }
        case TokenType::PLUS: {
            if (isNum(left) && isNum(right)) {
                if (isFloat(left) || isFloat(right)) {
                    result = std::make_shared<LoxFloat>(getFloat(left) + getFloat(right));
                } else {
                    result = std::make_shared<LoxInt>(getInt(left) + getInt(right));
                }
            } else {
                // 字符串拼接
                string.str("");
                string << *left << *right;
                result = std::make_shared<LoxString>(string.str());
            }
            break;
        }
        case TokenType::BIT_OR: {
            checkNumberOps(expr->op_, left, right);
            if (isFloat(left) || isFloat(right)) {
                throw interpreter_error{expr->op_, "Wrong type argument to bit-complement"};
            } else {
                result = std::make_shared<LoxInt>(getInt(left) | getInt(right));
            }
            break;
        }
        case TokenType::BIT_XOR: {
            checkNumberOps(expr->op_, left, right);
            if (isFloat(left) || isFloat(right)) {
                throw interpreter_error{expr->op_, "Wrong type argument to bit-complement"};
            } else {
                result = std::make_shared<LoxInt>(getInt(left) ^ getInt(right));
            }
            break;
        }
        case TokenType::BIT_AND: {
            checkNumberOps(expr->op_, left, right);
            if (isFloat(left) || isFloat(right)) {
                throw interpreter_error{expr->op_, "Wrong type argument to bit-complement"};
            } else {
                result = std::make_shared<LoxInt>(getInt(left) & getInt(right));
            }
            break;
        }
        case TokenType::SHIFT_L: {
            checkNumberOps(expr->op_, left, right);
            if (isFloat(left) || isFloat(right)) {
                throw interpreter_error{expr->op_, "Wrong type argument to bit-complement"};
            } else {
                result = std::make_shared<LoxInt>(getInt(left) << getInt(right));
            }
            break;
        }
        case TokenType::SHIFT_R: {
            checkNumberOps(expr->op_, left, right);
            if (isFloat(left) || isFloat(right)) {
                throw interpreter_error{expr->op_, "Wrong type argument to bit-complement"};
            } else {
                result = std::make_shared<LoxInt>(getInt(left) >> getInt(right));
            }
            break;
        }
        case TokenType::GREATER: {
            checkNumberOps(expr->op_, left, right);
            bool comp;
            if (isFloat(left) || isFloat(right)) {
                comp = getFloat(left) > getFloat(right);
            } else {
                comp = getInt(left) > getInt(right);
            }
            result = std::make_shared<LoxBool>(comp);
            break;
        }
        case TokenType::GREATER_EQUAL: {
            checkNumberOps(expr->op_, left, right);
            bool comp;
            if (isFloat(left) || isFloat(right)) {
                comp = getFloat(left) >= getFloat(right);
            } else {
                comp = getInt(left) >= getInt(right);
            }
            result = std::make_shared<LoxBool>(comp);
            break;
        }
        case TokenType::LESS: {
            checkNumberOps(expr->op_, left, right);
            bool comp;
            if (isFloat(left) || isFloat(right)) {
                comp = getFloat(left) < getFloat(right);
            } else {
                comp = getInt(left) < getInt(right);
            }
            result = std::make_shared<LoxBool>(comp);
            break;
        }
        case TokenType::LESS_EQUAL: {
            checkNumberOps(expr->op_, left, right);
            bool comp;
            if (isFloat(left) || isFloat(right)) {
                comp = getFloat(left) <= getFloat(right);
            } else {
                comp = getInt(left) <= getInt(right);
            }
            result = std::make_shared<LoxBool>(comp);
            break;
        }
        case TokenType::NOT_EQUAL: {
            result = std::make_shared<LoxBool>(!isEqual(left, right));
            break;
        }
        case TokenType::EQUAL_EQUAL: {
            result = std::make_shared<LoxBool>(isEqual(left, right));
            break;
        }
        case TokenType::IN:
        case TokenType::NOTIN:
        case TokenType::IS:
        case TokenType::NOTIS: {
            // todo::
            result = std::make_shared<LoxBool>(false);
            break;
        }
        default:break;
    }
}

void Interpreter::visitGroupingExpr(GroupingExprPtr expr) {
    result = evaluate(expr->expression_);
}

void Interpreter::visitLiteralExpr(LiteralExprPtr expr) {
    result = expr->value_;
}

void Interpreter::visitStrExpr(StrExprPtr expr) {
    string.str("");
    for (const auto &str: *expr->strs) {
        auto v = evaluate(str);
        string << *v;
    }
    result = std::make_shared<LoxString>(string.str());
}

void Interpreter::visitUnaryExpr(UnaryExprPtr expr) {
    auto right = evaluate(expr->right_);

    switch (expr->op_->type) {
        case TokenType::NOT: {
            result = std::make_shared<LoxBool>(!isTruth(right));
            break;
        }
        case TokenType::MINUS: {
            checkNumberOp(expr->op_, right);
            if (isFloat(right)) {
                result = std::make_shared<LoxFloat>(-getFloat(right));
            } else {
                result = std::make_shared<LoxInt>(-getInt(right));
            }
            break;
        }
        case TokenType::BIT_NOT: {
            checkNumberOp(expr->op_, right);

            if (isFloat(right)) throw interpreter_error{expr->op_, "Wrong type argument to bit-complement"};

            result = std::make_shared<LoxInt>(~getInt(right));
            break;
        }
        default:break;
    }
}

void Interpreter::visitVariableExpr(VariableExprPtr expr) {
    result = lookupVariable(expr->name_, expr);
}

void Interpreter::visitLogicalExpr(LogicalExprPtr expr) {
    auto left = evaluate(expr->left_);
    if (expr->op_->type == TokenType::OR) {
        if (isTruth(left)) {
            return;
        }
    } else if (expr->op_->type == TokenType::AND) {
        if (!isTruth(left)) {
            return;
        }
    }
    evaluate(expr->right_);
}

void Interpreter::visitCallExpr(CallExprPtr expr) {
    auto callee = evaluate(expr->callee_);
    std::vector<LoxValuePtr> args;
    for (const auto &arg: *(expr->args_)) {
        args.push_back(evaluate(arg));
    }
    // 把 函数调用类型 转为 函数定义类型
    if (auto function = CAST(LoxCallable, callee)) {
        if (args.size() != function->arity()) {
            throw interpreter_error{
                    expr->paren_,
                    std::format("Expected {} arguments but got {}.", function->arity(), args.size())
            };
        }
        result = function->call(*this, args);
    } else {
        throw interpreter_error{expr->paren_, "Can only call functions and classes."};
    }
}

void Interpreter::visitGetExpr(GetExprPtr expr) {
    auto instance = evaluate(expr->expr_);
    if (auto loxClass = CAST(LoxInstance, instance)) {
        result = loxClass->get(expr->name_);
        return;
    }
    throw interpreter_error{expr->name_, "Only instances have properties."};
}

void Interpreter::visitSetExpr(SetExprPtr expr) {
    auto instance = evaluate(expr->expr_);
    auto loxClass = CAST(LoxInstance, instance);
    if (!loxClass) {
        throw interpreter_error{expr->name_, "Only instances have fields."};
    }
    std::shared_ptr<LoxValue> value = evaluate(expr->value_);
    loxClass->set(expr->name_, value);
}

void Interpreter::visitThisExpr(ThisExprPtr expr) {
    result = lookupVariable(expr->keyword_, expr);
}

void Interpreter::visitSuperExpr(SuperExprPtr expr) {
    int distance = locals.at(expr);
    auto super_ = CAST(LoxClass, env->getAt(distance, "super"));
    auto instance = CAST(LoxInstance, env->getAt(distance - 1, "this"));
    auto method = super_->findMethod(expr->method_->lexeme);
    if (!method) {
        throw interpreter_error{expr->method_, "Undefined property '" + expr->method_->lexeme + "'."};
    }
    result = method->bind(instance);
}

void Interpreter::visitIfStmt(IfStmtPtr stmt) {
    if (isTruth(evaluate(stmt->condition_))) {
        execute(stmt->thenStmt_);
    } else if (stmt->elseStmt_) {
        execute(stmt->elseStmt_);
    }
}

void Interpreter::visitWhileStmt(WhileStmtPtr stmt) {
    while (isTruth(evaluate(stmt->condition_))) {
        try {
            execute(stmt->statements_);
        } catch (continue_loop &_) {
            continue;
        } catch (break_loop &_) {
            break;
        }
    }
}

void Interpreter::visitContinueStmt(ContinueStmtPtr stmt) {
    throw continue_loop{};
}

void Interpreter::visitBreakStmt(BreakStmtPtr stmt) {
    throw break_loop{};
}

void Interpreter::visitForStmt(ForStmtPtr stmt) {
    // todo:123

}

void Interpreter::visitWhenStmt(WhenStmtPtr stmt) {
    // when的所有分支
    for (const auto &conds_block: *stmt->branches) {
        // 判断该分支是否所有条件都为真
        auto allTrue = std::ranges::any_of(
                conds_block.first,
                [this](const auto &cond) { return isTruth(evaluate(cond)); }
        );
        if (allTrue) {  // 该分支条件都为真则执行分支语句
            execute(conds_block.second);
            return;     // 分支执行完成后直接退出when语句
        }
    }
    execute(stmt->else_);  // 任何分支都不为真则执行else语句
}

void Interpreter::visitBlockStmt(BlockStmtPtr stmt) {
    executeBlock(stmt->statements_, std::make_shared<Environment>(env));
}

void Interpreter::visitExpressionStmt(ExpressionStmtPtr stmt) {
    evaluate(stmt->expression_);
}

void Interpreter::visitLetStmt(LetStmtPtr stmt) {
    LoxValuePtr initVal = evaluate(stmt->initializer_);
    env->define(stmt->name_->lexeme, initVal);
}

void Interpreter::visitVarStmt(VarStmtPtr stmt) {
    LoxValuePtr initVal;
    if (stmt->initializer_) {
        initVal = evaluate(stmt->initializer_);
    } else {
        initVal = std::make_shared<LoxNil>();
    }

    env->define(stmt->name_->lexeme, initVal);
}

void Interpreter::visitFunctionStmt(FunctionStmtPtr stmt) {
    auto funcDef = std::make_shared<LoxFunction>(stmt, env, false);
    // 在当前作用域用函数名声明一个函数
    env->define(stmt->name_->lexeme, funcDef);
}

void Interpreter::visitReturnStmt(ReturnStmtPtr stmt) {
    LoxValuePtr retValue;
    if (stmt->value_) {
        retValue = evaluate(stmt->value_);
    } else {
        retValue = std::make_shared<LoxNil>();
    }
    throw return_value{retValue};
}

void Interpreter::visitClassStmt(ClassStmtPtr stmt) {
    LoxValuePtr superClass = nullptr;
    std::shared_ptr<LoxClass> boolClass = nullptr;
    if (stmt->superClass_) {
        superClass = evaluate(stmt->superClass_);
        boolClass = std::dynamic_pointer_cast<LoxClass>(superClass);
        if (!boolClass) {
            throw interpreter_error{stmt->superClass_->name_, "Superclass must be a class."};
        }
    }
    env->define(stmt->name_->lexeme, nullptr);

    if (stmt->superClass_) {
        env = std::make_shared<Environment>(env);
        env->define("super", superClass);
    }

    std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods;
    for (const auto &method: *stmt->methods_) {
        auto function =
                std::make_shared<LoxFunction>(method, env, method->name_->lexeme == "init");
        methods.insert_or_assign(method->name_->lexeme, function);
    }
    auto loxClass = std::make_shared<LoxClass>(stmt->name_->lexeme, boolClass, methods);

    if (stmt->superClass_) {
        env = env->parentEnv;
    }

    env->assign(stmt->name_, loxClass);
}

bool Interpreter::isTruth(const LoxValuePtr &value) {
    if (auto boolVal = std::dynamic_pointer_cast<LoxBool>(value)) {
        return boolVal->value_;
    }

    if (auto numberVal = std::dynamic_pointer_cast<LoxInt>(value)) {
        return numberVal->value_ != 0;
    }

    if (auto nilVal = std::dynamic_pointer_cast<LoxNil>(value)) {
        return false;
    }

    return true;
}

bool Interpreter::isEqual(const LoxValuePtr &a, const LoxValuePtr &b) {
    if (auto nila = CAST(LoxNil, a)) {
        if (auto nilb = CAST(LoxNil, b)) {
            return true;
        }
        return false;
    } else if (auto nilb = CAST(LoxNil, b)) {
        return false;
    }

    if (isNum(a) && isNum(b)) {
        auto val1 = getFloat(a), val2 = getFloat(b);
        return val1 == val2;
    }

    if (isString(a) && isString(b)) {
        return CAST(LoxString, a)->value_ == CAST(LoxString, b)->value_;
    }

    if (isBool(a) && isBool(b)) {
        return CAST(LoxBool, a)->value_ == CAST(LoxBool, b)->value_;
    }

    return false;
}

bool Interpreter::isBool(const LoxValuePtr &value) {
    if (auto boolVal = CAST(LoxBool, value)) {
        return true;
    }
    return false;
}

bool Interpreter::isNum(const LoxValuePtr &value) {
    if (auto numVal = CAST(LoxInt, value)) {
        return true;
    }
    if (auto floatVal = CAST(LoxFloat, value)) {
        return true;
    }
    return false;
}

bool Interpreter::isFloat(const LoxValuePtr &value) {
    if (auto floatVal = CAST(LoxFloat, value)) {
        return true;
    }
    return false;
}

bool Interpreter::isString(const LoxValuePtr &value) {
    if (auto strVal = CAST(LoxString, value)) {
        return true;
    }
    return false;
}

int64_t Interpreter::getInt(const LoxValuePtr &value) {
    return CAST(LoxInt, value)->value_;
}

double Interpreter::getFloat(const LoxValuePtr &value) {
    if (auto floatVal = CAST(LoxFloat, value)) {
        return floatVal->value_;
    }
    return CAST(LoxInt, value)->value_;
}

void Interpreter::checkNumberOp(TokenPtr op, const LoxValuePtr &value) {
    if (isNum(value)) return;
    throw interpreter_error{std::move(op), "Operand must be a number."};
}

void Interpreter::checkNumberOps(TokenPtr op, const LoxValuePtr &left, const LoxValuePtr &right) {
    if (isNum(left) && isNum(right)) return;
    throw interpreter_error{std::move(op), "Operands must be numbers."};
}

LoxValuePtr Interpreter::evaluate(const ExprPtr &expr) {
    expr->accept(*this);
    return result;
}

void Interpreter::execute(const StmtPtr &stmt) {
    stmt->accept(*this);
}

void Interpreter::executeBlock(const std::shared_ptr<std::vector<StmtPtr>> &statements, std::shared_ptr<Environment> _env) {
    // 保护作用域
    auto previousEnv = this->env;
    this->env = std::move(_env);

    EnvGuard envGuard{this->env, previousEnv};

    for (const auto &stmt: *statements) {
        stmt->accept(*this);
    }
}

LoxValuePtr Interpreter::lookupVariable(const TokenPtr &name, const ExprPtr &expr) {
    auto it = locals.find(expr);

    if (it != locals.end()) {
        return env->getAt(it->second, name->lexeme);
    } else {
        return global->get(name);
    }
}

void Interpreter::resolve(const ExprPtr &expr, int depth) {
    locals.insert_or_assign(expr, depth);
}

void Interpreter::interpret(const std::vector<StmtPtr> &statements) {
    try {
        for (const auto &statement: statements) {
            execute(statement);
        }
    } catch (interpreter_error &error) {
        std::cerr << "Line [" << error.token_->line << "]: " << error.what() << std::endl;
    }
}
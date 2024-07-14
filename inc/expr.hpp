#pragma once

#include <vector>

#include "token.hpp"

struct AssignExpr;
struct BinaryExpr;
struct GroupingExpr;
struct LiteralExpr;
struct StrExpr;
struct UnaryExpr;
struct VariableExpr;
struct LogicalExpr;
struct CallExpr;
struct GetExpr;
struct SetExpr;
struct ThisExpr;
struct SuperExpr;

struct Expr {
    struct AbstractVisitor {
        virtual void visitAssignExpr(std::shared_ptr<AssignExpr> expr) = 0;

        virtual void visitBinaryExpr(std::shared_ptr<BinaryExpr> expr) = 0;

        virtual void visitGroupingExpr(std::shared_ptr<GroupingExpr> expr) = 0;

        virtual void visitLiteralExpr(std::shared_ptr<LiteralExpr> expr) = 0;

        virtual void visitStrExpr(std::shared_ptr<StrExpr> expr) = 0;

        virtual void visitUnaryExpr(std::shared_ptr<UnaryExpr> expr) = 0;

        virtual void visitVariableExpr(std::shared_ptr<VariableExpr> expr) = 0;

        virtual void visitLogicalExpr(std::shared_ptr<LogicalExpr> expr) = 0;

        virtual void visitCallExpr(std::shared_ptr<CallExpr> expr) = 0;

        virtual void visitGetExpr(std::shared_ptr<GetExpr> expr) = 0;

        virtual void visitSetExpr(std::shared_ptr<SetExpr> expr) = 0;

        virtual void visitThisExpr(std::shared_ptr<ThisExpr> expr) = 0;

        virtual void visitSuperExpr(std::shared_ptr<SuperExpr> expr) = 0;
    };

    virtual void accept(AbstractVisitor &visitor) = 0;

    virtual ~Expr() = default;
};

using ExprPtr = std::shared_ptr<Expr>;

struct AssignExpr : public Expr, public std::enable_shared_from_this<AssignExpr> {
    std::shared_ptr<Token> name_;
    std::shared_ptr<Expr> value_;

    AssignExpr(std::shared_ptr<Token> name, std::shared_ptr<Expr> value)
            : name_{std::move(name)}, value_{std::move(value)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitAssignExpr(shared_from_this());
    }
};

using AssignExprPtr = std::shared_ptr<AssignExpr>;

struct BinaryExpr : public Expr, public std::enable_shared_from_this<BinaryExpr> {
    std::shared_ptr<Expr> left_;
    std::shared_ptr<Token> op_;
    std::shared_ptr<Expr> right_;

    BinaryExpr(std::shared_ptr<Expr> left, std::shared_ptr<Token> op, std::shared_ptr<Expr> right)
            : left_{std::move(left)}, op_{std::move(op)}, right_{std::move(right)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitBinaryExpr(shared_from_this());
    }
};

using BinaryExprPtr = std::shared_ptr<BinaryExpr>;

struct GroupingExpr : public Expr, public std::enable_shared_from_this<GroupingExpr> {
    std::shared_ptr<Expr> expression_;

    explicit GroupingExpr(std::shared_ptr<Expr> expression) : expression_{std::move(expression)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitGroupingExpr(shared_from_this());
    }
};

using GroupingExprPtr = std::shared_ptr<GroupingExpr>;

struct LiteralExpr : public Expr, public std::enable_shared_from_this<LiteralExpr> {
    std::shared_ptr<LoxValue> value_;

    explicit LiteralExpr(std::shared_ptr<LoxValue> value) : value_{std::move(value)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitLiteralExpr(shared_from_this());
    }
};

using LiteralExprPtr = std::shared_ptr<LiteralExpr>;

struct StrExpr : public Expr, public std::enable_shared_from_this<StrExpr> {
    std::shared_ptr<std::vector<ExprPtr>> strs;

    explicit StrExpr(std::shared_ptr<std::vector<ExprPtr>> strs) : strs{std::move(strs)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitStrExpr(shared_from_this());
    }
};

using StrExprPtr = std::shared_ptr<StrExpr>;

struct UnaryExpr : public Expr, public std::enable_shared_from_this<UnaryExpr> {
    std::shared_ptr<Token> op_;
    std::shared_ptr<Expr> right_;

    UnaryExpr(std::shared_ptr<Token> op, std::shared_ptr<Expr> right) : op_{std::move(op)}, right_{std::move(right)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitUnaryExpr(shared_from_this());
    }
};

using UnaryExprPtr = std::shared_ptr<UnaryExpr>;

struct VariableExpr : public Expr, public std::enable_shared_from_this<VariableExpr> {
    std::shared_ptr<Token> name_;

    explicit VariableExpr(std::shared_ptr<Token> name) : name_{std::move(name)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitVariableExpr(shared_from_this());
    }
};

using VariableExprPtr = std::shared_ptr<VariableExpr>;

struct LogicalExpr : public Expr, public std::enable_shared_from_this<LogicalExpr> {
    std::shared_ptr<Expr> left_;
    std::shared_ptr<Token> op_;
    std::shared_ptr<Expr> right_;

    LogicalExpr(std::shared_ptr<Expr> left, std::shared_ptr<Token> op, std::shared_ptr<Expr> right)
            : left_{std::move(left)}, op_{std::move(op)}, right_{std::move(right)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitLogicalExpr(shared_from_this());
    }
};

using LogicalExprPtr = std::shared_ptr<LogicalExpr>;

struct CallExpr : public Expr, public std::enable_shared_from_this<CallExpr> {
    std::shared_ptr<Expr> callee_;
    std::shared_ptr<Token> paren_;
    std::shared_ptr<std::vector<ExprPtr>> args_;

    CallExpr(std::shared_ptr<Expr> callee, std::shared_ptr<Token> paren, std::shared_ptr<std::vector<ExprPtr>> args)
            : callee_{std::move(callee)}, paren_{std::move(paren)}, args_{std::move(args)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitCallExpr(shared_from_this());
    }
};

using CallExprPtr = std::shared_ptr<CallExpr>;

struct GetExpr : public Expr, public std::enable_shared_from_this<GetExpr> {
    std::shared_ptr<Expr> expr_;
    std::shared_ptr<Token> name_;

    GetExpr(std::shared_ptr<Expr> expr_, std::shared_ptr<Token> name_) : expr_{std::move(expr_)}, name_{std::move(name_)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitGetExpr(shared_from_this());
    }
};

using GetExprPtr = std::shared_ptr<GetExpr>;

struct SetExpr : public Expr, public std::enable_shared_from_this<SetExpr> {
    std::shared_ptr<Expr> expr_;
    std::shared_ptr<Token> name_;
    std::shared_ptr<Expr> value_;

    SetExpr(std::shared_ptr<Expr> expr_, std::shared_ptr<Token> name_, std::shared_ptr<Expr> value_)
            : expr_{std::move(expr_)}, name_{std::move(name_)}, value_{std::move(value_)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitSetExpr(shared_from_this());
    }
};

using SetExprPtr = std::shared_ptr<SetExpr>;

struct ThisExpr : public Expr, public std::enable_shared_from_this<ThisExpr> {
    std::shared_ptr<Token> keyword_;

    explicit ThisExpr(std::shared_ptr<Token> keyword_) : keyword_{std::move(keyword_)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitThisExpr(shared_from_this());
    }
};

using ThisExprPtr = std::shared_ptr<ThisExpr>;

struct SuperExpr : public Expr, public std::enable_shared_from_this<SuperExpr> {
    std::shared_ptr<Token> keyword_;
    std::shared_ptr<Token> method_;

    SuperExpr(std::shared_ptr<Token> keyword_, std::shared_ptr<Token> method_)
            : keyword_{std::move(keyword_)}, method_{std::move(method_)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitSuperExpr(shared_from_this());
    }
};

using SuperExprPtr = std::shared_ptr<SuperExpr>;
#pragma once

#include <utility>
#include <vector>

#include "expr.hpp"

struct IfStmt;
struct WhileStmt;
struct ContinueStmt;
struct BreakStmt;
struct ForStmt;
struct WhenStmt;
struct BlockStmt;
struct ExpressionStmt;
struct LetStmt;
struct VarStmt;
struct FunctionStmt;
struct ReturnStmt;
struct ClassStmt;

struct Stmt {
    struct AbstractVisitor {
        virtual void visitIfStmt(std::shared_ptr<IfStmt> stmt) = 0;

        virtual void visitForStmt(std::shared_ptr<ForStmt> stmt) = 0;

        virtual void visitWhileStmt(std::shared_ptr<WhileStmt> stmt) = 0;

        virtual void visitContinueStmt(std::shared_ptr<ContinueStmt> stmt) = 0;

        virtual void visitBreakStmt(std::shared_ptr<BreakStmt> stmt) = 0;

        virtual void visitWhenStmt(std::shared_ptr<WhenStmt> stmt) = 0;

        virtual void visitBlockStmt(std::shared_ptr<BlockStmt> stmt) = 0;

        virtual void visitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) = 0;

        virtual void visitLetStmt(std::shared_ptr<LetStmt> stmt) = 0;

        virtual void visitVarStmt(std::shared_ptr<VarStmt> stmt) = 0;

        virtual void visitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) = 0;

        virtual void visitReturnStmt(std::shared_ptr<ReturnStmt> stmt) = 0;

        virtual void visitClassStmt(std::shared_ptr<ClassStmt> stmt) = 0;
    };

    virtual void accept(AbstractVisitor &visitor) = 0;

    virtual ~Stmt() = default;
};

using StmtPtr = std::shared_ptr<Stmt>;

struct IfStmt : public Stmt, public std::enable_shared_from_this<IfStmt> {
    ExprPtr condition_;
    StmtPtr thenStmt_;
    StmtPtr elseStmt_;

    IfStmt(ExprPtr condition, StmtPtr thenStmt, StmtPtr elseStmt)
            : condition_{std::move(condition)}, thenStmt_{std::move(thenStmt)}, elseStmt_{std::move(elseStmt)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitIfStmt(shared_from_this());
    }
};

using IfStmtPtr = std::shared_ptr<IfStmt>;

struct WhileStmt : public Stmt, public std::enable_shared_from_this<WhileStmt> {
    ExprPtr condition_;
    StmtPtr statements_;

    WhileStmt(ExprPtr condition, StmtPtr statements) :
            condition_{std::move(condition)}, statements_{std::move(statements)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitWhileStmt(shared_from_this());
    }
};

using WhileStmtPtr = std::shared_ptr<WhileStmt>;

struct ContinueStmt : public Stmt, public std::enable_shared_from_this<ContinueStmt> {
    TokenPtr keyword_;

    explicit ContinueStmt(TokenPtr keyword_) : keyword_{std::move(keyword_)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitContinueStmt(shared_from_this());
    }
};

using ContinueStmtPtr = std::shared_ptr<ContinueStmt>;

struct BreakStmt : public Stmt, public std::enable_shared_from_this<BreakStmt> {
    TokenPtr keyword_;

    explicit BreakStmt(TokenPtr keyword_) : keyword_{std::move(keyword_)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitBreakStmt(shared_from_this());
    }
};

using BreakStmtPtr = std::shared_ptr<BreakStmt>;

struct ForStmt : public Stmt, public std::enable_shared_from_this<ForStmt> {
    TokenPtr variable_;
    ExprPtr iterable_;
    StmtPtr body_;

    ForStmt(TokenPtr variable_, ExprPtr iterable_, StmtPtr body_)
            : variable_{std::move(variable_)}, iterable_{std::move(iterable_)}, body_{std::move(body_)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitForStmt(shared_from_this());
    }
};

using ForStmtPtr = std::shared_ptr<ForStmt>;

using BranchStmtPtr = std::shared_ptr<std::vector<std::pair<std::vector<ExprPtr>, StmtPtr>>>;

struct WhenStmt : public Stmt, public std::enable_shared_from_this<WhenStmt> {
    BranchStmtPtr branches;
    StmtPtr else_;

    WhenStmt(BranchStmtPtr branches, StmtPtr else_) : branches{std::move(branches)}, else_{std::move(else_)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitWhenStmt(shared_from_this());
    }
};

using WhenStmtPtr = std::shared_ptr<WhenStmt>;

struct BlockStmt : public Stmt, public std::enable_shared_from_this<BlockStmt> {
    std::shared_ptr<std::vector<StmtPtr>> statements_;

    explicit BlockStmt(std::shared_ptr<std::vector<StmtPtr>> statements) : statements_{std::move(statements)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitBlockStmt(shared_from_this());
    }
};

using BlockStmtPtr = std::shared_ptr<BlockStmt>;

struct ExpressionStmt : public Stmt, public std::enable_shared_from_this<ExpressionStmt> {
    ExprPtr expression_;

    explicit ExpressionStmt(ExprPtr expression) : expression_{std::move(expression)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitExpressionStmt(shared_from_this());
    }
};

using ExpressionStmtPtr = std::shared_ptr<ExpressionStmt>;

struct LetStmt : public Stmt, public std::enable_shared_from_this<LetStmt> {
    TokenPtr name_;
    ExprPtr initializer_;

    LetStmt(TokenPtr name, ExprPtr initializer)
            : name_{std::move(name)}, initializer_{std::move(initializer)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitLetStmt(shared_from_this());
    }
};

using LetStmtPtr = std::shared_ptr<LetStmt>;

struct VarStmt : public Stmt, public std::enable_shared_from_this<VarStmt> {
    TokenPtr name_;
    ExprPtr initializer_;

    VarStmt(TokenPtr name, ExprPtr initializer)
            : name_{std::move(name)}, initializer_{std::move(initializer)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitVarStmt(shared_from_this());
    }
};

using VarStmtPtr = std::shared_ptr<VarStmt>;

struct FunctionStmt : public Stmt, public std::enable_shared_from_this<FunctionStmt> {
    TokenPtr name_;
    std::shared_ptr<std::vector<TokenPtr>> params_;
    std::shared_ptr<std::vector<StmtPtr>> body_;

    FunctionStmt(TokenPtr name, std::shared_ptr<std::vector<TokenPtr>> params,
                 std::shared_ptr<std::vector<StmtPtr>> body)
            : name_{std::move(name)}, params_{std::move(params)}, body_{std::move(body)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitFunctionStmt(shared_from_this());
    }
};

using FunctionStmtPtr = std::shared_ptr<FunctionStmt>;

struct ReturnStmt : public Stmt, public std::enable_shared_from_this<ReturnStmt> {
    TokenPtr keyword_;
    ExprPtr value_;

    ReturnStmt(TokenPtr keyword, ExprPtr value)
            : keyword_{std::move(keyword)}, value_{std::move(value)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitReturnStmt(shared_from_this());
    }
};

using ReturnStmtPtr = std::shared_ptr<ReturnStmt>;

struct ClassStmt : public Stmt, public std::enable_shared_from_this<ClassStmt> {
    TokenPtr name_;
    VariableExprPtr superClass_;
    std::shared_ptr<std::vector<FunctionStmtPtr>> methods_;

    ClassStmt(TokenPtr name, VariableExprPtr superClass_, std::shared_ptr<std::vector<FunctionStmtPtr>> methods)
            : name_{std::move(name)}, superClass_{std::move(superClass_)}, methods_{std::move(methods)} {}

    void accept(AbstractVisitor &visitor) override {
        visitor.visitClassStmt(shared_from_this());
    }
};

using ClassStmtPtr = std::shared_ptr<ClassStmt>;


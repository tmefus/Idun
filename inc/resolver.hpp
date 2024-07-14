#pragma once

#include <unordered_map>
#include <memory>
#include <vector>

#include "expr.hpp"
#include "stmt.hpp"
#include "token.hpp"

struct Interpreter;

struct Resolver : public Expr::AbstractVisitor, public Stmt::AbstractVisitor {
public:
    enum class BlockType {
        NONE, LOOP
    };

    enum class FunctionType {
        NONE, FUNCTION, INITIALIZER, METHOD
    };

    enum class ClassType {
        NONE, CLASS, SUBCLASS
    };

private:
    Interpreter &interpreter;
    bool has_error_{false};

    std::vector<std::unordered_map<std::string, bool>> scopes;
    BlockType currentBlock{BlockType::NONE};
    FunctionType currentFunction{FunctionType::NONE};
    ClassType currentClass{ClassType::NONE};

    void resolve(const std::shared_ptr<std::vector<StmtPtr>> &stmts);

    void resolve(const StmtPtr &stmt);

    void resolveLocal(const ExprPtr &expr, const TokenPtr &name);

    void resolveFunction(const FunctionStmtPtr &stmt, FunctionType type);

    void beginScope();

    void endScope();

    void declare(const TokenPtr &name);

    void define(const TokenPtr &name);

public:
    explicit Resolver(Interpreter &interpreter) : interpreter{interpreter} {};

    // Visitor methods for Expressions
    void visitAssignExpr(AssignExprPtr expr) override;

    void visitBinaryExpr(BinaryExprPtr expr) override;

    void visitGroupingExpr(GroupingExprPtr expr) override;

    void visitLiteralExpr(LiteralExprPtr expr) override;

    void visitStrExpr(StrExprPtr expr) override;

    void visitUnaryExpr(UnaryExprPtr expr) override;

    void visitVariableExpr(VariableExprPtr expr) override;

    void visitLogicalExpr(LogicalExprPtr expr) override;

    void visitCallExpr(CallExprPtr expr) override;

    void visitGetExpr(GetExprPtr expr) override;

    void visitSetExpr(SetExprPtr expr) override;

    void visitThisExpr(ThisExprPtr expr) override;

    void visitSuperExpr(SuperExprPtr expr) override;

    // Visitor methods for Statements
    void visitIfStmt(IfStmtPtr stmt) override;

    void visitWhileStmt(WhileStmtPtr stmt) override;

    void visitContinueStmt(ContinueStmtPtr stmt) override;

    void visitBreakStmt(BreakStmtPtr stmt) override;

    void visitForStmt(ForStmtPtr stmt) override;

    void visitWhenStmt(WhenStmtPtr stmt) override;

    void visitBlockStmt(BlockStmtPtr stmt) override;

    void visitExpressionStmt(ExpressionStmtPtr stmt) override;

    void visitLetStmt(LetStmtPtr stmt) override;

    void visitVarStmt(VarStmtPtr stmt) override;

    void visitFunctionStmt(FunctionStmtPtr stmt) override;

    void visitReturnStmt(ReturnStmtPtr stmt) override;

    void visitClassStmt(ClassStmtPtr stmt) override;

    bool resolve(const std::vector<StmtPtr> &ast);

    void resolve(const ExprPtr &expr);
};


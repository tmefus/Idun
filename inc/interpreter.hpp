#pragma once

#include "expr.hpp"
#include "stmt.hpp"
#include "environment.hpp"

#include <vector>
#include <sstream>

struct Interpreter : public Expr::AbstractVisitor, public Stmt::AbstractVisitor {
    explicit Interpreter();

    LoxValuePtr result;

    std::shared_ptr<Environment> global;
    std::shared_ptr<Environment> env;

    std::ostringstream string;  // 字符串拼接时的缓冲区

    std::unordered_map<ExprPtr, int> locals; // 变量及其所属作用域

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

    // Helpers
    static bool isTruth(const LoxValuePtr &value);

    static bool isEqual(const LoxValuePtr &a, const LoxValuePtr &b);

    static bool isBool(const LoxValuePtr &value);

    static bool isNum(const LoxValuePtr &value);

    static bool isFloat(const LoxValuePtr &value);

    static bool isString(const LoxValuePtr &value);

    static int64_t getInt(const LoxValuePtr &value);

    static double getFloat(const LoxValuePtr &value);

    // Checkers
    static void checkNumberOp(TokenPtr op, const LoxValuePtr &value);

    static void checkNumberOps(TokenPtr op, const LoxValuePtr &left, const LoxValuePtr &right);

    LoxValuePtr evaluate(const ExprPtr &expr);

    void execute(const StmtPtr &stmt);

    void executeBlock(const std::shared_ptr<std::vector<StmtPtr>> &statements, std::shared_ptr<Environment> env);

    LoxValuePtr lookupVariable(const TokenPtr &name, const ExprPtr &expr);

    void resolve(const ExprPtr &expr, int depth);

    void interpret(const std::vector<StmtPtr> &statements);
};


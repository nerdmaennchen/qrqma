#pragma once

#include <map>
#include <variant>
#include <vector>
#include <typeinfo>

#include "qrqma/symbol.h"
#include "qrqma/unique_function.h"

namespace qrqma {
namespace actions {

struct Context {
    struct StaticText : std::string {};
    using Callable = unique_func<std::string()>;
    using Token = std::variant<StaticText, Callable>;
    using SymbolTable = std::map<std::string, symbol::Symbol>;

    // a function that eats an any and converts the inner type to something else
    using ConverterFunc = std::function<std::any(std::any const&)>;


    struct Expression {
        using FuncType = qrqma::unique_func<symbol::Symbol()>;

        Expression(std::type_info const& t, FuncType func) 
          : mType{t}, mEval{std::move(func)} {}

        Expression(Expression&& other) noexcept : mType{std::move(other.mType)}, mEval{std::move(other.mEval)} {}

        template<typename T>
        T eval() const { return std::any_cast<T>(eval_f()); }

        std::any eval_f() const { return mEval(); }
        std::type_info const& type() { return mType; }
    private:
        std::type_info const& mType;
        FuncType mEval;
    };

    Context() = default;
    Context(SymbolTable symbols);
    Context(Context const* parent);

    void addToken(Token t);

    void pushExpression(Expression f);
    void pushExpression(std::type_info const& t, Expression::FuncType f) {
        pushExpression(Expression{t, std::move(f)});
    }
    
    Expression popExpression();
    std::vector<Expression> popAllExpressions();
    
    auto expressionStackSize() const {
        return expression_stack.size();
    }

    symbol::Symbol const &operator[](std::string const &name) const;
    symbol::Symbol const* getSymbol(std::string const &name) const;

    void setSymbol(std::string const& name, symbol::Symbol symbol);

    std::string operator()() const;

    static ConverterFunc convert(std::type_info const& from, std::type_info const& to);

    Context& childContext();

private:

    std::vector<std::unique_ptr<Context>> childContexts;
    std::vector<Token> tokens;
    SymbolTable symbols;

    std::vector<Expression> expression_stack;

    Context const* parent{nullptr};
};

}
}

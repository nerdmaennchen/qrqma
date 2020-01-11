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
          : type{t}, eval_f{std::move(func)} {}

        Expression(Expression&& other) noexcept : type{other.type}, eval_f{std::move(other.eval_f)} {}

        template<typename T>
        T eval() const { return std::any_cast<T>(eval_f()); }

        std::type_info const& type;
        FuncType eval_f;
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
    auto expressionStackSize() const {
        return expression_stack.size();
    }

    symbol::Symbol const &operator[](std::string const &name) const;
    std::string operator()() const;

    ConverterFunc convert(std::type_info const& from, std::type_info const& to);

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

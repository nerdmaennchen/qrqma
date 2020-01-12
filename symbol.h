#pragma once

#include "unique_function.h"
#include <functional>
#include <type_traits>
#include <any>

namespace qrqma {
namespace symbol {

using Symbol = std::any;

struct Function {
    using AnyVec = std::vector<std::any>;

    std::function<std::any(AnyVec const &)> f_int;
    std::type_info const& targetType;
    std::vector<std::type_info const*> argTypes;

    std::any operator()(AnyVec const &anyVec) const { return f_int(anyVec); }

    template <typename ArgType> static auto pullArg(int idx) {
        return [=](AnyVec const &anyVec) {
            return std::any_cast<ArgType>(anyVec.at(idx));
        };
    }

    template <typename R, typename... Args>
    Function(std::function<R(Args...)> &&func) : targetType{typeid(R)} {
        using indexes = std::index_sequence_for<Args...>;
        f_int = function_construct_helper<std::tuple<Args...>>(std::move(func), indexes{});
        argTypes = {
            &typeid(std::remove_reference_t<std::remove_cv_t<Args>>)...
        };
    }

    template <typename _Functor,
              typename _Signature = typename detail::__function_guide_helper<
                  decltype(&_Functor::operator())>::type>
    Function(_Functor &&func)
        : Function{std::function<_Signature>(std::forward<_Functor>(func))} {}

private:
    template <typename Tuple, typename F, std::size_t... I>
    decltype(f_int) function_construct_helper(F &&f, std::index_sequence<I...>) {
        return [=](AnyVec const &anyvec) {
            return std::invoke(f, pullArg<std::decay_t<decltype(std::get<I>(
                                      std::declval<Tuple>()))>>(I)(anyvec)...);
        };
    }
};

struct List {
    std::vector<Symbol> list;

    template<typename inner_type> 
    List(std::vector<inner_type> l) {
        for (auto& e : l) {
            list.emplace_back(std::move(e));
        }
    }

    bool empty() const {
        return list.empty();
    }
    auto size() const -> decltype(list.size()) {
        return list.size();
    }

    auto begin() const -> decltype(std::begin(list)) {
        return std::begin(list);
    }
    auto end() const -> decltype(std::end(list)) {
        return std::end(list);
    }
    auto rbegin() const -> decltype(std::rbegin(list)) {
        return std::rbegin(list);
    }
    auto rend() const -> decltype(std::rend(list)) {
        return std::rend(list);
    }

    auto operator[](std::size_t index) -> decltype(list[index])& {
        return list[index];
    }
    auto operator[](std::size_t index) const -> decltype(list[index]) const& {
        return list[index];
    }

};

} // namespace symbol
} // namespace qrqma
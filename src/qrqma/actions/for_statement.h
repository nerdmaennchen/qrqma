#pragma once

#include "context.h"
#include "../grammar/grammar.h"

#include <vector>
#include <string_view>

namespace qrqma {
namespace actions {

namespace pegtl = tao::pegtl;

using SymbolList = std::vector<std::string>;

template <> struct action<grammar::for_control_statement> : pegtl::change_states<SymbolList, Context, Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, Context& context)
    {
        char const* marker = in.current();
        auto& inner_context = context.childContext();

        inner_context.setSymbol("loop.length",    types::Integer{});
        
        inner_context.setSymbol("loop.index0",    types::Integer{});
        inner_context.setSymbol("loop.index",     types::Integer{});
        inner_context.setSymbol("loop.revindex0", types::Integer{});
        inner_context.setSymbol("loop.revindex",  types::Integer{});
        inner_context.setSymbol("loop.first",     types::Bool{});
        inner_context.setSymbol("loop.last",      types::Bool{});

        inner_context.setSymbol("loop.previtem", {});
        inner_context.setSymbol("loop.nextitem", {});

        return pegtl::change_states<SymbolList, Context, Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<3>{}, in, SymbolList{}, context.childContext(), inner_context, context, marker);
    }

    template <typename Input> 
    static void success(const Input &in, SymbolList &symbol_names, Context &container_ctx, Context &inner_context, Context &outer_context, char const* marker) {
        auto iterable = container_ctx.popExpression();
        if (iterable.type() == typeid(symbol::List)) {
            if (symbol_names.size() != 1) {
                throw std::runtime_error("cannot broadcast list items into multiple identifiers: \"" + std::string{marker, std::size_t(in.current()-marker)} + "\"");
            }
            outer_context.addToken([it = std::move(iterable), symName=std::move(symbol_names[0]), &inner_context]{
                auto l = it.eval<symbol::List>();
                std::string ret;
                
                inner_context.setSymbol("loop.length", static_cast<types::Integer>(l.size()));
                types::Integer itCounter {};

                for (auto const& i : l) {
                    inner_context.setSymbol("loop.index0", itCounter);
                    inner_context.setSymbol("loop.index",  itCounter+1);
                    inner_context.setSymbol("loop.revindex0", static_cast<types::Integer>(l.size() - itCounter - 1));
                    inner_context.setSymbol("loop.revindex",  static_cast<types::Integer>(l.size() - itCounter));
                    inner_context.setSymbol("loop.first",  itCounter == 0);
                    inner_context.setSymbol("loop.last",   itCounter == static_cast<types::Integer>(l.size()-1));

                    inner_context.setSymbol("loop.previtem",  itCounter==0?std::any():l[itCounter-1]);
                    inner_context.setSymbol("loop.nextitem",  itCounter==static_cast<types::Integer>(l.size()-1)?std::any():l[itCounter+1]);

                    inner_context.setSymbol(symName, i);
            
                    ret += inner_context();

                    itCounter++;
                }

                return ret;
            });
        } else if (iterable.type() == typeid(symbol::Map)) {
            if (symbol_names.size() > 2) {
                throw std::runtime_error("cannot broadcast list items into more than two identifiers: \"" + std::string{marker, std::size_t(in.current()-marker)} + "\"");
            }
            outer_context.addToken([it = std::move(iterable), symbol_names=std::move(symbol_names), &inner_context]{
                auto m = it.eval<symbol::Map>();
                std::string ret;
                
                inner_context.setSymbol("loop.length", static_cast<types::Integer>(m.size()));
                types::Integer itCounter {};

                for (auto it=begin(m); it!=end(m); ++it) {
                    inner_context.setSymbol("loop.index0", itCounter);
                    inner_context.setSymbol("loop.index",  itCounter+1);
                    inner_context.setSymbol("loop.revindex0", static_cast<types::Integer>(m.size() - itCounter - 1));
                    inner_context.setSymbol("loop.revindex",  static_cast<types::Integer>(m.size() - itCounter));
                    inner_context.setSymbol("loop.first",  itCounter == 0);
                    inner_context.setSymbol("loop.last",   itCounter == static_cast<types::Integer>(m.size()-1));

                    inner_context.setSymbol("loop.previtem",  itCounter==0?std::any():std::prev(it)->first);
                    inner_context.setSymbol("loop.nextitem",  itCounter==static_cast<types::Integer>(m.size()-1)?std::any():std::next(it)->first);

                    inner_context.setSymbol(symbol_names[0], it->first);
                    if (symbol_names.size() == 2) {
                        inner_context.setSymbol(symbol_names[1], it->second);
                    }
            
                    ret += inner_context();

                    itCounter++;
                }

                return ret;
            });
        }
    }
};

template <> struct action<grammar::for_identifier> {
    template <typename Input> 
    static void apply(const Input &in, SymbolList &symbol_names, Context&, Context& inner_context) {
        auto name = in.string();
        inner_context.setSymbol(name, {});
        symbol_names.emplace_back(in.string());
    }
};
template <> struct action<grammar::for_container_identifier> : pegtl::change_states<Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, SymbolList&, Context& container_context, Context&) 
    {
        return pegtl::change_states<Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<1>{}, in, container_context);
    }

    template <typename Input> 
    static void success(const Input &, Context&) { }
};


template <> struct action<grammar::for_content> : pegtl::change_states<Context> {
    template< typename Rule, pegtl::apply_mode A, pegtl::rewind_mode M, template< typename... > class Action, template< typename... > class Control, typename Input>
    [[nodiscard]] static bool match( Input& in, SymbolList&, Context&, Context& inner_context)
    {
        return pegtl::change_states<Context>::match< Rule, A, M, Action, Control >(std::make_index_sequence<1>{}, in, inner_context );
    }

    template <typename Input> 
    static void success(const Input &, Context&) { }
};

}
}
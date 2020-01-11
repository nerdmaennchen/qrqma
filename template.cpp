#include "template.h"

#include "actions/actions.h"

#include "actions/context.h"
#include "grammar/grammar.h"

#include <map>
#include <string>

namespace qrqma {

struct Template::Pimpl {
    actions::Context context;

    Pimpl(actions::Context::SymbolTable symbols)
        : context{std::move(symbols)} {}
};

namespace pegtl = tao::pegtl;
Template::Template(std::string_view input, std::map<std::string, symbol::Symbol> symbols)
    : pimpl{std::make_unique<Pimpl>(std::move(symbols))} 
{
    pegtl::parse<pegtl::seq<grammar::grammar, pegtl::eof>, actions::action>(
        pegtl::memory_input{input, ""}, 
        pimpl->context
    );
}

Template::~Template() {}

std::string Template::operator()() const { return pimpl->context(); }

} // namespace qrqma
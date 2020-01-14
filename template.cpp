#include "template.h"

#include "actions/actions.h"

#include "actions/context.h"
#include "grammar/grammar.h"

#include <map>
#include <string>

namespace qrqma {

struct Template::Pimpl {
    actions::Context context;

    Pimpl(actions::Context::SymbolTable symbols, TemplateLoader loader, symbol::BlockTable blocks)
        : context{std::move(symbols), std::move(blocks)} {
            context.setTemplateLoader(std::move(loader));
        }
};

namespace pegtl = tao::pegtl;
Template::Template(std::string_view input, symbol::SymbolTable symbols, TemplateLoader loader, symbol::BlockTable blocks)
    : pimpl{std::make_unique<Pimpl>(std::move(symbols), std::move(loader), std::move(blocks))} 
{
    pegtl::parse<pegtl::if_must<grammar::grammar, pegtl::eof>, actions::action>(
        pegtl::memory_input{input, ""}, 
        pimpl->context
    );
}

Template::Template(Template&& rhs) {
    std::swap(pimpl, rhs.pimpl);
}

Template& Template::operator=(Template&& rhs) {
    std::swap(pimpl, rhs.pimpl);
    return *this;
}
    

Template::~Template() {}

std::string Template::operator()() const { 
    return pimpl->context().rendered;
}

} // namespace qrqma
#include "extends_statement.h"

#include "types.h"
#include "../template.h"

namespace qrqma {
namespace actions {

void action<grammar::extends_control_statement>::apply(Context& context) {
    auto name = context.popExpression().eval<types::String>();
    
    Context* rootC = &context;
    while (rootC->parentContext()) { rootC = rootC->parentContext(); }
    context.addToken([templ = std::optional<Template>{}, name=std::move(name), rootC]() mutable -> Context::RenderOutput {
        if (not templ) {
            Context::SymbolTable symbols = rootC->getSymbolTable();
            Context::BlockTable blocks = rootC->popBlockTable();
            auto loader = rootC->getTemplateLoader();

            templ.emplace(loader(name), symbols, loader, std::move(blocks));
        }

        return {(*templ)(), true};
    });
}

}
}
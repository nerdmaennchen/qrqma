#include "extends_statement.h"

#include "types.h"
#include "../template.h"

namespace qrqma {
namespace actions {

namespace pegtl = tao::pegtl;


void action<grammar::extends_control_statement>::success(Context& inner_ctx, Context &outer_ctx) {
    auto name = outer_ctx.popExpression().eval<types::String>();
    Context const* rootC = &outer_ctx;
    while (rootC->parentContext()) {
        rootC = rootC->parentContext();
    }
    auto loader = rootC->getTemplateLoader();
    
    Context::SymbolTable symbols = inner_ctx.getSymbolTable();
    for (auto const& [k, v] : outer_ctx.getSymbolTable()) {
        symbols.emplace(k, v);
    }
    Context::BlockTable blocks = inner_ctx.popBlockTable();
    outer_ctx.addToken([templ = Template(loader(name), symbols, loader, std::move(blocks))]() -> std::string {
        return templ();
    });
}

}
}
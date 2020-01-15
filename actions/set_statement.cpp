#include "set_statement.h"
#include "types.h"

namespace qrqma {
namespace actions {

void action<grammar::set_control_statement>::success(std::string &symbol_name, Context& context) {
    auto e = context.popExpression();
    auto& sym = context[symbol_name];
    if (sym.type() == typeid(types::Undefined)) {
        context.setSymbol(symbol_name, e.eval_f());
    } else {
        sym = e.eval_f();
    }
}

}
}
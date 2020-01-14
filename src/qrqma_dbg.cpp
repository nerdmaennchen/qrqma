#include "qrqma_test.h"

#include "qrqma/tao/pegtl.hpp"
#include "qrqma/grammar/grammar.h"
#include "qrqma/demangle.h"

#include "qrqma/actions/actions.h"

namespace pegtl = tao::pegtl;

namespace qrqma_test
{

void qrqma_check();
void qrqma_dbg();
auto check_command = sargp::Command("check", "check the qrqma grammar with pegtl", qrqma_check);

auto dbg_command   = sargp::Command("dbg", "debug an expression", qrqma_dbg);
auto dbg_start     = dbg_command.Parameter<bool>(false, "start", "print start rules");
auto dbg_success   = dbg_command.Parameter<bool>(true, "success", "print success rules");
auto dbg_failure   = dbg_command.Parameter<bool>(false, "failure", "print failure rules");

void qrqma_check() {
    if( pegtl::analyze< qrqma::grammar::grammar >() == 0 ) {
        std::cout << "pegtl::analyze sucessfull" << std::endl;
    }
}

struct Marker_Stack {
    std::vector<char const*> marker_stack;
};

template< typename Rule >
struct normal : pegtl::normal<Rule>
{

    using SuperClass = pegtl::normal<Rule>;

    template< typename Input,
                typename... States >
    static void start( const Input& in, Marker_Stack& state) {
        state.marker_stack.emplace_back(in.current());
        if (*dbg_start) {
            std::string_view sv{in.current(), in.size()};
            std::vector<char> indent = std::vector<char>(static_cast<std::size_t>(state.marker_stack.size()), ' '); indent.back() = '\0';
            std::cout << indent.data() << "start: " << qrqma::internal::demangle(typeid(Rule)) << "\n" << indent.data() << sv << "\n";
        }
        SuperClass::start(in);
    }
    template< typename Input,
                typename... States >
    static void success( const Input& in, Marker_Stack& state) {
        auto marker = state.marker_stack.back();
        state.marker_stack.pop_back();
        if (*dbg_success) {
            std::string_view sv{marker, static_cast<std::size_t>(in.current()-marker)};
            std::vector<char> indent = std::vector<char>(static_cast<std::size_t>(state.marker_stack.size()+1), ' '); indent.back() = '\0';
            std::cout << indent.data() << "success: " << qrqma::internal::demangle(typeid(Rule)) << "\n" << indent.data() << sv << "\n";
        }
        SuperClass::success(in);
    }
    template< typename Input,
                typename... States >
    static void failure( const Input& in, Marker_Stack& state) {
        auto marker = state.marker_stack.back();
        state.marker_stack.pop_back();
        if (*dbg_failure) {
            std::string_view sv{marker, static_cast<std::size_t>(in.current()-marker)};
            std::vector<char> indent = std::vector<char>(static_cast<std::size_t>(state.marker_stack.size()+1), ' '); indent.back() = '\0';
            std::cout << indent.data() << "failure: " << qrqma::internal::demangle(typeid(Rule)) << "\n" << indent.data() << sv << "\n";
        }
        SuperClass::failure(in);
    }
};

void qrqma_dbg() {
    namespace pegtl = tao::pegtl;
    if (template_text) {
        pegtl::parse<pegtl::seq<qrqma::grammar::grammar, pegtl::eof>, pegtl::nothing, normal>(
            pegtl::memory_input{*template_text, ""}, Marker_Stack{}
        );
    }
    if (template_file) {
        auto content = qrqma::defaultLoader()(template_file->native());
        pegtl::parse<pegtl::seq<qrqma::grammar::grammar, pegtl::eof>, pegtl::nothing, normal>(
            pegtl::memory_input{content, ""}, Marker_Stack{}
        );
    }
}

}
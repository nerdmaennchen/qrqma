#pragma once

#include "sargparse/Parameter.h"
#include "sargparse/File.h"

namespace qrqma_test
{

inline auto template_text = sargp::Parameter<std::string>("", "input", "text to be analyzed");
inline auto template_file = sargp::Parameter<std::string>("", "in_file", "file to be analyzed", {}, sargp::completeFile());

}

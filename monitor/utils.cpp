#include <cstdlib>
#include <iostream>

#include "utils.h"


void halt(
  std::optional<std::string_view> reason,
  const int exit_code
) {

  if (reason.has_value()) {
    std::cerr << std::endl << reason.value() << std::endl;		
  }

  exit(exit_code);
}
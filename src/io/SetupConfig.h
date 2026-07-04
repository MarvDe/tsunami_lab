#ifndef TSUNAMI_LAB_IO_SETUP_CONFIG_H
#define TSUNAMI_LAB_IO_SETUP_CONFIG_H

#include "../constants.h"
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace tsunami_lab {
  namespace io {
    enum class ArgType {
      String,
      Int,
      Real,
      Bool,
    };

    using ArgValue = std::variant<std::string, tsunami_lab::t_idx, tsunami_lab::t_real, bool>;

    struct SetupArgDef {
      std::string name;
      ArgType type;
      bool required;
      std::optional<ArgValue> fallback;
    };

    struct SetupDef {
      std::string name;
      std::vector<SetupArgDef> args;
      std::string printTemplate;
    };

    struct SetupArgs {
      std::string name;
      std::unordered_map<std::string, ArgValue> values;

      template<typename T>
      T get(const std::string &i_name) const {
        return std::get<T>(values.at(i_name));
      }
    };
  }
}

#endif

/**
 * @file
 * @brief Data types describing setup-specific configuration arguments.
 */
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
    /** Supported types of setup arguments. */
    enum class ArgType {
      String,
      Int,
      Real,
      Bool,
    };

    //! Runtime value of a setup argument.
    using ArgValue = std::variant<std::string, tsunami_lab::t_idx, tsunami_lab::t_real, bool>;

    /** Definition of one setup argument. */
    struct SetupArgDef {
      std::string name;                 //!< Argument name used in the YAML file.
      ArgType type;                     //!< Expected value type.
      bool required;                    //!< Whether the argument must be specified.
      std::optional<ArgValue> fallback; //!< Optional fallback value.
    };

    /** Definition and printable template of a simulation setup. */
    struct SetupDef {
      std::string name;                 //!< Setup name.
      std::vector<SetupArgDef> args;    //!< Supported setup arguments.
      std::string printTemplate;        //!< Printable YAML template.
    };

    /** Parsed argument values for a selected simulation setup. */
    struct SetupArgs {
      std::string name;                                  //!< Selected setup name.
      std::unordered_map<std::string, ArgValue> values; //!< Values indexed by argument name.

      /**
       * Retrieves a setup argument with the requested type.
       *
       * @tparam T expected value type.
       * @param i_name argument name.
       * @return configured argument value.
       * @throws std::out_of_range if @p i_name is unknown.
       * @throws std::bad_variant_access if the stored value has a different type.
       */
      template<typename T>
      T get(const std::string &i_name) const {
        return std::get<T>(values.at(i_name));
      }
    };
  }
}

#endif

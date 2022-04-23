// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

namespace ScriptPluginUtils {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA
    MCAPI std::unique_ptr<class Scripting::IRuntime> CreateRuntimeByType(enum ScriptRuntimeType, class Scripting::LifetimeRegistry &);
    MCAPI std::string GetFileExtensionFromRuntimeType(enum ScriptRuntimeType);
    MCAPI enum ScriptRuntimeType GetRuntimeType(std::string const &);
    MCAPI enum ScriptRuntimeType GetRuntimeTypeFromFileExtension(std::string const &);
    MCAPI extern std::string const JAVA_SCRIPT_RUNTIME_EXTENSION;
    MCAPI extern std::string const JAVA_SCRIPT_RUNTIME_NAME;
    MCAPI std::string SanitizeRuntimeName(std::string const &);

};
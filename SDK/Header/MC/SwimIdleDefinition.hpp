// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "JsonUtil.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class SwimIdleDefinition {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_SWIMIDLEDEFINITION
public:
    class SwimIdleDefinition& operator=(class SwimIdleDefinition const &) = delete;
    SwimIdleDefinition(class SwimIdleDefinition const &) = delete;
    SwimIdleDefinition() = delete;
#endif

public:
    /*0*/ virtual ~SwimIdleDefinition();
    MCAPI static void buildSchema(std::string const &, class std::shared_ptr<class JsonUtil::JsonSchemaObjectNode<class JsonUtil::EmptyClass, class SwimIdleDefinition>> &);

protected:

private:

};
// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class ActorAnimationController {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_ACTORANIMATIONCONTROLLER
public:
    class ActorAnimationController& operator=(class ActorAnimationController const &) = delete;
    ActorAnimationController(class ActorAnimationController const &) = delete;
    ActorAnimationController() = delete;
#endif

public:
    MCAPI class std::shared_ptr<class ActorAnimationControllerState> & addState(class HashedString const &);
    MCAPI unsigned __int64 findStateIndex(std::string const &, bool, unsigned __int64) const;
    MCAPI void resolveTransitionStateIndices();

protected:

private:

};
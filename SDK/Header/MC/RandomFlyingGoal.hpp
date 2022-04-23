// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"
#include "RandomStrollGoal.hpp"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class RandomFlyingGoal : public RandomStrollGoal {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_RANDOMFLYINGGOAL
public:
    class RandomFlyingGoal& operator=(class RandomFlyingGoal const &) = delete;
    RandomFlyingGoal(class RandomFlyingGoal const &) = delete;
    RandomFlyingGoal() = delete;
#endif

public:
    /*0*/ virtual ~RandomFlyingGoal();
    /*1*/ virtual bool canUse();
    /*3*/ virtual bool canBeInterrupted();
    /*7*/ virtual void appendDebugInfo(std::string &) const;
    /*8*/ virtual void __unk_vfn_8();
    /*9*/ virtual void __unk_vfn_9();
    /*10*/ virtual bool _setWantedPosition();
    MCAPI RandomFlyingGoal(class Mob &, float, int, int, bool);

protected:

private:
    MCAPI bool _getTreePos(class Vec3 &) const;

};
// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class AngryComponent {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_ANGRYCOMPONENT
public:
    class AngryComponent& operator=(class AngryComponent const &) = delete;
    AngryComponent(class AngryComponent const &) = delete;
#endif

public:
    MCAPI AngryComponent(class AngryComponent &&);
    MCAPI AngryComponent();
    MCAPI bool canAttack(class Mob &, class Mob *, class Actor *, bool, bool);
    MCAPI enum LevelSoundEvent const getAngrySound(class Mob const &) const;
    MCAPI bool getBroadcastAnger() const;
    MCAPI bool getBroadcastAngerOnAttack() const;
    MCAPI bool getBroadcastAngerOnBeingAttacked() const;
    MCAPI class ActorFilterGroup const & getBroadcastFilter() const;
    MCAPI int getBroadcastRange() const;
    MCAPI bool getHasTicked() const;
    MCAPI struct Tick const getNextSoundEventTick() const;
    MCAPI class AngryComponent & operator=(class AngryComponent &&);
    MCAPI void restartTimer(class Mob &);
    MCAPI void setAngry(class Mob &, bool);
    MCAPI void setHasTicked(bool);
    MCAPI void setNextSoundEventTick(class Mob const &);
    MCAPI ~AngryComponent();

protected:

private:

};
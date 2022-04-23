// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class SharedAmplifiers {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_SHAREDAMPLIFIERS
public:
    class SharedAmplifiers& operator=(class SharedAmplifiers const &) = delete;
    SharedAmplifiers(class SharedAmplifiers const &) = delete;
    SharedAmplifiers() = delete;
#endif

public:
    MCAPI static class std::shared_ptr<class Amplifier> ABSORPTION;
    MCAPI static class std::shared_ptr<class Amplifier> FATAL_POISON;
    MCAPI static class std::shared_ptr<class Amplifier> HARM;
    MCAPI static class std::shared_ptr<class Amplifier> HEAL;
    MCAPI static class std::shared_ptr<class Amplifier> HUNGER_DURATION;
    MCAPI static class std::shared_ptr<class Amplifier> HUNGER_VALUE;
    MCAPI static class std::shared_ptr<class Amplifier> POISON;
    MCAPI static class std::shared_ptr<class Amplifier> REGENERATION;
    MCAPI static class std::shared_ptr<class Amplifier> SATURATION;
    MCAPI static class std::shared_ptr<class Amplifier> WITHER;

protected:

private:

};
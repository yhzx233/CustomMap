// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA

#undef BEFORE_EXTRA

class VehicleClientPositionPassengerSystem {

#define AFTER_EXTRA

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_VEHICLECLIENTPOSITIONPASSENGERSYSTEM
public:
    class VehicleClientPositionPassengerSystem& operator=(class VehicleClientPositionPassengerSystem const &) = delete;
    VehicleClientPositionPassengerSystem(class VehicleClientPositionPassengerSystem const &) = delete;
    VehicleClientPositionPassengerSystem() = delete;
#endif

public:
    MCAPI static class Vec3 getNewPassengerPosition(class SynchedActorData const &, class Vec3 const &, class Vec2 const &);

protected:

private:

};
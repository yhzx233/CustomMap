// This Header is auto generated by BDSLiteLoader Toolchain
#pragma once
#define AUTO_GENERATED
#include "../Global.h"

#define BEFORE_EXTRA
// Include Headers or Declare Types Here

#undef BEFORE_EXTRA

class DebugAssertException {

#define AFTER_EXTRA
// Add Member There

#undef AFTER_EXTRA

#ifndef DISABLE_CONSTRUCTOR_PREVENTION_DEBUGASSERTEXCEPTION
public:
    class DebugAssertException& operator=(class DebugAssertException const &) = delete;
    DebugAssertException() = delete;
#endif

public:
    /*0*/ virtual ~DebugAssertException();
    /*1*/ virtual char const * what() const;
    /*
    inline  ~DebugAssertException(){
         (DebugAssertException::*rv)();
        *((void**)&rv) = dlsym("??1DebugAssertException@@UEAA@XZ");
        return (this->*rv)();
    }
    */
    MCAPI DebugAssertException(class DebugAssertException const &);
    MCAPI DebugAssertException(char const *, char const *, char const *, int, char const *, char const *);
    MCAPI static bool isEnabled();

protected:

private:
    MCAPI static char const *const WHAT;

};
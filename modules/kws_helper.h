#ifndef KWS_H
#define KWS_H
#include "../micropython/py/builtin.h"
#include "../micropython/py/mphal.h"
#include "../micropython/py/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

// #include "py/runtime.h"  // Assuming this is needed as per your C++ file

// Declare any functions, types, constants, etc., that are used externally
extern mp_obj_t run();
// Add other declarations here if needed

#ifdef __cplusplus
}
#endif

#endif // KWS_H

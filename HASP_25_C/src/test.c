#include "test.h"

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif


EXPORT float square(float x) {
    return x*x;
}

#ifdef __cplusplus
};
#endif

#include "export.h"


#ifdef __cplusplus
extern "C" {  // only need to export C interface if
              // used by C++ source code
#endif


EXPORT float square(float x) __fromfile("test.c"); // fromfile required for linking

#ifdef __cplusplus
};
#endif

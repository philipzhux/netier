#ifndef ERROR_H
#define ERROR_H
#include <stdio.h>
#include <stdlib.h>

namespace netier {

void errif(bool condition, const char *errmsg);
#endif
} // namespace netier

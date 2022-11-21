/*
 * Created on Sun Nov 20 2022
 *
 * Copyright (c) 2022 Philip Zhu Chuyan <me@cyzhu.dev>
 */

#include "error.hpp"

void errif(bool condition, const char *errmsg) {
    if(condition){
        perror(errmsg);
        exit(EXIT_FAILURE);
    }
}
#ifndef clox_compiler_h
#define clox_compiler_h

#include "object.h"
#include "virtualMachine.h"

ObjFunction* compile(const char* source);

#endif

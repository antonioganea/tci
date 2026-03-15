#pragma once

#include <Windows.h>

extern BYTE* DLL_BRIDGE;

extern int* DLL_DETOURED;
extern int* DLL_IS_MAGIC_CALL;
extern int* DLL_COMMAND;
extern int* DLL_TRIGGER_DEBUG_CALL;

extern int* DLL_INTN_IN;
extern int* DLL_INTN_OUT;

extern int* DLL_INTS_IN; // array
extern int* DLL_INTS_OUT; // array

extern int* DLL_FLOATN_IN;
extern int* DLL_FLOATN_OUT;

extern float* DLL_FLOATS_IN; // array
extern float* DLL_FLOATS_OUT; // array

extern int* DLL_STRN_IN;
extern int* DLL_STRN_OUT;
extern int* DLL_STRLEN_IN; // array
extern int* DLL_STRLEN_OUT; // array

extern char** DLL_STRING_LOC;
extern char* DLL_STRING;

extern char** DLL_IN_STR1;
extern char** DLL_IN_STR2;
extern char** DLL_IN_STR3;
extern char** DLL_IN_STR4;

void bindBridgeLayout();
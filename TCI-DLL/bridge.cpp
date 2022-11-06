#include "bridge.h"

BYTE* DLL_BRIDGE = NULL;

int* DLL_DETOURED;
int* DLL_IS_MAGIC_CALL;
int* DLL_COMMAND;
int* DLL_TRIGGER_DEBUG_CALL;

int* DLL_INTN_IN;
int* DLL_INTN_OUT;

int* DLL_INTS_IN; // array
int* DLL_INTS_OUT; // array

int* DLL_FLOATN_IN;
int* DLL_FLOATN_OUT;

float* DLL_FLOATS_IN; // array
float* DLL_FLOATS_OUT; // array

int* DLL_STRN_IN;
int* DLL_STRN_OUT;
int* DLL_STRLEN_IN; // array
int* DLL_STRLEN_OUT; // array


char** DLL_STRING_LOC = NULL;
char* DLL_STRING = NULL;

char** DLL_IN_STR1 = NULL;
char** DLL_IN_STR2 = NULL;
char** DLL_IN_STR3 = NULL;
char** DLL_IN_STR4 = NULL;
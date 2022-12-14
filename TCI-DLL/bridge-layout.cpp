#include "bridge-layout.h"

#include <string.h>



/* BRIDGE LAYOUT INITIALIZATION */

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

/* END OF BRIDGE LAYOUT INITIALIZATION */



void bindBridgeLayout() {
    BYTE* ptr = DLL_BRIDGE + 4 * sizeof(int);

    DLL_DETOURED = (int*)ptr; ptr += sizeof(int);
    DLL_IS_MAGIC_CALL = (int*)ptr; ptr += sizeof(int);
    DLL_COMMAND = (int*)ptr; ptr += sizeof(int);
    DLL_TRIGGER_DEBUG_CALL = (int*)ptr; ptr += sizeof(int);

    DLL_INTN_IN = (int*)ptr; ptr += sizeof(int);
    DLL_INTN_OUT = (int*)ptr; ptr += sizeof(int);

    //ptr += 4; // QWORD alignment

    ptr += 8; // ES Array header
    DLL_INTS_IN = (int*)ptr; ptr += 8 * sizeof(int);

    ptr += 8; // ES Array header
    DLL_INTS_OUT = (int*)ptr; ptr += 8 * sizeof(int);

    DLL_FLOATN_IN = (int*)ptr; ptr += sizeof(int);
    DLL_FLOATN_OUT = (int*)ptr; ptr += sizeof(int);

    ptr += 8; // ES Array header
    DLL_FLOATS_IN = (float*)ptr; ptr += 8 * sizeof(float);

    ptr += 8; // ES Array header
    DLL_FLOATS_OUT = (float*)ptr; ptr += 8 * sizeof(float);

    DLL_STRN_IN = (int*)ptr; ptr += sizeof(int);
    DLL_STRN_OUT = (int*)ptr; ptr += sizeof(int);

    ptr += 8; // ES Array header
    DLL_STRLEN_IN = (int*)ptr; ptr += 4 * sizeof(int);

    ptr += 8; // ES Array header
    DLL_STRLEN_OUT = (int*)ptr; ptr += 4 * sizeof(int);

    //Setting up the DLL_STRING bridge
    DLL_STRING_LOC = (char**)ptr; ptr += 8;
    DLL_STRING = *DLL_STRING_LOC;

    DLL_IN_STR1 = (char**)ptr; ptr += 8;
    DLL_IN_STR2 = (char**)ptr; ptr += 8;
    DLL_IN_STR3 = (char**)ptr; ptr += 8;
    DLL_IN_STR4 = (char**)ptr; ptr += 8;

    // TODO : Sometimes, there has been a spacer (8 bytes) observed between STRLEN_OUT and DLL_STRING_LOC in memory...

    strcpy(DLL_STRING, "Ok this is an interesting string");
}
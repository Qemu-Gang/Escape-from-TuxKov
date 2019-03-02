#pragma once

#include "BaseStruct.h"


// credits for some of this: https://www.unknowncheats.me/forum/1570870-post10.html
enum PropType : int
{
    INT =0,
    FLOAT,
    VECTOR,
    VECTORXY,
    STRING,
    ARRAY,
    QUATERNION,
    INT64,
    DATATABLE,
    NUM_SEND_PROP_TYPES,
    //Possible new ones
    TIME,
    TICKS,
};

inline const char *PropType2String( PropType type ){
    switch( type ){
        case INT:
            return "INT";
        case FLOAT:
            return "FLOAT";
        case VECTOR:
            return "VECTOR";
        case VECTORXY:
            return "VECTORXY";
        case STRING:
            return "STRING";
        case ARRAY:
            return "ARRAY";
        case QUATERNION:
            return "QUATERNION";
        case INT64:
            return "INT64";
        case DATATABLE:
            return "DATATABLE";
        case NUM_SEND_PROP_TYPES:
            return "NUM_SEND_PROP_TYPES";
        case TIME:
            return "TIME";
        case TICKS:
            return "TICKS";
        default:
            return "UNKNOWN PROP-TYPE!";
    }
}

class RecvProp
{
public:
    PropType dataType; //0x0000
    uint32_t offset; //0x0004
    char pad_0008[24]; //0x0008
    class recvTable* dataRecvTable; //0x0020
    char* name; //0x0028
    char pad_0030[32]; //0x0030
    void* proxyFn; //0x0050
};

class propArray
{
public:
    class RecvProp* prop[1024]; //0x0000
};

class RecvTable
{
public:
    char pad_0000[8]; //0x0000
    class propArray* pProps; //0x0008
    int32_t numOfProps; //0x0010
    char pad_0014[4]; //0x0014
};

class ClientClass
{
public:
    void* createFn; //0x0000
    void* createEventFn; //0x0008
    char* networkName; //0x0010
    class RecvTable* recvTable; //0x0018
    class ClientClass* next; //0x0020
    char pad_0028[8]; //0x0028
};
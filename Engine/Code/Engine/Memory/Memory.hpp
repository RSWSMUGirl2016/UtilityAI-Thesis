#pragma once

/************************************************************************/
/*                                                                      */
/* INCLUDE                                                              */
/*                                                                      */
/************************************************************************/
#include "Engine/EngineCommon.hpp"
#include "Engine/DataStructures/LinkedList.hpp"
#include "Engine/Memory/Callstack.hpp"

/************************************************************************/
/*                                                                      */
/* DEFINES AND CONSTANTS                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* MACROS                                                               */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* TYPES                                                                */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* STRUCTS                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* CLASSES                                                              */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* GLOBAL VARIABLES                                                     */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/* FUNCTION PROTOTYPES                                                  */
/*                                                                      */
/************************************************************************/
// void* TrackedAlloc( size_t const size );
// void TrackedFree( 

void ProfileMemoryFrameTick();

void MemTest();

uint GetAllocCount();

uint MemoryProfilerGetTotalByteCount();

uint MemoryProfilerGetAllocationCount();

uint MemoryProfilerHighwaterByteCount();

uint MemoryProfilerGetAllocationsLastFrame();

uint MemoryProfilerGetFreesLastFrame();

void MemoryProfilerLogLiveAllocations();

void ConsoleMemProfLogLiveAlloc();
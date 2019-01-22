/*
 * Copyright (c) 2005-2014 Imperas Software Ltd., www.imperas.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.
 *
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "icm/icmCpuManager.h"

#define SIM_ATTRS (ICM_ATTR_DEFAULT)

// Function Prototypes early declaration
static void parseArgs(int argc, char **argv);

// valid command line
const char *usage = "<application name> [variant] [GDB port] [core name to debug e.g. cpu0]";
// Variables set by arguments
char  *application;                    // the application to load
char  *variant        = "Cortex-A9UP"; // the model variant (default)
Bool  enableDebug     = False;         // set True when debugging selected
#define GDB 1
#define MPD 2
Uns32 autoConsole     = 0;             // set to GDB or MPD to enable debug console
Uns32 portNum         = 0;             // set to a port number for a debug connection
const char  *coreName = "cpu0";        // set to core name to debug

//
// Main routine
//
int main(int argc, char **argv) {

    parseArgs(argc, argv);

    // initialize OVPsim, enabling verbose mode to get statistics at end of execution
    unsigned int icmAttrs = ICM_VERBOSE | ICM_STOP_ON_CTRLC;
    switch (autoConsole) {
        case GDB:
            icmAttrs |= ICM_GDB_CONSOLE;
            break;
        case MPD:
            icmAttrs |= ICM_MPD_CONSOLE;
            break;
        default:
            break;
    }
    icmInit(icmAttrs, enableDebug ? "rsp" : 0, portNum);

    const char *armModel    = icmGetVlnvString(NULL, "arm.ovpworld.org", "processor", "arm", "1.0", "model");
    const char *armSemihost = icmGetVlnvString(NULL, "arm.ovpworld.org", "semihosting", "armNewlib", "1.0", "model");

    icmAttrListP icmAttr = icmNewAttrList();
    icmAddStringAttr(icmAttr, "endian",        "little");
    icmAddStringAttr(icmAttr, "compatibility", "nopSVC");
    icmAddStringAttr(icmAttr, "variant",       variant);
    icmAddStringAttr(icmAttr, "UAL",           "1");

    // create a processor instance
    icmProcessorP processor = icmNewProcessor(
        "cpu0",             // CPU name
        "arm",              // CPU type
        0,                  // CPU cpuId
        0,                  // CPU model flags
        32,                 // address bits
        armModel,           // model file
        "modelAttrs",       // morpher attributes
        SIM_ATTRS,          // attributes
        icmAttr,            // user-defined attributes
        armSemihost,        // semi-hosting file
        "modelAttrs"        // semi-hosting attributes
    );

    // create the processor bus
    icmBusP bus = icmNewBus("bus", 32);

    // connect the processors onto the busses
    icmConnectProcessorBusses(processor, bus, bus);

    // create memory
    icmMemoryP local = icmNewMemory("local", ICM_PRIV_RWX, 0xffffffff);

    // connect the memory onto the busses
    icmConnectMemoryToBus(bus, "mp1", local, 0x00000000);

    if(enableDebug) {
        icmProcessorP proc = icmFindProcessorByName(coreName);
        if(proc) {
            // If there's a processor of this name, set this for debug
            // This is required for debug using a standard GDB via RSP.
            // It's not required for Imperas MPD
            icmDebugThisProcessor(proc);
        }
    }

    // load the application executable file into processor memory space
    if(!icmLoadProcessorMemory(processor, application, ICM_LOAD_DEFAULT, False, True)) {
        return -1;
    }

    // Ignore specific message that compatibility mode causes
    icmIgnoreMessage("ARM_MORPH_BII");

    // simulate the platform
    icmProcessorP final = icmSimulatePlatform();

    // was simulation interrupted or did it complete
    if(final && (icmGetStopReason(final)==ICM_SR_INTERRUPT)) {
        icmPrintf("*** simulation interrupted\n");
    }

    icmTerminate();

    return 0;
}


//
// Parse the argument list and set variables
//
static void parseArgs(int argc, char **argv)
{
    // check for the application program name argument
    if((argc<2) || (argc>5)) {
        icmPrintf(
           "usage: %s %s\n\n",
            argv[0], usage
        );
        exit(1);
    }
    application = argv[1];

    if(argc>=3){
        variant = argv[2];
    }

    // check for debugging requirement
    if(argc>=4){
        enableDebug = True;
        if (!strcmp(argv[3], "autoGDB"))
            autoConsole = GDB;
        else if (!strcmp(argv[3], "autoMPD"))
            autoConsole = MPD;
        else
            sscanf(argv[3], "%d", &portNum);
    }
    if(argc==5){
        coreName = argv[4];
    }

}


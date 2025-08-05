#include "meta_comps.h"
#include "PSI_DMC.h"


HashMap CreateComponentExamples(MArena *a_dest) {
    s32 count = 100;
    HashMap map = InitMap(a_dest, count * 2);
    for (s32 i = 1; i < CT_CNT; ++i) {
        CompType ct = (CompType) i;
        CompMeta *cm = CreateComponent(a_dest, ct, i-1);
        MapPut(&map, ct, cm);
    }

    return map;
}


void InitSimcore() {
    strcpy(&instrument_name[0], (char*) "default_instr_name");
    strcpy(&instrument_source[0], (char*) "default_instr_name");
    instrument_exe = (char*) "default_instr_exe";
}


void RunProgram() {
    TimeFunction;

    // init
    InitSimcore();
    Instrument instr = {};

    // config for the particular instrument, PSI_DMC
    PSI_DMC spec = {};
    Init_PSI_DMC(&spec);
    Config_PSI_DMC(&spec, &instr);
}


void Test() {
    TimeFunction;

    MArena *a = GetContext()->a_life;
    HashMap comps = CreateComponentExamples(a);

    printf("Installed components:\n");
    MapIter iter = {};
    while (CompMeta *comp = (CompMeta*) MapNextVal(&comps, &iter)) {
        printf("%d", comp->type);        
        
        if (comp->type_name.len) {
            StrPrint(" -> ", comp->name, "");
            StrPrint(" (", comp->type_name, ")");
        }
        printf("\n");
    }
}


int main (int argc, char **argv) {
    TimeProgram;
    BaselayerAssertVersion(0, 2, 1);
    CbuiAssertVersion(0, 1, 0);

    if (CLAContainsArg("--help", argc, argv) || CLAContainsArg("-h", argc, argv)) {
        printf("--help:          display help (this text)\n");
        printf("--test:          run test functions\n");
        exit(0);
    }
    else if (CLAContainsArg("--test", argc, argv)) {
        Test();
    }
    else {
        InitBaselayer();
        RunProgram();
    }
}

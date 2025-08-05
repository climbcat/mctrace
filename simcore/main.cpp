#include "../lib/jg_baselayer.h"
#include "simcore.h"
#include "simlib.h"


void RunProgram() {
    TimeFunction;

    printf("Size of neutron struct: %lu vs. %lu\n", sizeof(Neutron), sizeof(NeutronSmall));
}

void Test() {
    printf("Running tests ...\n");
}


int main (int argc, char **argv) {
    TimeProgram;
    BaselayerAssertVersion(0, 1, 1);

    if (CLAContainsArg("--count", argc, argv)) {
        s32 cnt = ParseInt( CLAGetArgValue("--count", argc, argv) );
        printf("count: %d\n", cnt);
        exit(0);
    }
    else if (CLAContainsArg("--help", argc, argv) || CLAContainsArg("-h", argc, argv)) {
        printf("--help:          display help (this text)\n");
        printf("--test:          run test functions\n");
        printf("--count [int]:   an example parameter\n");
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

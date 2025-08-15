
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstddef>

#include "lib/jg_baselayer.h"
//#include "lib/jg_cbui.h"
#include "../cbui/cbui_includes.h"
#include "simcore/simcore.h"
#include "simcore/simlib.h"

#include "src/comps_meta.h"
//#include "src/PSI_DMC.h"
#include "src/PSI_DMC_config.h"


void RunDisplayLoop(Array<Component*> comps) {

    CbuiInit("mctrace", false);

    Perspective persp = ProjectionInit(cbui.plf.width, cbui.plf.height);
    OrbitCamera cam = OrbitCameraInit(persp.aspect);
    Array<Wireframe> objs = InitArray<Wireframe>(cbui.ctx->a_pers, 100);

    Wireframe box = CreateAABox(0.2f, 0.2f, 0.2f);

    Wireframe plane = CreatePlane(10);
    objs.Add(plane);

    while (cbui.running) {
        CbuiFrameStart();
        OrbitCameraUpdate(&cam, cbui.plf.cursorpos.dx, cbui.plf.cursorpos.dy, cbui.plf.left.ended_down, cbui.plf.scroll.yoffset_acc);
        OrbitCameraPan(&cam, persp.fov, persp.aspect, cbui.plf.cursorpos.x_frac, cbui.plf.cursorpos.y_frac, MouseRight().pushed, MouseRight().released);
        // start

        objs.len = 1;

        // (re-) generate the world matrices
        SceneGraphUpdate();

        for (s32 i = 0; i < comps.len; ++i) {
            Component *comp = comps.arr[i];

            Wireframe marker = box;
            marker.color = COLOR_BLUE;
            marker.transform = comp->transform->t_world;
            objs.Add(marker);
        }

        // end 
        Array<Vector3f> segments = WireframeLineSegments(cbui.ctx->a_tmp, objs);
        RenderLineSegmentList(cbui.image_buffer, cam.view, persp.proj, cbui.plf.width, cbui.plf.height, objs, segments);

        CbuiFrameEnd();
    }
    CbuiExit();
}


void RunProgram() {
    TimeFunction;

    MContext *ctx = InitBaselayer();
    SceneGraphInit();

    // init
    Instrument instr = {};

    // config for the particular instrument, PSI_DMC
    PSI_DMC spec = {};
    Init_PSI_DMC(&spec);
    Array<Component*> comps = Config_PSI_DMC(ctx->a_life, &spec, &instr);
    SceneGraphUpdate();

    RunDisplayLoop(comps);
}


void Test() {
    TimeFunction;

    //  This test just creates every component, it doens't even init them:
    //  For that we need 

    MArena *a_life = GetContext()->a_life;
    s32 count = 100;
    HashMap comps = InitMap(a_life, count * 2);
    for (s32 i = 1; i < CT_CNT; ++i) {
        CompType ct = (CompType) i;
        Component *cm = CreateComponent(a_life, ct, i-1, "default_name");
        MapPut(&comps, ct, cm);
    }

    printf("Installed components:\n");
    MapIter iter = {};
    while (Component *comp = (Component*) MapNextVal(&comps, &iter)) {
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

    BaselayerAssertVersion(0, 2, 4);
    CbuiAssertVersion(0, 2, 2);

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


#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstddef>

#include "lib/jg_baselayer.h"
//#include "lib/jg_cbui.h"
#include "../cbui/cbui_includes.h"

#include "simcore/simcore.h"
#include "simcore/simlib.h"
#include "src/mcdis.h" // <- this is our "core amendment" code lives

#include "src/comps_meta.h"
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


    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];

        DisplayComponent(comp);
    }
}


void TestInstrConfig() {
    TimeFunction;

    printf("Testing PSI_DMC_config\n\n");

    // init
    MContext *ctx = InitBaselayer();
    SceneGraphInit();
    Instrument instr = {};

    // run the create, init and configure for each component as defined by PSI_DMC
    PSI_DMC spec = {};
    Init_PSI_DMC(&spec);
    Array<Component*> comps = Config_PSI_DMC(ctx->a_life, &spec, &instr);
    SceneGraphUpdate();

    // rn the vanilla mcdisplay functions
    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];
        DisplayComponent(comp);
    }

    // display the transformed registered by the init/config run, onscreen as boxes
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
        TestInstrConfig();
    }
    else {
        InitBaselayer();
        RunProgram();
    }
}

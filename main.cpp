
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstddef>

#include "lib/jg_baselayer.h"
//#include "lib/jg_cbui.h"
#include "../cbui/cbui_includes.h"

#include "src/mcdis.h"  // <- this is where our "core amendment" code lives
#include "simcore/simcore.h"
#include "simcore/simlib.h"

#include "src/comps_meta.h"
#include "src/PSI_DMC_config.h"


Color ComponentCatToColor(CompCategory cat) {
    switch (cat) {
        case CCAT_sources: return COLOR_RED;
        case CCAT_monitors: return COLOR_GREEN_50;
        case CCAT_contrib: return COLOR_GRAY_50;
        case CCAT_misc: return COLOR_BLACK;
        case CCAT_optics: return COLOR_BLUE;
        case CCAT_samples: return COLOR_RED;
    }

    return COLOR_BLACK;
}


void RunProgram() {
    TimeFunction;

    CbuiInit("mctrace", false);
    SceneGraphInit();

    // config for the particular instrument, PSI_DMC
    Instrument instr = {};
    PSI_DMC spec = {};
    Init_PSI_DMC(&spec);
    Array<Component*> comps = Config_PSI_DMC(cbui.ctx->a_life, &spec, &instr);
    // DBG: displace everything into the minus z: (admittedly, a temp hack)
    Component *first = comps.arr[0];
    first->transform->t_loc = TransformBuildTranslation( { 0, 0, -51 } ) * first->transform->t_loc;

    // consolidate component world matrices
    SceneGraphUpdate();

    // storge for the graphical objects
    Array<Wireframe> objs = InitArray<Wireframe>(cbui.ctx->a_pers, 100);
    Wireframe plane = CreatePlane(10, cbui.ctx->a_pers);
    plane.transform = TransformBuildTranslation( { 0, -0.5f, 0 } );
    objs.Add(plane);


    for (s32 i = 0; i < comps.len; ++i) {
        // component as component
        Component *comp = comps.arr[i];
        printf("%.*s\n", comp->name.len, comp->name.str);
        PrintTransform(g_mcdis_t_world);

        McDisplayNext(cbui.ctx->a_pers, comp->transform->t_world);
        DisplayComponent(comp);        

        // component as wireframe
        // TODO: we could just set the component transform here, and not worry about doing anything to the 
        //      mcdis_* points.:
        Wireframe wf_comp = {};
        wf_comp.transform = Matrix4f_Identity();
        wf_comp.color = ComponentCatToColor(comp->cat);
        wf_comp.segments.arr = g_mcdis_anchors.lst;
        wf_comp.segments.len = g_mcdis_anchors.len;
        wf_comp.segments.max = g_mcdis_anchors.len;
        objs.Add(wf_comp);
    }


    // UI
    Perspective persp = ProjectionInit(cbui.plf.width, cbui.plf.height);
    OrbitCamera cam = OrbitCameraInit(persp.aspect);
    


    while (cbui.running) {
        CbuiFrameStart();
        OrbitCameraUpdate(&cam, cbui.plf.cursorpos.dx, cbui.plf.cursorpos.dy, cbui.plf.left.ended_down, cbui.plf.scroll.yoffset_acc);
        OrbitCameraPan(&cam, persp.fov, persp.aspect, cbui.plf.cursorpos.x_frac, cbui.plf.cursorpos.y_frac, MouseRight().pushed, MouseRight().released);
        // start


        RenderLineSegmentList(cbui.image_buffer, cam.view, persp.proj, cbui.plf.width, cbui.plf.height, objs);


        // end 
        CbuiFrameEnd();
    }
    CbuiExit();


    // try running Finally
    for (s32 i = 0; i < comps.len; ++i) {
        // component as component
        Component *comp = comps.arr[i];

        printf("%.*s\n", comp->name.len, comp->name.str);
        FinallyComponent(comp);        
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
        RenderLineSegmentList(cbui.image_buffer, cam.view, persp.proj, cbui.plf.width, cbui.plf.height, objs);

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

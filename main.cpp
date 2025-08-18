
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


void TestComponentFuncitonsRun() {
    TimeFunction;
    printf("TestComponentFuncitonsRun\n\n");


    // init
    MContext *ctx = InitBaselayer();
    SceneGraphInit();
    Instrument instr = {};


    // create, configure & init
    // (using the PSI_DMC example)
    PSI_DMC spec = {};
    Init_PSI_DMC(&spec);
    Array<Component*> comps = Config_PSI_DMC(ctx->a_life, &spec, &instr);
    SceneGraphUpdate();


    // display
    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];
        McDisplayNext(ctx->a_pers, comp->transform->t_world);

        DisplayComponent(comp);
    }


    // trace
    Neutron particle = {};
    Vector3f n_pos = {};
    Vector3f n_vel = {};
    Matrix4f w2l = {};
    Matrix4f l2w = {};
    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];

        // transform particle into local component coords
        w2l = TransformGetInverse(comp->transform->t_world);

        n_pos = { (f32) particle.x, (f32) particle.y, (f32) particle.z };
        n_pos = TransformPoint(w2l, n_pos);
        particle.x = n_pos.x;
        particle.y = n_pos.y;
        particle.z = n_pos.z;
        n_vel = { (f32) particle.vx, (f32) particle.vy, (f32) particle.vz };
        n_vel = TransformDirection(w2l, n_vel);
        particle.vx = n_vel.x;
        particle.vy = n_vel.y;
        particle.vz = n_vel.z;
        // TODO: the same should be done for s

        // run trace code
        TraceComponent(comp, &particle, &instr);

        // transform particle back into world coords
        l2w = comp->transform->t_world;

        n_pos = { (f32) particle.x, (f32) particle.y, (f32) particle.z };
        n_pos = TransformPoint(w2l, n_pos);
        particle.x = n_pos.x;
        particle.y = n_pos.y;
        particle.z = n_pos.z;
        n_vel = { (f32) particle.vx, (f32) particle.vy, (f32) particle.vz };
        n_vel = TransformDirection(w2l, n_vel);
        particle.vx = n_vel.x;
        particle.vy = n_vel.y;
        particle.vz = n_vel.z;
        // TODO: the same should be done for s
    }


    // finally
    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];

        FinallyComponent(comp);
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
        TestComponentFuncitonsRun();
    }
    else {
        RunProgram();
    }
}

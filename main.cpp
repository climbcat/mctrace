
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstddef>

#include "lib/jg_baselayer.h"
#include "lib/jg_cbui.h"


#define DEBUG_DISPLAY
#define DEBUG_TRACE
#define DEBUG_PLOT


#include "simcore/simcore_types.h"

#include "src/display_hooks.h"
#include "src/trace_hooks.h"
#include "src/plot_hooks.h"

#include "simcore/simcore.h"
#include "simcore/simlib.h"

#include "src/comps_meta.h"
#include "src/PSI_DMC_config.h"


Color ComponentCatToColor(u32 cat) {
    switch ((CompCategory) cat) {
        case CCAT_sources: return COLOR_RED;
        case CCAT_monitors: return COLOR_GREEN_50;
        case CCAT_contrib: return COLOR_GRAY_50;
        case CCAT_misc: return COLOR_BLACK;
        case CCAT_optics: return COLOR_BLUE;
        case CCAT_samples: return COLOR_RED;
    }

    return COLOR_BLACK;
}

struct InstrumentConfig {
    Instrument instr;
    Array<Component*> comps;

    Matrix4f box_t_worold;
    Vector3f box_dims;
};


// TODO: Should this be a code generated function?
//      Actually, this should be compressed into a library function
InstrumentConfig InitAndConfigure_PSI_DMC(MArena *a_dest, s32 ncount) {
    InstrumentConfig instr_conf = {};


    // NOTE: We must set mcncount BEFORE initialization.
    //      This is used by API call mcget_ncount(), and called by some components during init (SourceMaxwell)
    mcset_ncount(ncount);


    // create, configure & init:
    PSI_DMC spec = {};
    Init_PSI_DMC(&spec);
    instr_conf.comps = Config_PSI_DMC(a_dest, &spec, &instr_conf.instr);
    SceneGraphUpdate();


    // TODO: extract into a generic function, independent on the particular instrument (e.g. PSI_DMC)


    // run display & calculate helper matrices:
    Matrix4f t_world_prev = Matrix4f_Identity();
    for (s32 i = 0; i < instr_conf.comps.len; ++i) {
        Component *comp = instr_conf.comps.arr[i];
        McDisplayNext(a_dest, comp->transform->t_world);

        DisplayComponent(comp);

        Matrix4f t_world = comp->transform->t_world;
        comp->t_prev2loc = TransformGetInverse(t_world) * t_world_prev;
        t_world_prev = t_world;

        ComponentSharedHeader *hdr = comp->GetHeader();
        hdr-> position_absolute.x =  comp->transform->t_world.m[0][3];
        hdr-> position_absolute.y =  comp->transform->t_world.m[1][3];
        hdr-> position_absolute.z =  comp->transform->t_world.m[2][3];
        hdr-> position_relative.x =  comp->transform->t_loc.m[0][3];
        hdr-> position_relative.y =  comp->transform->t_loc.m[1][3];
        hdr-> position_relative.z =  comp->transform->t_loc.m[2][3];
        for (u32 i = 0; i < 3; i++) {
            for (u32 j = 0; j < 3; j++) {
                // NOTE: testing will tell if we may need to mirror the rot matrix, or what

                hdr->rotation_absolute[i][j] = comp->transform->t_world.m[i][j];
                hdr->rotation_relative[i][j] = comp->transform->t_loc.m[i][j];
            }
        }
    }

    return instr_conf;
}

Array<Wireframe> DisplayComponents(MArena *a_dest, Array<Component*> comps) {
    // storge for the graphical objects
    Array<Wireframe> objs = InitArray<Wireframe>(cbui.ctx->a_pers, 100);
    Wireframe plane = CreatePlane(10, cbui.ctx->a_pers);
    plane.transform = TransformBuildTranslation( { 0, -0.5f, 0 } );
    objs.Add(plane);


    for (s32 i = 0; i < comps.len; ++i) {
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

    return objs;
}


inline
void ParticleTransform(Matrix4f t, Neutron *n) {
    Vector3f n_pos = { (f32) n->x, (f32) n->y, (f32) n->z };
    n_pos = TransformPoint(t, n_pos);
    n->x = n_pos.x;
    n->y = n_pos.y;
    n->z = n_pos.z;

    Vector3f n_vel = { (f32) n->vx, (f32) n->vy, (f32) n->vz };
    n_vel = TransformDirection(t, n_vel);
    n->vx = n_vel.x;
    n->vy = n_vel.y;
    n->vz = n_vel.z;

    // NOTE: figure out at what point the components start utilizing the spin // s
}

inline
Neutron ParticleImmutableTransform(Matrix4f t, Neutron n) {
    Neutron r = {};
    Vector3f n_pos = { (f32) n.x, (f32) n.y, (f32) n.z };
    n_pos = TransformPoint(t, n_pos);
    r.x = n_pos.x;
    r.y = n_pos.y;
    r.z = n_pos.z;

    Vector3f n_vel = { (f32) n.vx, (f32) n.vy, (f32) n.vz };
    n_vel = TransformDirection(t, n_vel);
    r.vx = n_vel.x;
    r.vy = n_vel.y;
    r.vz = n_vel.z;

    r.t = n.t;
    r.p = n.p;

    return r;
}

void ParticlePrintWorld(Matrix4f t_world, Neutron n) {
    n = ParticleImmutableTransform(t_world, n);
    printf("(%g %g %g, %g %g %g, %g, %g)\n", n.x, n.y, n.z, n.vx, n.vy, n.vz, n.t, n.p);
}

void ParticlePrint(Neutron n) {
    printf("(%g %g %g, %g %g %g, %g, %g)\n", n.x, n.y, n.z, n.vx, n.vy, n.vz, n.t, n.p);
}


struct NeutronTrajectory {
    NeutronTrajectory *next;
    List<Vector3f> event_segments; // these are just pairs of vector3, but they could have been events ...
};


NeutronTrajectory *TraceParticles(MArena *a_trajectories, Array<Component*> comps, Instrument *instr, u32 ncount,u32 DBG_break_after_ncount) {

    NeutronTrajectory *ntrace = (NeutronTrajectory*) ArenaAlloc(a_trajectories, sizeof(NeutronTrajectory));;
    NeutronTrajectory *ntrace_prev = ntrace;
    NeutronTrajectory *ntrace_head = ntrace;


    for (u32 j = 0; j < ncount; ++j) {
        // This sets vz = 1 and p = 1.
        // From the legacy generated code, we got:
        //      particle = mcsetstate(0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, mcgravitation, NULL, mcallowbackprop);
        // (see mcsetstate in simcore.h).
        Neutron n = {};

        // record trajectories 
        ntrace = (NeutronTrajectory*) ArenaAlloc(a_trajectories, sizeof(NeutronTrajectory));
        ntrace->event_segments = InitList<Vector3f>(a_trajectories, 0);
        g_anchors_trace = &ntrace->event_segments;
        g_a_dest_trace = a_trajectories;

        ntrace_prev->next = ntrace;
        ntrace_prev = ntrace;

        g_trace_prev = {};
        g_trace_current = {};


        // trace components
        Vector3f current = {};
        Vector3f prev = {};
        for (s32 i = 0; i < comps.len; ++i) {
            Component *comp = comps.arr[i];
            g_t_world_current_comp = comp->transform->t_world;

            // previous local system -> current local system
            ParticleTransform(comp->t_prev2loc, &n);

            // run trace code
            TraceComponent(comp, &n, instr);

            // record the state after each comp, because there is no guarantee that the component will call SCATTER
            trace_state_ext_hook(n.x, n.y, n.z);

            // break iteration of absorbed particles
            if (n._absorbed) {
                break;
            }
        }

        if (j + 1 == DBG_break_after_ncount) { break; }
    }

    return ntrace_head;
}


void RunProgram() {
    TimeFunction;

    CbuiInit("mctrace", false);
    SceneGraphInit();


    s32 ncount = 1000000;
    InstrumentConfig psi_dmc = InitAndConfigure_PSI_DMC(cbui.ctx->a_pers, ncount);
    Array<Wireframe> objs = DisplayComponents(cbui.ctx->a_pers, psi_dmc.comps);


    // UI
    Perspective persp = ProjectionInit(cbui.plf.width, cbui.plf.height);
    OrbitCamera cam = OrbitCameraInit(persp.aspect);
    

    // TRACE
    NeutronTrajectory *traces_first = TraceParticles(cbui.ctx->a_pers, psi_dmc.comps, &psi_dmc.instr, ncount, 1000);


    // PLOT
    Array<Component*> comps = psi_dmc.comps;
    Array<Component*> monitors = InitArray<Component*>(cbui.ctx->a_pers, comps.len);
    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];

        g_current_monitor = &comp->monitor;
        SaveComponent(comp);

        if (comp->monitor.mon_tpe != MT_NOT) {
            monitors.Add(comp);
        }
    }

    // DISPLAY
    while (cbui.running) {
        CbuiFrameStart();
        OrbitCameraUpdate(&cam, cbui.plf.cursorpos.dx, cbui.plf.cursorpos.dy, cbui.plf.left.ended_down, cbui.plf.scroll.yoffset_acc);
        OrbitCameraPan(&cam, persp.fov, persp.aspect, cbui.plf.cursorpos.x_frac, cbui.plf.cursorpos.y_frac, MouseRight().pushed, MouseRight().released);
        // start


        NeutronTrajectory *traces = traces_first;
        while (traces) {
            for (u32 i = 0; i < traces->event_segments.len / 2; ++i) {
                Vector3f a = traces->event_segments.lst[2*i];
                Vector3f b = traces->event_segments.lst[2*i + 1];

                RenderLineSegment(cbui.image_buffer, cam.view, persp.proj, a, b, cbui.plf.width, cbui.plf.height, COLOR_BLACK);
            }

            traces = traces->next;
        }


        // render
        RenderLineSegmentList(cbui.image_buffer, cam.view, persp.proj, cbui.plf.width, cbui.plf.height, objs);



        // end 
        CbuiFrameEnd();
    }


    // plot 
    for (u32 i = 0; i < monitors.len; ++i) {
        Component *mon = monitors.arr[i];
        MonitorPrint(&mon->monitor);
    }



    CbuiExit();
}


void TestComponentFuncitonsRun() {
    TimeFunction;
    printf("TestComponentFuncitonsRun\n\n");


    // init
    MContext *ctx = InitBaselayer();
    SceneGraphInit();
    Instrument instr = {};


    // NOTE: We must set mcncount BEFORE initialization.
    //      This is used by API call mcget_ncount(), and called by some components during init (SourceMaxwell)
    mcncount = 1000000;


    // create, configure & init:
    PSI_DMC spec = {};
    Init_PSI_DMC(&spec);
    Array<Component*> comps = Config_PSI_DMC(ctx->a_life, &spec, &instr);
    SceneGraphUpdate();


    // run display & calculate helper matrices:
    Matrix4f t_world_prev = Matrix4f_Identity();
    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];
        McDisplayNext(ctx->a_pers, comp->transform->t_world);

        DisplayComponent(comp);

        Matrix4f t_world = comp->transform->t_world;
        comp->t_prev2loc = TransformGetInverse(t_world) * t_world_prev;
        t_world_prev = t_world;

        ComponentSharedHeader *hdr = comp->GetHeader();
        hdr-> position_absolute.x =  comp->transform->t_world.m[0][3];
        hdr-> position_absolute.y =  comp->transform->t_world.m[1][3];
        hdr-> position_absolute.z =  comp->transform->t_world.m[2][3];
        hdr-> position_relative.x =  comp->transform->t_loc.m[0][3];
        hdr-> position_relative.y =  comp->transform->t_loc.m[1][3];
        hdr-> position_relative.z =  comp->transform->t_loc.m[2][3];
        for (u32 i = 0; i < 3; i++) {
            for (u32 j = 0; j < 3; j++) {
                // NOTE: testing will tell if we may need to mirror the rot matrix, or what

                hdr->rotation_absolute[i][j] = comp->transform->t_world.m[i][j];
                hdr->rotation_relative[i][j] = comp->transform->t_loc.m[i][j];
            }
        }
    }


    // trace
    u32 DBG_break_after_ncount = 5;
    for (u32 j = 0; j < mcncount; ++j) {
        Neutron particle = {};

        for (s32 i = 0; i < comps.len; ++i) {
            Component *comp = comps.arr[i];

            ParticlePrintWorld(comp->transform->t_world, particle);

            // previous local system -> current local system
            ParticleTransform(comp->t_prev2loc, &particle);

            // run trace code
            TraceComponent(comp, &particle, &instr);
        }

        if (j + 1 == DBG_break_after_ncount) {
            break;
        }
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

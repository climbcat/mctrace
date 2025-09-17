
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstddef>

//#include "lib/jg_baselayer.h"
#include "../baselayer/baselayer_includes.h"
//#include "lib/jg_cbui.h"
#include "../cbui/cbui_includes.h"


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
#include "src/helpers.h"
#include "src/PSI_DMC_config.h"



struct NeutronTrajectory {
    NeutronTrajectory *next;
    List<Vector3f> event_segments; // these are just pairs of vector3, but they could have been events ...
};

NeutronTrajectory *TraceParticles(MArena *a_trajectories, Array<Component*> comps, Instrument *instr, u32 ncount, u32 ncount_record_as_trajectories) {

    g_do_trace_trajectories = true;
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
        if (j < ncount_record_as_trajectories) {
            // record trajectories
            ntrace = (NeutronTrajectory*) ArenaAlloc(a_trajectories, sizeof(NeutronTrajectory));
            ntrace->event_segments = InitList<Vector3f>(a_trajectories, 0);
            g_anchors_trace = &ntrace->event_segments;
            g_a_dest_trace = a_trajectories;

            ntrace_prev->next = ntrace;
            ntrace_prev = ntrace;

            g_trace_prev = {};
            g_trace_current = {};

        }
        else {
            g_do_trace_trajectories = false;
        }


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
    }

    return ntrace_head;
}


void RunProgram() {
    TimeFunction;

    CbuiInit("mctrace", false);
    Perspective persp = ProjectionInit(cbui.plf.width, cbui.plf.height);
    OrbitCamera cam = OrbitCameraInit(persp.aspect);
    // TODO: scenegraph init might be included in CbuiInit

    s32 ncount = 1e6;
    InstrumentConfig config = {};
    config.scenegraph = SceneGraphInit(cbui.ctx->a_pers);
    config.comps = InitAndConfig_PSI_DMC(cbui.ctx->a_pers, &config.instr, &config.scenegraph, ncount);


    // scene objects
    Array<Wireframe> scene_objs = InitArray<Wireframe>(cbui.ctx->a_pers, 100);
    Wireframe plane = CreatePlane(10);
    plane.transform = TransformBuildTranslation( { 0, -0.5f, 0 } );
    WireframeRawSegments(cbui.ctx->a_pers, &plane);
    scene_objs.Add(plane);

    // create component wireframes
    DisplayComponents(cbui.ctx->a_pers, config.comps);


    // TRACE
    NeutronTrajectory *traces_first = TraceParticles(cbui.ctx->a_pers, config.comps, &config.instr, ncount, 100);


    // PLOT
    Array<Component*> comps = config.comps;
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
        OrbitCameraRotateZoom(&cam, cbui.plf.cursorpos.dx, cbui.plf.cursorpos.dy, cbui.plf.left.ended_down, cbui.plf.scroll.yoffset_acc);
        OrbitCameraPanInPlane(&cam, persp.fov, persp.aspect, cbui.plf.cursorpos.x_frac, cbui.plf.cursorpos.y_frac, MouseRight().pushed, MouseRight().released);


        // render neutron trajectories
        {
            NeutronTrajectory *traces = traces_first;
            while (traces) {
                for (u32 i = 0; i < traces->event_segments.len / 2; ++i) {
                    Vector3f a = traces->event_segments.lst[2*i];
                    Vector3f b = traces->event_segments.lst[2*i + 1];

                    RenderLineSegment(cbui.image_buffer, TransformGetInverse(cam.view), persp, a, b, cbui.plf.width, cbui.plf.height, COLOR_BLACK);
                }

                traces = traces->next;
            }
        }


        // render wireframes
        {
            // render scene objects
            for (s32 i = 0; i < scene_objs.len; ++i) {
                RenderWireframe(cbui.image_buffer, cam.view, persp, cbui.plf.width, cbui.plf.height, scene_objs.arr[i]);
            }

            // render components
            for (s32 i = 0; i < comps.len; ++i) {
                RenderWireframe(cbui.image_buffer, cam.view, persp, cbui.plf.width, cbui.plf.height, comps.arr[i]->display);
            }
        }


        // render monitors
        {
            UI_LayoutVertical();
            UI_LayoutHorizontal();

            // labels
            for (u32 i = 0; i < monitors.len; ++i) {
                Component *mon = monitors.arr[i];

                if (mon->monitor.mon_tpe == MT_2D) {
                    Str text = StrCat(mon->name, " | ");
                    Widget *lbl = UI_Label( (const char*) StrZ(text) );
                    lbl->sz_font = FS_18;
                }
            }

            UI_Pop();
            UI_LayoutHorizontal();

            // blit plot into some panels
            for (u32 i = 0; i < monitors.len; ++i) {
                Component *mon = monitors.arr[i];

                if (mon->monitor.mon_tpe == MT_2D) {
                    Str text = StrCat(mon->name, "_pnl");

                    Widget *w = WidgetGetCached( (const char*) StrZ(text) );
                    w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
                    w->w = mon->monitor.binm_x;
                    w->h = mon->monitor.binn_y;
                    w->sz_border = 0;
                    w->col_bckgrnd = COLOR_WHITE;
                    w->col_bckgrnd.a = 0;
                    w->col_border = ColorGray(0.7f);
                    WidgetTreeSibling(w);

                    // TODO: we want to express this as a sprite, which will then get properly blitted during FrameEnd
                    MonitorBlit(cbui.ctx->a_tmp, mon, mon->monitor, w->x0, w->y0, cbui.plf.width, cbui.plf.height, (Color*) cbui.image_buffer);
                }
            }
        }


        CbuiFrameEnd();
    }

    CbuiExit();
}


#include "test/test.cpp"


int main (int argc, char **argv) {
    TimeProgram;

    BaselayerAssertVersion(0, 2, 5);
    CbuiAssertVersion(0, 2, 3);

    bool force_test = false;

    if (CLAContainsArg("--help", argc, argv) || CLAContainsArg("-h", argc, argv)) {
        printf("--help:          display help (this text)\n");
        printf("--test:          run test functions\n");
        exit(0);
    }
    else if (CLAContainsArg("--test", argc, argv) || force_test) {
        Test();
    }
    else {
        RunProgram();
    }
}

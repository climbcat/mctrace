
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


void RenderTrajectories(NeutronTrajectory *traces, Matrix4f view, Perspective persp) {
    while (traces) {
        for (u32 i = 0; i < traces->event_segments.len / 2; ++i) {
            Vector3f a = traces->event_segments.lst[2*i];
            Vector3f b = traces->event_segments.lst[2*i + 1];

            RenderLineSegment(cbui.image_buffer, TransformGetInverse(view), persp, a, b, cbui.plf.width, cbui.plf.height, COLOR_BLACK);
        }

        traces = traces->next;
    }
}

void RenderWireframes(Array<Wireframe> objs, Matrix4f view, Perspective persp) {
    for (s32 i = 0; i < objs.len; ++i) {
        RenderWireframe(cbui.image_buffer, view, persp, cbui.plf.width, cbui.plf.height, objs.arr[i]);
    }
}

void RenderMonitors(Array<Monitor> monitors) {
    UI_LayoutVertical();
    UI_LayoutHorizontal();

    // labels
    for (u32 i = 0; i < monitors.len; ++i) {
        Monitor mon = monitors.arr[i];

        if (mon.mon_tpe == MT_2D) {
            Str text = StrCat(mon.comp_name, " | ");
            Widget *lbl = UI_Label( (const char*) StrZ(text) );
            lbl->sz_font = FS_18;
        }
    }

    UI_Pop();
    UI_LayoutHorizontal();

    // blit plot into some panels
    for (u32 i = 0; i < monitors.len; ++i) {
        Monitor mon = monitors.arr[i];

        if (mon.mon_tpe == MT_2D) {
            Str text = StrCat(mon.comp_name, "_pnl");

            Widget *w = WidgetGetCached( (const char*) StrZ(text) );
            w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
            w->w = mon.binm_x;
            w->h = mon.binn_y;
            w->sz_border = 0;
            w->col_bckgrnd = COLOR_WHITE;
            w->col_bckgrnd.a = 0;
            w->col_border = ColorGray(0.7f);
            WidgetTreeSibling(w);

            // TODO: we want to express this as a sprite, which will then get properly blitted during FrameEnd
            MonitorBlit(cbui.ctx->a_tmp, mon, w->x0, w->y0, cbui.plf.width, cbui.plf.height, (Color*) cbui.image_buffer);
        }
    }
}


Array<Monitor> PlotSaveAndGetMonitors(MArena *a_dest, Array<Component*> comps) {
    List<Monitor> monitors = InitList<Monitor>(a_dest, 0);
    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];

        g_current_monitor = &comp->monitor;
        SaveComponent(comp);

        // NOTE: not sure where this should be initialized, probably in the component_create generated function 
        // TODO: init this field in generated code
        comp->monitor.comp_name = comp->name;

        if (comp->monitor.mon_tpe != MT_NOT) {
            ArenaAlloc(a_dest, sizeof(Monitor));
            monitors.Add(comp->monitor);
        }
    }

    Array<Monitor> result = {};
    result.arr = monitors.lst;
    result.len = monitors.len;
    result.max = monitors.len;

    return result;
}


void RunProgram() {
    TimeFunction;

    CbuiInit("mctrace", true);
    Perspective persp = ProjectionInit(cbui.plf.width, cbui.plf.height);
    OrbitCamera cam = OrbitCameraInit(persp.aspect);

    //s32 ncount = 1e6;
    s32 ncount = 1e5;
    InstrumentConfig config = InitAndConfig_PSI_DMC(cbui.ctx->a_pers, ncount);

    // scene objects
    Array<Wireframe> scene_objs = InitArray<Wireframe>(cbui.ctx->a_pers, 100);
    Wireframe plane = CreatePlaneDetailed(10, 60, 6);

    Vector3f v_inst_center = { 0, -0.5f, 25 };
    plane.transform = TransformBuildTranslation( v_inst_center );

    WireframeRawSegments(cbui.ctx->a_pers, &plane);
    scene_objs.Add(plane);

    cam.center = v_inst_center;
    cam.radius = v_inst_center.z * 2;
    cam.phi = 0;
    cam.theta = 25;


    // get DISPLAY/TRACE data and PLOT data pointers
    DisplayComponents(cbui.ctx->a_pers, config.comps);
    NeutronTrajectory *traces_first = TraceParticles(cbui.ctx->a_pers, config.comps, &config.instr, ncount, 100);
    Array<Monitor> monitors = PlotSaveAndGetMonitors(cbui.ctx->a_pers, config.comps);


    // app display
    while (cbui.running) {
        // frame start
        CbuiFrameStart();
        OrbitCameraRotateZoom(&cam, cbui.plf.cursorpos.dx, cbui.plf.cursorpos.dy, cbui.plf.left.ended_down, cbui.plf.scroll.yoffset_acc);
        OrbitCameraPanInPlane(&cam, persp.fov, persp.aspect, cbui.plf.cursorpos.x_frac, cbui.plf.cursorpos.y_frac, MouseRight().pushed, MouseRight().released);
        scene_objs.len = 0;
        scene_objs.Add(plane);

        // swoop up component wireframes for rendering
        for (s32 i = 0; i < config.comps.len; ++i) {
            Wireframe wf = config.comps.arr[i]->display;
            scene_objs.Add(wf);

            if (wf.type == WFT_SEGMENTS) {
                scene_objs.Add( CreateAABoundingBox(cbui.ctx->a_tmp, wf) );
            }
        }

        // render calls
        RenderTrajectories(traces_first, cam.view, persp);
        RenderWireframes(scene_objs, cam.view, persp);
        RenderMonitors(monitors);
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

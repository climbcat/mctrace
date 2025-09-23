
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

void DisplayComponents(MArena *a_dest, Array<Component*> comps) {
    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];

        printf("%.*s\n", comp->name.len, comp->name.str);
        PrintTransform(g_mcdis_t_world);

        McDisplayNext(cbui.ctx->a_pers, Matrix4f_Identity());
        DisplayComponent(comp);

        comp->display = {};
        comp->display.type = WFT_SEGMENTS;
        comp->display.transform = comp->transform->t_world;
        comp->display.color = ComponentCatToColor(comp->cat);
        comp->display.segments.arr = g_mcdis_anchors.lst;
        comp->display.segments.len = g_mcdis_anchors.len;
        comp->display.segments.max = g_mcdis_anchors.len;
        comp->display.CalculateAABox();
    }
}

void RenderTrajectories(NeutronTrajectory *traces, Matrix4f view, Perspective persp) {
    Color traj_col = COLOR_GRAY_75;
    while (traces) {
        for (u32 i = 0; i < traces->event_segments.len / 2; ++i) {
            Vector3f a = traces->event_segments.lst[2*i];
            Vector3f b = traces->event_segments.lst[2*i + 1];

            RenderLineSegment(cbui.image_buffer, TransformGetInverse(view), persp, a, b, cbui.plf.width, cbui.plf.height, traj_col);
        }

        traces = traces->next;
    }
}

void RenderWireframes(Array<Wireframe> objs, Matrix4f view, Perspective persp) {
    for (s32 i = 0; i < objs.len; ++i) {
        RenderWireframe(cbui.image_buffer, view, persp, cbui.plf.width, cbui.plf.height, objs.arr[i]);
    }
}

Component *RenderMonitors(Array<Monitor> monitors) {
    UI_LayoutVertical();
    UI_SpaceV(10);
    UI_LayoutHorizontal();
    UI_SpaceH(10);

    s32 plot_area_width = 128;
    s32 plot_area_height = 128;

    Component *result_clicked = NULL;

    // labels
    for (u32 i = 0; i < monitors.len; ++i) {
        Monitor mon = monitors.arr[i];

        if (mon.mon_tpe == MT_2D) {
            Widget *l = WidgetGetCached( (const char*) StrZ(StrCat(mon.comp_name, "_pnl")) );
            WidgetTreeBranch(l);
            l->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
            l->SetFlag(WF_CAN_COLLIDE);
            l->SetFlag(WF_LAYOUT_VERTICAL);
            l->SetFlag(WF_ALIGN_CENTER);
            l->sz_border = 1;
            l->col_border = COLOR_BLACK;
            l->col_bckgrnd = COLOR_WHITE;
            l->w = plot_area_height * 1.4;
            l->h = plot_area_width * 1.4;
            if (l->hot) {
                l->col_border = COLOR_RED;
            }
            if (l->clicked) {
                result_clicked = (Component*) mon.comp;
            }

            Widget *lbl = UI_Label( (const char*) StrZ(StrCat(mon.comp_name, " ")) );
            lbl->sz_font = FS_18;
            UI_SpaceV(10);

            Widget *w = WidgetGetCached( (const char*) StrZ(StrCat(mon.comp_name, "_plot")) );
            w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
            w->w = plot_area_width;
            w->h = plot_area_height;
            w->sz_border = 0;
            w->col_bckgrnd = COLOR_WHITE;
            w->col_bckgrnd.a = 0;
            w->col_border = ColorGray(0.7f);
            WidgetTreeSibling(w);

            // TODO: we want to express this as a sprite, which will then get properly blitted during FrameEnd
            //
            // jg-250922: This can be done by pushing a sprite with SpriteBufferPush().
            //      However, we still need a way to push it to the "top" of the stack.
            //      This also works with a "texture" that has a "texture id". This means registering textures,
            //      per-frame or persistently. It is fine to put it on the appropriate lifescale areana, but 
            //      we also need to register the texture with an ID in a persistent hashmap; How should it be
            //      de-registered, then?
            //
            //      Temporary hack: We store the widget in our Monitor and draw it at the right time.
            MonitorBlit(cbui.ctx->a_tmp, mon, w->x0, w->y0, plot_area_width, plot_area_height, cbui.plf.width, cbui.plf.height, (Color*) cbui.image_buffer);
            UI_SpaceV(10);

            UI_Pop();
            UI_SpaceH(10);
        }
    }

    return result_clicked;
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
        comp->monitor.comp = comp;

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


enum McTraceMode {
    MTM_UNDEF,
    MTM_TRACE,

    MTM_CNT
};


struct McTraceApp {
    bool do_display;
    bool do_plot;
    bool do_rays;
    bool do_plane;
    McTraceMode mode;
    Component *comp_selected = NULL;
};


void RunProgram() {
    TimeFunction;

    bool do_fullscreen = false;
    CbuiInit("mctrace", do_fullscreen);
    Perspective persp = ProjectionInit(cbui.plf.width, cbui.plf.height);
    OrbitCamera cam = OrbitCameraInit();

    //s32 ncount = 1e6;
    s32 ncount = 1e5;
    InstrumentConfig config = InitAndConfig_PSI_DMC(cbui.ctx->a_pers, ncount);

    // scene objects
    Array<Wireframe> scene_objs = InitArray<Wireframe>(cbui.ctx->a_pers, 100);
    Wireframe plane = CreatePlaneDetailed(10, 60, 6);

    Vector3f v_instr_center = { 0, -0.5f, 25 };
    plane.transform = TransformBuildTranslation( v_instr_center );

    WireframeRawSegments(cbui.ctx->a_pers, &plane);
    scene_objs.Add(plane);

    v_instr_center.y = 0;
    cam.radius = v_instr_center.z * 1.5;
    cam.phi = 0;
    cam.theta = 25;
    cam.Update(v_instr_center);

    // get DISPLAY/TRACE data and PLOT data pointers
    DisplayComponents(cbui.ctx->a_pers, config.comps);
    NeutronTrajectory *traces_first = TraceParticles(cbui.ctx->a_pers, config.comps, &config.instr, ncount, 100);
    Array<Monitor> monitors = PlotSaveAndGetMonitors(cbui.ctx->a_pers, config.comps);


    McTraceApp app = {};
    app.mode = MTM_TRACE;
    app.do_display = true;
    app.do_plane = true;
    app.do_rays = true;
    app.do_plot = false;


    // app display
    while (cbui.running) {
        // frame start
        CbuiFrameStart();
        PerspectiveSetAspectAndP(&persp, cbui.plf.width, cbui.plf.height);
        OrbitCameraRotateZoom(&cam, cbui.plf.cursorpos.dx, cbui.plf.cursorpos.dy, cbui.plf.left.ended_down, cbui.plf.scroll.yoffset_acc);
        OrbitCameraPanInPlane(&cam, persp.fov, persp.aspect, cbui.plf.cursorpos.x_frac, cbui.plf.cursorpos.y_frac, MouseRight().pushed, MouseRight().released);
        scene_objs.len = 0;

        // swoop up component wireframes for rendering
        bool collided_this_frame = false;
        Button lft = MouseLeft();
        for (s32 i = 0; i < config.comps.len; ++i) {
            Component *comp = config.comps.arr[i];
            Wireframe wf = comp->display;
            scene_objs.Add(wf);


            if (app.do_plot == false && wf.type == WFT_SEGMENTS && comp->interactable) {
                Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, wf, 0.02f);
                box.color = Color { 74, 78, 121, 128 };

                Ray mouse_ray = CameraGetRayWorld(cam.view, persp.fov, persp.aspect, cbui.plf.cursorpos.x_frac, cbui.plf.cursorpos.y_frac);
                bool collided = BoxCollideSLAB(mouse_ray, box);

                if (collided && (collided_this_frame == false)) {
                    collided_this_frame = true;

                    if (lft.dblclicked) {
                        box.style = WFR_FAT;
                        cam.SetRelativeTo(box.transform, box.SizeBallpark() * 1.25f);
                        app.comp_selected = comp;
                    }
                    else if (lft.clicked) {
                        box.style = WFR_FAT;
                        app.comp_selected = comp;
                    }

                    scene_objs.Add(box);
                }

                if (comp == app.comp_selected) {
                    if (collided) {
                        scene_objs.Pop();
                    }

                    box.style = WFR_FAT;
                    scene_objs.Add(box);
                }
            }
        }

        if (app.do_plot == false && collided_this_frame == false && lft.clicked) {
            app.comp_selected = NULL;
        }

        if (GetChar('p')) { app.do_plot = !app.do_plot; }
        if (GetChar('l')) { app.do_plane = !app.do_plane; }
        if (GetChar('r')) { app.do_rays = !app.do_rays; }
        if (GetChar('d')) { app.do_display = !app.do_display; }

        // render calls
        if (app.do_display) {
            if (app.do_rays) {
                RenderTrajectories(traces_first, cam.view, persp);
            }

            if (app.do_plane) {
                scene_objs.Add(plane);
            }

            if (app.do_plot) {
                Component *monitor_clicked = RenderMonitors(monitors);

                if (monitor_clicked) {
                    app.comp_selected = monitor_clicked;
                }
                if (app.comp_selected && app.comp_selected->monitor.mon_tpe == MT_2D) {

                    Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, app.comp_selected->display, 0.02f);
                    //box.color = Color { 74, 78, 121, 128 };
                    box.color = COLOR_RED;
                    scene_objs.Add(box);

                    if (monitor_clicked) {
                        cam.SetRelativeTo(box.transform, box.SizeBallpark() * 2);
                    }
                }
            }

            RenderWireframes(scene_objs, cam.view, persp);
        }
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

#ifndef __MCTRACE__
#define __MCTRACE__


#include "../simcore/simcore_types.h"


#define MCT_COLOR_SELECTION_BOX         COLOR_GRAY_75
#define MCT_COLOR_MONITOR               COLOR_RED
#define MCT_COLOR_TRAJECTORY            COLOR_GRAY_75
#define MCT_COLOR_OPTICS                COLOR_BLUE
#define MCT_COLOR_SOURCE_OR_SAMPLE      COLOR_RED
#define MCT_COLOR_DEFOCUSED             COLOR_GRAY_30


//
//  Types


enum McTraceMode {
    MTM_UNDEF,

    MTM_SIM,
    MTM_TRACE,
    MTM_MONITORS,
    MTM_PLOT,

    MTM_CNT
};

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

struct ColorSheme {
    Color monitors;
    Color optics;
    Color sourcesample;
    Color selection;
    Color rays;

    void SetToMode(McTraceMode mode) {
        if (mode == MTM_MONITORS) {
            monitors = MCT_COLOR_MONITOR;
            optics = MCT_COLOR_DEFOCUSED;
            sourcesample = MCT_COLOR_DEFOCUSED;

            selection = MCT_COLOR_DEFOCUSED;
            rays = MCT_COLOR_TRAJECTORY;
        }
        else {
            monitors = ComponentCatToColor(CCAT_monitors);
            optics = ComponentCatToColor(CCAT_optics);
            sourcesample = ComponentCatToColor(CCAT_sources);

            selection = MCT_COLOR_DEFOCUSED;
            rays = MCT_COLOR_TRAJECTORY;
        }
    }
};

struct McTraceApp {
    Perspective persp;
    OrbitCamera cam;
    Array<Wireframe> scene_objs;
    Wireframe plane;

    InstrumentConfig *config;

    bool simulation_active;
    bool trace_active;
    s32 *ncount_target;
    s32 *ncount_current;

    bool draw_rays;
    bool draw_plane;
    u32 draw_rays_limit;

    McTraceMode mode;
    ColorSheme colors;

    Component *comp_selected = NULL;
    Component *comp_hover = NULL;
    Component *comp_clicked = NULL;
    Component *comp_dbl_clicked = NULL;
};


// forward declarations
void OnSwitchToMode(McTraceApp *app);
Array<Monitor> PlotSaveAndGetMonitors(MArena *a_dest, Array<Component*> comps);
void GetComponentDisplayWireframes(MArena *a_dest, Array<Component*> comps);


void McTraceSetConfig(McTraceApp *app, InstrumentConfig *config) {
    app->config = config;
    app->ncount_target = &config->instr.ncount_target;
    app->ncount_current = &config->instr.ncount_current;
}


McTraceApp McTraceInit(InstrumentConfig *config) {
    McTraceApp app = {};

    // core counters
    app.persp = ProjectionInit(cbui.plf.width, cbui.plf.height);
    app.cam = OrbitCameraInit();

    // scene objects
    app.scene_objs = InitArray<Wireframe>(cbui.ctx->a_pers, 100);
    app.plane = CreatePlaneDetailed(10, 60, 6);
    app.plane.color = MCT_COLOR_TRAJECTORY;

    Vector3f v_instr_center = { 0, -0.5f, 25 };
    app.plane.transform = TransformBuildTranslation( v_instr_center);

    WireframeRawSegments(cbui.ctx->a_pers, &app.plane);
    app.scene_objs.Add(app.plane);

    v_instr_center.y = 0;
    app.cam.radius = v_instr_center.z * 1.5;
    app.cam.phi = 0;
    app.cam.theta = 25;
    app.cam.Update(v_instr_center);

    app.mode = MTM_TRACE;
    app.draw_plane = true;
    app.draw_rays = true;

    McTraceSetConfig(&app, config);
    OnSwitchToMode(&app);

    app.trace_active = false;
    app.simulation_active = false;
    app.draw_rays_limit = 100;

    return app;
}


//
//  Component "display" wireframes


void GetComponentDisplayWireframes(MArena *a_dest, Array<Component*> comps) {
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


//
//  Monitor data / plot


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

u32 RenderTrajectories(NeutronTrajectory *traces, Matrix4f view, Perspective persp, Color trajectory_color, u32 max_trajectories) {
    u32 trajectories_rendered = 0;

    while (traces) {
        for (u32 i = 0; i < traces->events.len - 1; ++i) {
            f32 lambda = traces->events.lst[i].lambda;
            Vector3f a = traces->events.lst[i].pos;
            Vector3f b = traces->events.lst[i + 1].pos;

            f32 lambda_color_value = (lambda - lambda_min) / (lambda_max - lambda_min);
            trajectory_color = ColorMapGet(lambda_color_value, colormap_paletted_jet);

            RenderLineSegment(cbui.image_buffer, TransformGetInverse(view), persp, a, b, cbui.plf.width, cbui.plf.height, trajectory_color);
        }

        trajectories_rendered++;
        if ((max_trajectories > 0) && (trajectories_rendered >= max_trajectories)) {
            break;
        }

        traces = traces->next;
    }
    return trajectories_rendered;
}

void DoRendering(McTraceApp *app) {
    if (app->draw_plane) {
        app->scene_objs.Add(app->plane);
    }

    if (app->draw_rays && app->draw_rays_limit > 0) {
        TrajContainer *container = &app->config->container;

        if (app->comp_selected) {
            u32 selected_idx = app->comp_selected->GetHeader()->index;

            Traj *first_at_comp_idx = container->bundles_ptrs.arr[selected_idx]->head;
            RenderTrajectories(first_at_comp_idx, app->cam.view, app->persp, MCT_COLOR_TRAJECTORY, app->draw_rays_limit);
        }
        else {
            u32 trajs_rendered = 0;
            for (s32 i = 0; i < container->bundles_ptrs.len; ++i) {
                if (trajs_rendered >= app->draw_rays_limit) {
                    break;
                }

                Traj *first_at_comp_idx = container->bundles_ptrs.arr[i]->head;
                trajs_rendered += RenderTrajectories(first_at_comp_idx, app->cam.view, app->persp, MCT_COLOR_TRAJECTORY, 0);
            }
        }
    }

    RenderWireframes(app->scene_objs, app->cam.view, app->persp);
}


#endif

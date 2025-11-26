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
    InstrumentConfig config;
    Array<Wireframe> scene_objs;
    Wireframe plane;
    Array<Monitor> monitors;

    u32 ncount_init;
    TrajContainer container;

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

void OnSwitchToMode(McTraceApp *app);
Array<Monitor> PlotSaveAndGetMonitors(MArena *a_dest, Array<Component*> comps);
void GetComponentDisplayWireframes(MArena *a_dest, Array<Component*> comps);

McTraceApp McTraceInit() {
    McTraceApp app = {};

    app.persp = ProjectionInit(cbui.plf.width, cbui.plf.height);
    app.cam = OrbitCameraInit();

    app.ncount_init = 1e6;
    app.config = InitAndConfig_PSI_DMC(cbui.ctx->a_pers, app.ncount_init);

    // scene objects
    app.scene_objs = InitArray<Wireframe>(cbui.ctx->a_pers, 100);
    app.plane = CreatePlaneDetailed(10, 60, 6);
    app.plane.color = MCT_COLOR_TRAJECTORY;

    Vector3f v_instr_center = { 0, -0.5f, 25 };
    app.plane.transform = TransformBuildTranslation( v_instr_center);

    WireframeRawSegments(cbui.ctx->a_pers, &app.plane);
    app.scene_objs.Add(app.plane);

    // trajectories
    app.container = InitTrajectoryContainer(cbui.ctx->a_pers, app.config.comps.len, 256 * KILOBYTE);
    app.draw_rays_limit = 300;

    // monitors
    // get DISPLAY/TRACE data and PLOT data pointers
    app.monitors = PlotSaveAndGetMonitors(cbui.ctx->a_pers, app.config.comps);
    GetComponentDisplayWireframes(cbui.ctx->a_pers, app.config.comps);

    s32 ncomps = app.config.comps.len;
    Array<bool> is_interactible = InitArray<bool>(cbui.ctx->a_pers, ncomps);
    Array<bool> is_monitors = InitArray<bool>(cbui.ctx->a_pers, ncomps);
    for (s32 i = 0; i < ncomps; ++i) {
        is_interactible.Add(app.config.comps.arr[i]->interactable);
    }
    for (s32 i = 0; i < ncomps; ++i) {
        CompMonitorType montpe = app.config.comps.arr[i]->monitor.mon_tpe;
        is_monitors.Add(montpe != MT_NOT);
    }
    app.config.comps_interactible = is_interactible;
    app.config.comps_monitors = is_monitors;

    v_instr_center.y = 0;
    app.cam.radius = v_instr_center.z * 1.5;
    app.cam.phi = 0;
    app.cam.theta = 25;
    app.cam.Update(v_instr_center);

    app.mode = MTM_TRACE;
    app.draw_plane = true;
    app.draw_rays = true;

    OnSwitchToMode(&app);

    return app;
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


//
//  Particle traces


void TraceParticles(TrajContainer *container, Array<Component*> comps, Instrument *instr, u32 ncount) {
    MArena a_thread = ArenaCreate();
    g_a_dest_trace = &a_thread;
    g_do_trace_trajectories = true;


    for (u32 j = 0; j < ncount; ++j) {
        if (cbui.running == false) {
            break;
        }

        // This sets vz = 1 and p = 1.
        // From the legacy generated code, we got:
        //      particle = mcsetstate(0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, mcgravitation, NULL, mcallowbackprop);
        // (see mcsetstate in simcore.h).
        Neutron n = {};

        u32 compo_idx_max = 0;

        // record trajectories
        if (container->full == false) {
            u32 idx = container->current_idx;
            TrajBundle *bundle = container->bundles_ptrs.arr[idx];

            g_anchors_trace = InitList<Vector3f>(&a_thread, 0);

            g_trace_current = {};
        }
        else {
            g_do_trace_trajectories = false;
        }

        // trace components
        Vector3f current = {};
        Vector3f prev = {};
        for (s32 i = 0; i < comps.len; ++i) {
            compo_idx_max = i;

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

        // update core "current" ncount
        mcncount = j;

        if (g_do_trace_trajectories) {
            Traj t = {};
            t.comp_idx_max = compo_idx_max;
            t.events = g_anchors_trace;

            PushTrajectory(container, t);
            g_anchors_trace.len = 0;
        }
    }
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
            Vector3f a = traces->events.lst[i];
            Vector3f b = traces->events.lst[i + 1];

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

        if (app->comp_selected) {
            u32 selected_idx = app->comp_selected->GetHeader()->index;

            Traj *first_at_comp_idx = app->container.bundles_ptrs.arr[selected_idx]->head;
            RenderTrajectories(first_at_comp_idx, app->cam.view, app->persp, MCT_COLOR_TRAJECTORY, app->draw_rays_limit);
        }
        else {
            u32 trajs_rendered = 0;
            for (s32 i = 0; i < app->container.bundles_ptrs.len; ++i) {
                if (trajs_rendered >= app->draw_rays_limit) {
                    break;
                }

                Traj *first_at_comp_idx = app->container.bundles_ptrs.arr[i]->head;
                trajs_rendered += RenderTrajectories(first_at_comp_idx, app->cam.view, app->persp, MCT_COLOR_TRAJECTORY, 0);
            }
        }
    }

    RenderWireframes(app->scene_objs, app->cam.view, app->persp);
}


#endif

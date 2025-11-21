#ifndef __TRACE_HOOKS_H__
#define __TRACE_HOOKS_H__


static MArena *g_a_dest_trace;
static List<Vector3f> g_anchors_trace;
static Matrix4f g_t_world_current_comp;
static Vector3f g_trace_current;

static bool g_do_trace_trajectories;


void TraceState(double x, double y, double z) {
    if (g_do_trace_trajectories) {

        g_trace_current = TransformPoint(g_t_world_current_comp, Vector3f { (f32) x, (f32) y, (f32) z });
        ArenaAlloc(g_a_dest_trace, 2 * sizeof(Vector3f));
        g_anchors_trace.Add( g_trace_current );
    }
}


#define trace_state_ext_hook TraceState
#define trace_scatter_ext_hook TraceState


#endif

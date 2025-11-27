#ifndef __SIMBOX_H__
#define __SIMBOX_H__


//
//  Particle traces


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

void TraceParticles(TrajContainer *container, bool *run_pause, Array<Component*> comps, Instrument *instr, s32 *ncount_target, s32 *ncount_current) {
    assert(ncount_target);
    assert(ncount_current);

    MArena a_thread = ArenaCreate();
    g_a_dest_trace = &a_thread;
    g_do_trace_trajectories = true;

    u32 j = 0;
    while (*ncount_current < *ncount_target) {

        if (cbui.running == false) {
            break;
        }
        if (*run_pause == false) {
            XSleep(10);
            continue;
        }
        (*ncount_current)++;


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

        if (g_do_trace_trajectories) {
            Traj t = {};
            t.comp_idx_max = compo_idx_max;
            t.events = g_anchors_trace;

            PushTrajectory(container, t);
            g_anchors_trace.len = 0;
        }
    }
}


void SimulateParticles(bool *run_pause, Array<Component*> comps, Instrument *instr, s32 *ncount_target, s32 *ncount_current) {
    assert(ncount_target);
    assert(ncount_current);

    g_do_trace_trajectories = false;

    u32 j = 0;
    while (*ncount_current < *ncount_target) {

        if (cbui.running == false) {
            break;
        }
        if (*run_pause == false) {
            XSleep(10);
            continue;
        }
        (*ncount_current)++;


        // This sets vz = 1 and p = 1.
        // From the legacy generated code, we got:
        //      particle = mcsetstate(0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, mcgravitation, NULL, mcallowbackprop);
        // (see mcsetstate in simcore.h).
        Neutron n = {};

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
}



#include <thread>

// TODO: impl.

struct SimBox {

};

void RunSimulationContinuously(TrajContainer *container, bool *active) {
    std::thread worker = std::thread( 
        [active, container] 
        () 
    {
        while (*active == true) {
            // ...
        }
    });
}


#endif

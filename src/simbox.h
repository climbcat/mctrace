#ifndef __SIMBOX_H__
#define __SIMBOX_H__


struct Traj {
    Traj *next;
    List<Vector3f> events;
    u32 comp_idx_max;
};
typedef Traj NeutronTrajectory;

struct TrajBundle {
    MArena mem;
    Traj *head;
    Traj *current;

    u32 event_cnt;
    u32 traj_cnt;
    u32 comp_idx;
    bool full;
};

struct TrajContainer {
    Array<TrajBundle*> bundles_ptrs;
    u32 events_cnt;
    u32 traj_cnt;
    u32 current_idx;
    bool full;

    bool CheckIfFull() {
        // NOTE: check at strategic times to close it

        for (s32 i = 0; i < bundles_ptrs.len; ++i) {
            if (bundles_ptrs.arr[i]->full == false) {
                return false;
            }
        }
        full = true;
        return true;
    }

    void PrintOccupancy(bool verbose = false) {
        printf("Container: %d events, %d rays", events_cnt, traj_cnt);
        if (full) {
            printf(" - FULL");
        }
        printf("\n");

        if (verbose == true) {
            for (s32 i = 0; i < bundles_ptrs.len; ++i) {
                printf("    Bundle %d: %d events, %d rays", i, bundles_ptrs.arr[i]->event_cnt, bundles_ptrs.arr[i]->traj_cnt);
                if (bundles_ptrs.arr[i]->full) {
                    printf(" - FULL");
                }
                printf("\n");
            }
        }
    }
};

TrajBundle *TrajBundleInit(MArena *a_dest, u32 block_size, u32 comp_idx) {
    MArena a = ArenaSubInit(a_dest, block_size);

    TrajBundle *bundle = (TrajBundle*) ArenaAlloc(&a, sizeof(TrajBundle));
    bundle->mem = a;
    bundle->comp_idx = comp_idx;

    return bundle;
}

void TrajectoryContainerClear(TrajContainer *container) {
    container->current_idx = 0;
    container->events_cnt = 0;
    container->traj_cnt = 0;
    container->full = false;

    for (s32 i = 0; i < container->bundles_ptrs.len; ++i) {
        TrajBundle *bundle = container->bundles_ptrs.arr[i];
        bundle->mem.used = sizeof(TrajBundle);
        bundle->head = NULL;
        bundle->current = NULL;
        bundle->event_cnt = 0;
        bundle->traj_cnt = 0;
        bundle->full = false;
    }
}

TrajContainer TrajectoryContainerInit(MArena *a_dest, u32 bundle_count, u32 block_size = 64 * KILOBYTE) {
    TrajContainer container = {};
    container.bundles_ptrs = InitArray<TrajBundle*>(a_dest, bundle_count);

    for (u32 i = 0; i < bundle_count; ++i) {
        TrajBundle *bundle = TrajBundleInit(a_dest, block_size, i);
        container.bundles_ptrs.Add(bundle);
    }

    return container;
}

bool ArenaHasEnoughSpace(MArena *a, u64 requested) {
    if (a->fixed_size) {
        u64 available = a->fixed_size - a->used;
        bool is = available >= requested;
        return is;
    }
    else {
        return true;
    }
}

bool PushTrajectory(TrajBundle *bundle, Traj traj) {
    if (bundle->full) {
        return false;
    }
    u32 traj_size = sizeof(Traj) + sizeof(Vector3f) * traj.events.len;

    if (ArenaHasEnoughSpace(&bundle->mem, traj_size)) {
        Traj *tpushed = (Traj *) ArenaPush(&bundle->mem, &traj, sizeof(Traj));

        // NOTE: the len should not be set until the copy is done; how can we ensure this?
        tpushed->events.lst = (Vector3f*) ArenaPush(&bundle->mem, traj.events.lst, sizeof(Vector3f) * traj.events.len);
        tpushed->events.len = traj.events.len;

        if (bundle->head == NULL) {
            bundle->head = tpushed;
        }
        else  {
            bundle->current->next = tpushed;
        }
        bundle->current = tpushed;
        bundle->traj_cnt++;
        bundle->event_cnt += traj.events.len;

        return true;
    }
    else {
        bundle->full = true;
        return false;
    }
}

bool PushTrajectory(TrajContainer *container, Traj traj) {
    u32 bundles_cnt = container->bundles_ptrs.len;
    assert(traj.comp_idx_max <= bundles_cnt && "Traj container initialization problem"); 

    if (container->full == true) {
        return false;
    }

    else {
        TrajBundle* bundle = container->bundles_ptrs.arr[container->current_idx];
        if (bundle->full && (container->current_idx < bundles_cnt -1)) {
            container->current_idx++;

            bundle = container->bundles_ptrs.arr[container->current_idx];
        }
        else if (bundle->full && (container->current_idx == (bundles_cnt - 1))) {
            container->CheckIfFull();
            if (container->full) {
                return false;
            }
        }
        assert(bundle->full == false && "sanity check the current fill-strategy");

        if (traj.comp_idx_max < container->current_idx) {
            return false;
        }

        bool could_push = PushTrajectory(bundle, traj);
        if (could_push) {
            container->events_cnt += traj.events.len; 
            container->traj_cnt += 1;
        }

        return could_push;
    }
}


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

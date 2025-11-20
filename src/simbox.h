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
};

MArena ArenaSubInit(MArena *a_dest, u64 size) {
    MArena a = {};
    a.fixed_size = size;
    a.mapped = size;
    a.committed = size;
    a.mem = (u8*) ArenaAlloc(a_dest, size);
    return a;
}

TrajBundle *InitTrajBundle(MArena *a_dest, u32 block_size, u32 comp_idx) {
    MArena a = ArenaSubInit(a_dest, block_size);

    TrajBundle *bundle = (TrajBundle*) ArenaAlloc(&a, sizeof(TrajBundle));
    bundle->mem = a;
    bundle->comp_idx = comp_idx;

    return bundle;
}

TrajContainer InitTrajectoryContainer(MArena *a_dest, u32 bundle_count) {
    TrajContainer container = {};
    container.bundles_ptrs = InitArray<TrajBundle*>(a_dest, bundle_count);

    for (u32 i = 0; i < bundle_count; ++i) {
        TrajBundle *bundle = InitTrajBundle(a_dest, 16 * KILOBYTE, i);
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
        // TODO: this won't be good enough for large arenas (they are notinfinite, after all)
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
        return false;
    }
}

bool PushTrajectory(TrajContainer *container, Traj traj) {
    assert(traj.comp_idx_max <= container->bundles_ptrs.len && "Traj container initialization problem"); 

    u32 idx = MinU32(traj.comp_idx_max, container->current_idx);

    TrajBundle* bundle = container->bundles_ptrs.arr[idx];

    bool did_push = PushTrajectory(bundle, traj);

    // increment the target bundle index
    if (did_push == false && traj.comp_idx_max > container->current_idx) {
        container->current_idx++;
        PushTrajectory(bundle, traj);
    }

    return did_push;
}


bool PushTrajectory2(TrajContainer *container, Traj traj) {
    assert(traj.comp_idx_max <= container->bundles_ptrs.len && "Traj container initialization problem"); 

    //u32 idx = MinU32(traj.comp_idx_max, container->current_idx);
    u32 idx = traj.comp_idx_max;

    TrajBundle* bundle = container->bundles_ptrs.arr[idx];

    bool did_push = PushTrajectory(bundle, traj);

    /*
    // increment the target bundle index
    if (did_push == false && traj.comp_idx_max > container->current_idx) {
        container->current_idx++;
        PushTrajectory(bundle, traj);
    }
    */

    return did_push;
}


/*
#include <thread>

struct SimBox {

};

void RunSimulationContinuously(bool *active) {
    std::thread thread = std::thread( [active] () {
        while (*active == true) {



        }
    } );
}
*/


#endif

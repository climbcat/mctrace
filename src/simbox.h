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

TrajContainer InitTrajectoryContainer(MArena *a_dest, u32 bundle_count, u32 block_size = 64 * KILOBYTE) {
    TrajContainer container = {};
    container.bundles_ptrs = InitArray<TrajBundle*>(a_dest, bundle_count);

    for (u32 i = 0; i < bundle_count; ++i) {
        TrajBundle *bundle = InitTrajBundle(a_dest, block_size, i);
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
        u32 bundle_idx_to_push = traj.comp_idx_max;
        TrajBundle* bundle = container->bundles_ptrs.arr[bundle_idx_to_push];

        while (bundle->full == true) {
            if (bundle_idx_to_push == (bundles_cnt - 1) && bundle->full == true) {
                container->CheckIfFull();
                if (container->full) {
                    return false;
                }
            }

            if (bundle_idx_to_push > 0) {
                bundle_idx_to_push--;
            }

            else {
                // discard thsi trajectory

                assert(traj.comp_idx_max < bundles_cnt && "sanity check the CheckIfFull() logics above");
                return false;
            }

            bundle = container->bundles_ptrs.arr[bundle_idx_to_push];
        }
        bool did_push = PushTrajectory(bundle, traj);

        if (did_push) {
            container->events_cnt += traj.events.len; 
            container->traj_cnt += 1;
        }

        return did_push;
    }
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


#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cstddef>

#include "lib/jg_baselayer.h"


// ***************** include in jg_baselayer 0.2.5: ***********************************
MPool PoolCreate(MArena *a_dest, u32 block_size_min, u32 nblocks) {
    assert(nblocks > 1);

    MPool p = {};
    p.block_size = MPOOL_MIN_BLOCK_SIZE * (block_size_min / MPOOL_MIN_BLOCK_SIZE + 1);
    p.nblocks = nblocks;
    p.lock = (u64) &p; // this "magic" number is a lifetime constant, checked at allocation time
    p.mem = (u8*) ArenaAlloc(a_dest, p.block_size * p.nblocks);

    MPoolBlockHdr *freeblck = &p.free_list;
    for (u32 i = 0; i < nblocks; ++i) {
        freeblck->next = (MPoolBlockHdr*) (p.mem + i * p.block_size);
        freeblck->lock = p.lock;
        freeblck = freeblck->next;
    }
    freeblck->next = NULL;

    return p;
}

MArena ArenaSubInit(MArena *a_dest, u64 size) {
    MArena a = {};
    a.fixed_size = size;
    a.mapped = size;
    a.committed = size;
    a.mem = (u8*) ArenaAlloc(a_dest, size);
    return a;
}
// ************************************************************************************


//#include "lib/jg_cbui.h"
#include "../cbui/cbui_includes.h"


// ***************** include in jg_cbui: **********************************************
void RenderWireframes(Array<Wireframe> objs, Matrix4f view, Perspective persp) {
    for (s32 i = 0; i < objs.len; ++i) {
        RenderWireframe(cbui.image_buffer, view, persp, cbui.plf.width, cbui.plf.height, objs.arr[i]);
    }
}
Vector2f PointToScreen(Vector3f point, Matrix4f view_l2w, Perspective persp, u32 screen_width, u32 screen_height) {
    Matrix4f view_w2l = TransformGetInverse(view_l2w);
    Vector3f p_cam = TransformPoint(view_w2l, point);
    Vector3f p_ndc = TransformPerspective(persp.proj, p_cam);

    Vector2f p_screen = {};
    p_screen.x = (p_ndc.x + 1) / 2 * screen_width;
    p_screen.y = (p_ndc.y + 1) / 2 * screen_height;

    return p_screen;
}
struct Rect32 {
    s32 width;
    s32 height;
    s32 left;
    s32 top;

    void Print() {
        printf("rect: w: %u, h: %u, left: %d, top: %d\n", width, height, left, top);
    }
};
// ************************************************************************************


#define DEBUG_DISPLAY
#define DEBUG_TRACE
#define DEBUG_PLOT


#include "simcore/simcore_types.h"
#include "simcore/display_hooks.h"
#include "simcore/trace_hooks.h"
#include "simcore/plot_hooks.h"
#include "simcore/simcore.h"
#include "simcore/simlib.h"

#include "src/ptrajs.h"
#include "src/comps_meta.h"
#include "src/comps_helpers.h"
#include "src/comps/PSI_DMC_config.h"

#include "src/simbox.h"
#include "src/mctrace.h"
#include "src/ui.h"


enum InstrConfigs {
    IC_UNDEV,
    IC_PSI_DMC,

    IC_CNT
};

void _SelectorPrint(Array<bool> selector) {
    for (s32 i = 0; i < selector.len; ++i) {
        if ((i > 0) && (i % 4 == 0)) {
            printf(" ");
        }
        printf("%d", selector.arr[i]);
    }
    printf("\n");
}

InstrumentConfig InitInstrument(MArena *a_dest, InstrConfigs ic, u32 ncount) {
    InstrumentConfig config = {};
    config.scenegraph = SceneGraphInit(cbui.ctx->a_life);
    config.instr.ncount_target = ncount;

    switch (ic) {
    case IC_PSI_DMC:
        config.instr.name = (char*) "PSI_DMC";
        config.comps = InitAndConfig_PSI_DMC(a_dest, &config.instr, &config.scenegraph, ncount); break;

    default:
        break;
    }

    SceneGraphUpdate(&config.scenegraph);
    UpdateLegacyTransforms(config.comps);
    config.container = TrajectoryContainerInit(a_dest, config.comps.len, 256 * KILOBYTE);

    // monitors helper array
    // get DISPLAY/TRACE data and PLOT data pointers
    config.monitors = PlotSaveAndGetMonitors(a_dest, config.comps);
    GetComponentDisplayWireframes(a_dest, config.comps);

    // setup scrolling arrays
    s32 ncomps = config.comps.len;
    Array<bool> is_interactible = InitArray<bool>(a_dest, ncomps);
    Array<bool> is_monitors = InitArray<bool>(a_dest, ncomps);
    for (s32 i = 0; i < ncomps; ++i) {
        is_interactible.Add(config.comps.arr[i]->interactable);
    }
    for (s32 i = 0; i < ncomps; ++i) {
        CompMonitorType montpe = config.comps.arr[i]->monitor.mon_tpe;
        is_monitors.Add(montpe != MT_NOT);
    }
    config.comps_interactible = is_interactible;
    config.comps_monitors = is_monitors;

    return config;
}


void RunProgram(bool do_fullscreen) {
    TimeFunction;

    CbuiInit("mctrace", do_fullscreen);

    s32 ncount = 1e9;
    InstrumentConfig psi_dmc = InitInstrument(cbui.ctx->a_pers, IC_PSI_DMC, ncount);
    InstrumentConfig psi_dmc_alt = InitInstrument(cbui.ctx->a_pers, IC_PSI_DMC, ncount);
    psi_dmc.next = &psi_dmc_alt;
    psi_dmc_alt.prev = &psi_dmc;

    McTraceApp app = McTraceInit(&psi_dmc);

    std::thread trace_worker = std::thread(TraceParticles, &app.config->container, &app.trace_active, app.config->comps, &app.config->instr, app.ncount_target, app.ncount_current);
    std::thread sim_worker = std::thread(SimulateParticles, &app.simulation_active, app.config->comps, &app.config->instr, app.ncount_target, app.ncount_current);

    // app display
    while (cbui.running) {
        // frame start
        CbuiFrameStart();

        if (UI_DidCollide() == false) {
            PerspectiveSetAspectAndP(&app.persp, cbui.plf.width, cbui.plf.height);
            OrbitCameraRotateZoom(&app.cam, cbui.plf.cursorpos.dx, cbui.plf.cursorpos.dy, cbui.plf.left.ended_down, cbui.plf.scroll.yoffset_acc);
            OrbitCameraPanInPlane(&app.cam, app.persp.fov, app.persp.aspect, cbui.plf.cursorpos.x_frac, cbui.plf.cursorpos.y_frac, MouseRight().pushed, MouseRight().released);
        }

        DoUI(&app);
        DoRendering(&app);

        CbuiFrameEnd();
    }
    CbuiExit();

    trace_worker.join();
    sim_worker.join();
}


#include "test/test.cpp"


int main (int argc, char **argv) {
    TimeProgram;

    BaselayerAssertVersion(0, 2, 4);
    CbuiAssertVersion(0, 2, 3);

    bool force_test = false;
    bool do_fullscreen = false;

    if (CLAContainsArg("--fullscreen", argc, argv) || CLAContainsArg("-f", argc, argv)) {
        do_fullscreen = true;
    }

    if (CLAContainsArg("--help", argc, argv) || CLAContainsArg("-h", argc, argv)) {
        printf("--help / -h         display help (this text)\n");
        printf("--fullscreen / -f   display help (this text)\n");
        printf("--test              run test functions\n");
        exit(0);
    }
    else if (CLAContainsArg("--test", argc, argv) || force_test) {
        Test();
    }
    else {
        RunProgram(do_fullscreen);
    }
}


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


#include "src/comps_meta.h"
#include "src/comps_helpers.h"
#include "src/comps/PSI_DMC_config.h"

#include "src/mctrace.h"
#include "src/ui.h"


void RunProgram(bool do_fullscreen) {
    TimeFunction;

    CbuiInit("mctrace", do_fullscreen);

    McTraceApp app = McTraceInit();

    s32 ncount = 1e5;

    // get DISPLAY/TRACE data and PLOT data pointers
    GetComponentDisplayWireframes(cbui.ctx->a_pers, app.config.comps);
    NeutronTrajectory *traces_first = TraceParticles(cbui.ctx->a_pers, app.config.comps, &app.config.instr, ncount, 100);
    Array<Monitor> monitors = PlotSaveAndGetMonitors(cbui.ctx->a_pers, app.config.comps);

    // app display
    while (cbui.running) {
        // frame start
        CbuiFrameStart();
        PerspectiveSetAspectAndP(&app.persp, cbui.plf.width, cbui.plf.height);
        OrbitCameraRotateZoom(&app.cam, cbui.plf.cursorpos.dx, cbui.plf.cursorpos.dy, cbui.plf.left.ended_down, cbui.plf.scroll.yoffset_acc);
        OrbitCameraPanInPlane(&app.cam, app.persp.fov, app.persp.aspect, cbui.plf.cursorpos.x_frac, cbui.plf.cursorpos.y_frac, MouseRight().pushed, MouseRight().released);
        app.scene_objs.len = 0;
        UI_SetFontSize(FS_24);
        Button lft = MouseLeft();

        // handle input
        if (GetSpace()) {
            if (app.mode == MTM_TRACE) {
                app.mode = MTM_PLOT;
            }
            else {
                app.mode = MTM_TRACE;
            }
        }
        if (GetChar('l')) { app.draw_plane = !app.draw_plane; }
        if (GetChar('r')) { app.draw_rays = !app.draw_rays; }


        // component hover / selections
        if (app.mode == MTM_TRACE) {
            DoComponentSelectionAndDrawInfoHover(&app, app.config.comps, &app.persp, &app.cam, &app.scene_objs);
        }

        if (app.mode == MTM_PLOT) {
            DoPlotMode(&app, app.config.comps, monitors, &app.cam, &app.scene_objs);
        }

        // render calls
        if (app.draw_rays) {
            RenderTrajectories(traces_first, app.cam.view, app.persp, MCT_COLOR_TRAJECTORY);
        }

        if (app.draw_plane) {
            app.scene_objs.Add(app.plane);
        }

        RenderWireframes(app.scene_objs, app.cam.view, app.persp);
    }

    CbuiExit();
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

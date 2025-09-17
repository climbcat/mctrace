#ifndef __PLOT_HOOKS_H__
#define __PLOT_HOOKS_H__


static Monitor *g_current_monitor;


void mcdetector_out_1D_ext_hook(const char *t, const char *xl, const char *yl, const char *xvar, double x1, double x2, long n, double *p0, double *p1, double *p2, char *f, char *c) {
    assert(g_current_monitor && "set current component");
    assert(g_current_monitor->mon_tpe == MT_NOT || g_current_monitor->mon_tpe == MT_1D);
    // TODO: assert the current component name matches c, as an extra check

    *g_current_monitor = {};
    g_current_monitor->mon_tpe = MT_1D;

    g_current_monitor->title = StrL(t);
    g_current_monitor->xlabel = StrL(xl);
    g_current_monitor->ylabel = StrL(yl);
    g_current_monitor->xvar = StrL(xvar);
    g_current_monitor->xmin = x1;
    g_current_monitor->xmax = x2;
    g_current_monitor->binm_x = n;
    g_current_monitor->N = p0;
    g_current_monitor->p = p1;
    g_current_monitor->p2 = p2;
    g_current_monitor->fname = StrL(f);
}

void mcdetector_out_2D_ext_hook(const char *t, const char *xl, const char *yl, double x1, double x2, double y1, double y2, long m, long n, double *p0, double *p1, double *p2, char *f, char *c) {
    assert(g_current_monitor && "set current component");
    assert(g_current_monitor->mon_tpe == MT_NOT || g_current_monitor->mon_tpe == MT_2D);
    // TODO: assert the current component name matches c, as an extra check

    *g_current_monitor = {};
    g_current_monitor->mon_tpe = MT_2D;

    g_current_monitor->title = StrL(t);
    g_current_monitor->xlabel = StrL(xl);
    g_current_monitor->ylabel = StrL(yl);
    g_current_monitor->xmin = x1;
    g_current_monitor->xmax = x2;
    g_current_monitor->ymin = y1;
    g_current_monitor->ymax = y2;
    g_current_monitor->binm_x = m;
    g_current_monitor->binn_y = n;
    g_current_monitor->N = p0;
    g_current_monitor->p = p1;
    g_current_monitor->p2 = p2;
    g_current_monitor->fname = StrL(f);
}


void mcdetector_out_0D_ext_hook(const char *t, double p0, double p1, double p2, char *c) { printf("NOT IMPLEMENTED: SAVE monitor 0D\n"); }
void mcdetector_out_2D_list_ext_hook(const char *t, const char *xl, const char *yl, double x1, double x2, double y1, double y2, long m, long n, double *p0, double *p1, double *p2, char *f, char *c) { printf("NOT IMPLEMENTED: SAVE monitor 2D list\n"); }
void mcdetector_out_list_ext_hook(const char *t, const char *xl, const char *yl, long m, long n, double *p1, char *f, char *c) { printf("NOT IMPLEMENTED: SAVE monitor list\n"); }


//
//  Monitor functionality


void MonitorPrint(Monitor *mon) {
    if (mon->mon_tpe == MT_1D) {
        printf("1D Monitor: %.*s %d bins of %.*s\n", mon->title.len, mon->title.str, mon->binm_x, mon->xvar.len, mon->xvar.str);

        for (u32 i = 0; i < mon->binm_x; ++i) {
            printf("%g %g %g\n", mon->N[i], mon->p[i], mon->p2[i]);
        }
    }
    else if (mon->mon_tpe == MT_2D) {
        printf("2D Monitor: %.*s [%d,%d] bins\n", mon->title.len, mon->title.str, mon->binm_x, mon->binn_y);
        for (u32 i = 0; i < mon->binn_y; ++i) {
            for (u32 j = 0; j < mon->binm_x; ++j) {
                printf("%g %g %g | ", mon->N[i*mon->binn_y + j], mon->p[i*mon->binn_y + j], mon->p2[i*mon->binn_y + j]);
                //printf("%g ", mon->N[i*mon->binn_y + j]);
            }
            printf("\n");
        }
    }
    else {
        printf("TODO: Print monitor of type %d\n", mon->mon_tpe);
    }
}


void MonitorBlit(MArena *a_tmp, Monitor monitor, s32 mon_left, s32 mon_top, s32 dest_width, s32 dest_height, Color* dest_buffer) {
    // simply blit 2D monitor contents into the image/dest buffer

    // DBG
    Monitor *mon = &monitor;

    // sprite size
    s32 width = mon->binm_x;
    s32 height = mon->binn_y;

    // sprite position in dest buffer 
    s32 left = mon_left;
    s32 top = mon_top;

    // texture coords - blit entire source buffer
    f32 u0 = 0.0f;
    f32 u1 = 1.0f;
    f32 v0 = 0.0f;
    f32 v1 = 1.0f;

    // source size
    s32 src_width = mon->binm_x;
    s32 src_height = mon->binn_y;

    // source Color buffer
    // TODO: transform the double buffers into color (what is the strategy here?)
    double *src_buffer = mon->N;
    Color *src_colbuff = (Color*) ArenaAlloc(a_tmp, sizeof(Color) * src_width * src_height);


    // TODO: cache the prev-frame max value on the Monitor struct
    f32 max_value = 0;
    for (s32 i = 0; i < src_width; ++i) {
        for (s32 j = 0; j < src_width; ++j) {
            f32 src_value = src_buffer[i*src_width + j];
            max_value = MaxF32(max_value, src_value);
        }
    }


    // fill with solid color as a test ... 
    for (s32 i = 0; i < src_width; ++i) {
        for (s32 j = 0; j < src_width; ++j) {
            f32 src_value = src_buffer[i*src_width + j];

            if (src_value > 0) {
                src_colbuff[i*src_width + j] = ColorMapGet(src_value / max_value, colormap_paletted_jet);
            }
        }
    }

    Blit32Bit(width, height, left, top, u0, u1, v0, v1, src_width, src_height, src_colbuff, dest_width, dest_height, dest_buffer);
}


#endif

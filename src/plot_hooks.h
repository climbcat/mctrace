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


#endif

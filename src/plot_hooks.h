#ifndef __PLOT_HOOKS_H__
#define __PLOT_HOOKS_H__


static MArena *g_a_plot;
static Monitor *g_current_monitor;


void mcdetector_out_1D_ext_hook(const char *t, const char *xl, const char *yl, const char *xvar, double x1, double x2, long n, double *p0, double *p1, double *p2, char *f, char *c)
{
    assert(g_a_plot && "set plot arena");
    assert(g_current_monitor && "set current component");
    assert(g_current_monitor->mon_tpe == MT_NOT || g_current_monitor->mon_tpe == MT_1D);
    // TODO: assert the current component name matches c, as an extra check

    *g_current_monitor = {};
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

void mcdetector_out_2D_ext_hook( const char *t, const char *xl, const char *yl, double x1, double x2, double y1, double y2, long m, long n, double *p0, double *p1, double *p2, char *f, char *c)
{
    assert(g_a_plot && "set plot arena");
    assert(g_current_monitor && "set current component");
    assert(g_current_monitor->mon_tpe == MT_NOT || g_current_monitor->mon_tpe == MT_2D);
    // TODO: assert the current component name matches c, as an extra check

    *g_current_monitor = {};
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


#endif

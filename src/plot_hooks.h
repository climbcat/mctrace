#ifndef __PLOT_HOOKS_H__
#define __PLOT_HOOKS_H__



// TODO: capture data pointers and relevant info into a struct, and have a list of those things, for example


void mcdetector_out_0D_ext_hook(const char *t, double p0, double p1, double p2, char *c)
{
    // TODO: impl.
}

void mcdetector_out_1D_ext_hook(const char *t, const char *xl, const char *yl, const char *xvar, double x1, double x2, long n, double *p0, double *p1, double *p2, char *f, char *c)
{
    // TODO: impl.
}

void mcdetector_out_2D_ext_hook( const char *t, const char *xl, const char *yl, double x1, double x2, double y1, double y2, long m, long n, double *p0, double *p1, double *p2, char *f, char *c)
{
    // TODO: impl.
}

void mcdetector_out_2D_list_ext_hook(const char *t, const char *xl, const char *yl, double x1, double x2, double y1, double y2, long m, long n, double *p0, double *p1, double *p2, char *f, char *c)
{
    // TODO: impl.
}

void mcdetector_out_list_ext_hook(const char *t, const char *xl, const char *yl, long m, long n, double *p1, char *f, char *c)
{
    // TODO: impl.
}


#endif

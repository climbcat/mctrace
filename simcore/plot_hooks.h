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


void MonitorClear(Monitor *mon) {
    _memzero(mon->N, sizeof(double) * mon->binm_x * mon->binn_y);
    _memzero(mon->p, sizeof(double) * mon->binm_x * mon->binn_y);
    _memzero(mon->p2, sizeof(double) * mon->binm_x * mon->binn_y);
}


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


//
//  monitor blitting


void Monitor1DGetMinMax(s32 src_len, double *src_buffer, f64 *ymin_out, f64 *ymax_out) {
    assert(ymin_out || ymax_out);

    f64 max_value = src_buffer[0];
    f64 min_value = src_buffer[0];

    for (s32 i = 0; i < src_len; ++i) {
        f64 src_value = src_buffer[i];
        max_value = MaxF64(max_value, src_value);
        min_value = MinF64(min_value, src_value);
    }

    if (ymin_out) {
        *ymin_out = min_value;
    }
    if (ymax_out) {
        *ymax_out = max_value;
    }
}

void Monitor2DGetMinMax(s32 src_width, s32 src_height, double *src_buffer, f64 *zmin_out, f64 *zmax_out) {
    assert(zmin_out || zmax_out);

    f64 max_value = src_buffer[0];
    f64 min_value = src_buffer[0];

    for (s32 i = 0; i < src_width; ++i) {
        for (s32 j = 0; j < src_width; ++j) {
            f64 src_value = src_buffer[i*src_width + j];
            max_value = MaxF64(max_value, src_value);
            min_value = MinF64(min_value, src_value);
        }
    }

    if (zmin_out) {
        *zmin_out = min_value;
    }
    if (zmax_out) {
        *zmax_out = max_value;
    }
}

void Monitor2DBlit(s32 data_width, s32 data_height, f64 *data, f64 data_min, f64 data_max, s32 rect_t, s32 rect_l, s32 rect_w, s32 rect_h, s32 dest_width, s32 dest_height, Color *dest_buff) {
    if (rect_w + rect_l > dest_width || rect_h + rect_t > dest_height) {
        return;
    }

    f32 scale_x = 1.0f / rect_w;
    f32 scale_y = 1.0f / rect_h;

    s32 i, j;
    for (s32 y = 0; y < rect_h; y++) {
        j = rect_t + rect_h - 1 - y;
        for (s32 x = 0; x < rect_w; x++) {

            s32 x_data = (s32) round(data_width * scale_x * x);
            s32 y_data = (s32) round(data_height * scale_y * y);
            u32 idx = data_width * j + i;

            // get the proportianal data point
            f32 data_val = data[ data_width * y_data + x_data ];
            // scale the color map from zmin to zmax
            Color color_ij = ColorMapGet((data_val - data_min) / (data_max - data_min), colormap_paletted_jet);

            i = x + rect_l;
            dest_buff[ dest_width * j + i ] = color_ij;
        }
    }
}


void Monitor1DBlit(s32 data_size, f64 data_min, f64 data_max, f64 *data, s32 rect_t, s32 rect_l, s32 rect_w, s32 rect_h, s32 dest_width, s32 dest_height, Color *dest_buff) {
    if (rect_w > dest_width || rect_h > dest_height || data_size < 2) {
        return;
    }

    // adjust min/max value limits for nice plotting
    f32 data_range = data_max - data_min;
    data_min = data_min - 0.2 * data_range;
    data_min = MaxF32(data_min, 0); // stay positive
    data_max = data_max + 0.2 * data_range;

    f32 scale_y_1d = 1.0f * rect_h / ( (f32) data_max - (f32) data_min);
    f32 scale_x_1d = 1.0f * rect_w / (data_size - 1);

    s32 x1, y1, x2, y2;
    s32 i1, j1, i2, j2;
    for (s32 i = 0; i < data_size - 1; ++i) {
        f64 val1 = data[i];
        f64 val2 = data[i + 1];

        x1 = scale_x_1d * i;
        y1 = scale_y_1d * (val1 - data_min);
        x2 = scale_x_1d * (i + 1);
        y2 = scale_y_1d * (val2 - data_min);

        i1 = x1 + rect_l;
        j1 = rect_t + rect_h - 1 - y1;
        i2 = x2 + rect_l;
        j2 = rect_t + rect_h - 1 - y2;

        RenderLineRGBA((u8*) dest_buff, dest_width, dest_height, i1, j1, i2, j2, COLOR_BLACK);
    }
}


#endif

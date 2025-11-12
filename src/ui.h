#ifndef __MCT_UI_H__
#define __MCT_UI_H__


struct GridLayout {
    f32 w;
    f32 h;
    f32 c;
    f32 cols;
    f32 rows;

    void Print() {
        printf("w: %.2f, h: %.2f, c: %.2f, cols: %.2f, rows: %.2f\n", w, h, c, cols, rows);
    }
};

GridLayout GridCalculate(f32 w = 640, f32 h = 480, f32 N = 20) {
    GridLayout lay = {};
    if (w == 0 || h == 0 || w <= 0 || h <= 0) {
        return lay;
    }
    
    f32 c = 0;

    f32 cols_min = sqrt( w * N / h );
    f32 cols_0 = ceil(cols_min);
    f32 cols_i = 0;
    f32 rows_i = 0;

    s32 i = 0;
    while (true) {
        cols_i = cols_0 + i;
        c = (s32) w / (s32) cols_i;
        rows_i = floor( h / c );

        if (rows_i * cols_i >= N) {
            lay.rows = rows_i;
            lay.cols = cols_i;
            break;
        }
        i++;
    }

    lay.w = w;
    lay.h = h;
    lay.c = c;

    return lay;
}

Component *RenderMonitorGrid(Array<Monitor> monitors) {
    Component *result = NULL;

    // TODO: why is this wrapper widget needed?
    //      Without it, we flicker as 'p' below gets removed bi-framely
    UI_Center();

    Widget *p = WidgetGetCached( (const char*) "grid_pnl" );
    WidgetTreeBranch(p);
    p->SetFlag(WF_LAYOUT_CENTER);
    p->SetFlag(WF_EXPAND_VERTICAL);
    p->SetFlag(WF_EXPAND_HORIZONTAL);
    p->col_border = COLOR_BLACK;
    p->sz_border = 1;

    f32 grid_w = p->rect.x1 - p->rect.x0 - p->padding * 2;
    f32 grid_h = p->rect.y1 - p->rect.y0 - p->padding * 2;
    GridLayout grid = GridCalculate(grid_w, grid_h, monitors.len);

    s32 n = 0;
    s32 idx = 0;
    for (s32 j = 0; j < grid.rows; ++j) {
        for (s32 i = 0; i < grid.cols; ++i) {
            if (n++ < monitors.len) {
                char buff[50];
                _memzero(&buff[0], 50);
                sprintf(buff, "mongrid_%d", idx);

                Widget *w = WidgetGetCached( StrZ(StrL(buff)));
                WidgetTreeSibling(w);
                w->SetFlag(WF_ABSREL_POSITION);
                w->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
                w->SetFlag(WF_CAN_COLLIDE);
                w->col_bckgrnd = COLOR_WHITE;
                w->col_border = COLOR_BLACK;
                w->sz_border = 1;
                w->w = grid.c;
                w->h = grid.c;
                w->x0 = i * grid.c;
                w->y0 = j * grid.c;

                Monitor mon = monitors.arr[idx];
                if (mon.mon_tpe == MT_2D) {
                    //MonitorBlit(cbui.ctx->a_tmp, mon, w->x0, w->y0, grid.c, grid.c, cbui.plf.width, cbui.plf.height, (Color*) cbui.image_buffer);
                    MonitorBlit(cbui.ctx->a_tmp, mon, w->rect.x0, w->rect.y0, grid.c, grid.c, cbui.plf.width, cbui.plf.height, (Color*) cbui.image_buffer);
                }

                if (w->hot) {
                    w->col_border = COLOR_RED;
                }
                if (w->hot && MouseLeft().clicked) {
                    result = (Component *) (monitors.arr + idx)->comp;
                }

                idx++;
            }
        }
    }
    UI_Pop();
    UI_Pop();

    if (GetSpace()) {
        grid.Print();
    }

    return result;
}

void DrawComponentInfoBox(Component *comp) {
    Widget *w = UI_LayoutVertical();
    w->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
    w->SetFlag(WF_ABSREL_POSITION);
    w->col_bckgrnd = COLOR_WHITE;
    w->col_border = COLOR_BLACK;
    w->sz_border = 1;
    w->padding = 5;
    w->y0 = 15;

    Str line1 = StrCat(comp->name, " = ");
    line1 = StrCat(line1, comp->type_name);
    line1 = StrCat(line1, "(");

    UI_Label(StrZ(line1));
    comp->transform;

    ComponentSharedHeader *hdr = comp->GetHeader();
    //if (comp->type == CT_Guide) {
        MArena *a_tmp = cbui.ctx->a_tmp;

        for (s32 i = 0; i < comp->parameters.len; ++i) {
            CompPar par = comp->parameters.arr[i];
            //Str lbl = { "    ", 4 };
            //Str lbl = { (char*) "  ", 2 };
            Str lbl = {};
            lbl = StrCat(lbl, par.name);
            lbl = StrCat(lbl, " = ");
            lbl = StrCat(lbl, par.ValueAsString(a_tmp));
            if (i + 1 == comp->parameters.len) {
                lbl = StrCat(lbl, ")");
            }
            UI_Label(StrZ(lbl));
        }
    //}

    double rot_x, rot_y, rot_z;
    RotationToEulerAnglesDegs(hdr->rotation_absolute, &rot_y, &rot_x, &rot_z);

    char *buff = (char*) ArenaAlloc(cbui.ctx->a_tmp, 200);
    sprintf(buff, "AT (%.2f, %.2f, %.2f) ROTATED (%.2f, %.2f, %.2f)", hdr->position_absolute.x, hdr->position_absolute.y, hdr->position_absolute.z, rot_x, rot_y, rot_z);
    UI_Label(buff);

    UI_Pop();
}

void DrawComponentHover(Component *comp) {
    Widget *w = UI_LayoutVertical();
    w->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
    w->SetFlag(WF_ABSREL_POSITION);
    w->col_bckgrnd = COLOR_WHITE;
    w->col_border = COLOR_BLACK;
    w->sz_border = 1;
    w->padding = 5;

    Vector2f p = CurserPos();
    w->x0 = p.x + 15;
    w->y0 = p.y + 15;

    UI_Label(comp->name.str);

    UI_Pop();
}

void DoComponentSelection(McTraceApp *app, bool fat_monitors) {
    Array<Component*> comps = app->config.comps;

    bool collision_this_frame = false;
    Button lft = MouseLeft();

    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];
        comp->collided_this_frame = false;

        Wireframe comp_wireframe = comp->display;
        comp_wireframe.color = MCT_COLOR_DEFOCUSED;
        if (comp->cat == CCAT_samples || comp->cat == CCAT_sources) {
            comp_wireframe.color = app->colors.sourcesample;
        }
        else if (comp->cat == CCAT_optics) {
            comp_wireframe.color = app->colors.optics;
        }
        else if (comp->cat == CCAT_monitors) {
            comp_wireframe.color = app->colors.monitors;
            if (fat_monitors) { comp_wireframe.style = WFR_FAT; }
        }

        if (comp->interactable_this_frame) {
            Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, comp->display, 0.02f);

            Ray mouse_ray = CameraGetRayWorld(app->cam.view, app->persp.fov, app->persp.aspect, cbui.plf.cursorpos.x_frac, cbui.plf.cursorpos.y_frac);
            comp->collided_this_frame = BoxCollideSLAB(mouse_ray, box);

            if (comp->collided_this_frame && (collision_this_frame == false)) {
                collision_this_frame = true;
                app->comp_hover = comp;

                if (lft.clicked) {
                    app->comp_clicked = comp;
                }
                if (lft.dblclicked) {
                    app->comp_dbl_clicked = comp;
                }
            }
        }

        app->scene_objs.Add(comp_wireframe);
    }

    // hover / click  / double-click
    if (app->comp_hover) {
        DrawComponentHover(app->comp_hover);
        Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, app->comp_hover->display, 0.02f);
        box.color = app->colors.selection;
        app->scene_objs.Add(box);
    }
    if (app->comp_dbl_clicked) {
        Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, app->comp_dbl_clicked->display, 0.02f);
        app->cam.SetRelativeTo(box.transform, box.SizeBallpark() * 1.25f);
        app->comp_selected = app->comp_dbl_clicked;
    }
    else if (app->comp_clicked) {
        app->comp_selected = app->comp_clicked;
    }

    // selection exists
    if (app->comp_selected) {
        Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, app->comp_selected->display, 0.02f);
        box.style = WFR_FAT;
        box.color = MCT_COLOR_DEFOCUSED;
        app->scene_objs.Add(box);
        DrawComponentInfoBox(app->comp_selected);
    }

    // selection off
    if (app->comp_clicked == NULL && MouseLeft().clicked) {
        app->comp_selected = NULL;
    }
}

void OnSwitchToMode(McTraceApp *app) {
    app->colors.SetToMode(app->mode);

    if (app->mode == MTM_TRACE) {
        for (s32 i = 0; i < app->config.comps.len; ++i) {
            Component *comp = app->config.comps.arr[i];

            comp->interactable_this_frame = comp->interactable;
        }
    }
    else if (app->mode == MTM_MONITORS) {
        for (s32 i = 0; i < app->config.comps.len; ++i) {
            Component *comp = app->config.comps.arr[i];

            comp->interactable_this_frame = comp->cat == CCAT_monitors;
        }
    }
}


bool tab_sim = false;
bool tab_trace = true;
bool tab_monitors = false;
bool tab_plot = false;


void DoUI(McTraceApp *app) {
    app->scene_objs.len = 0;
    app->comp_hover = NULL;
    app->comp_clicked = NULL;
    app->comp_dbl_clicked = NULL;

    // handle input
    if (GetSpace()) {
        if (app->mode == MTM_TRACE) {
            app->comp_selected = NULL;
            tab_sim = false; tab_monitors = true; tab_trace = false; tab_plot = false;
            app->mode = MTM_MONITORS;
            OnSwitchToMode(app);
        }
        else if (app->mode == MTM_MONITORS) {
            app->comp_selected = NULL;
            tab_sim = false; tab_monitors = false; tab_trace = true; tab_plot = false;
            app->mode = MTM_TRACE;
            OnSwitchToMode(app);
        }
    }
    if (GetChar('p')) { app->draw_plane = !app->draw_plane; }
    if (GetChar('r')) { app->draw_rays = !app->draw_rays; }

    // UI
    UI_SetFontSize(FS_18);

    // tab menu frame
    s32 padding_1 = 20;
    s32 padding_2 = 10;
    {
        Widget *m2 = UI_Center();
        m2->sz_border = 1;
        m2->padding = padding_1;

        Widget *p = UI_LayoutVertical();
        p->SetFlag(WF_EXPAND_VERTICAL);
        p->SetFlag(WF_EXPAND_HORIZONTAL);
        p->padding = padding_2;
        if (app->mode == MTM_SIM || app->mode == MTM_PLOT) {
            p->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER); // TODO: what if we had a WF_DRAW_BORDER flag, ignoring the fill
            p->col_bckgrnd = COLOR_WHITE;
            p->col_border = COLOR_BLACK;
            p->sz_border = 1;
        }

        UI_SetFontSize(FS_18);
        UI_SpaceV(10);
    }

    // tab contents
    if (app->mode == MTM_TRACE) {
        app->draw_plane = true;
        app->draw_rays = true;

        DoComponentSelection(app, false);
    }

    else if (app->mode == MTM_MONITORS) {
        app->draw_plane = true;
        app->draw_rays = false;

        DoComponentSelection(app, true);

        if (app->comp_selected) {
            // TODO: show the monitor blit

            //RenderMonitors( Array<Monitor> { &app->comp_selected->monitor, 1, 1 } );
        }
    }

    if (app->mode == MTM_PLOT) {
        app->draw_plane = false;
        app->draw_rays = false;

        Component *monitor_clicked = RenderMonitorGrid(app->monitors);
        if (monitor_clicked) {
            app->mode = MTM_MONITORS;
            tab_sim = false; tab_monitors = true; tab_trace = false; tab_plot = false;
            OnSwitchToMode(app);
            app->comp_selected = monitor_clicked;
            Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, app->comp_selected->display, 0.02f);
            app->cam.SetRelativeTo(box.transform, box.SizeBallpark() * 1.25f);
        }
    }

    // tab menu
    {
        Widget *menu_align = UI_LayoutVertical(0);
        menu_align->SetFlag(WF_ABSREL_POSITION);
        menu_align->SetFlag(WF_EXPAND_HORIZONTAL);
        menu_align->y0 = - (padding_1 + padding_2) + 5;
        Widget *menu_2 = UI_LayoutHorizontal();
        menu_2->padding = padding_2;
        if (UI_ToggleTabButton(" Simulate ", &tab_sim)) {
            tab_sim = true; tab_monitors = false; tab_trace = false; tab_plot = false;
            app->mode = MTM_SIM;
            OnSwitchToMode(app);
        }
        if (UI_ToggleTabButton("   Trace  ", &tab_trace)) {
            tab_sim = false; tab_monitors = false; tab_trace = true; tab_plot = false;
            app->mode = MTM_TRACE;
            OnSwitchToMode(app);
        }
        if (UI_ToggleTabButton(" Monitors ", &tab_monitors)) {
            tab_sim = false; tab_monitors = true; tab_trace = false; tab_plot = false;
            app->mode = MTM_MONITORS;
            OnSwitchToMode(app);
        }
        if (UI_ToggleTabButton("   Plot   ", &tab_plot)) {
            tab_sim = false; tab_monitors = false; tab_trace = false; tab_plot = true;
            app->mode = MTM_PLOT;
            OnSwitchToMode(app);
        }
    }
}


#endif

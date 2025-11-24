#ifndef __MCT_UI_H__
#define __MCT_UI_H__


struct GridLayout {
    f32 w;
    f32 h;
    f32 c; // cell size (quadratic)
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


Sprite MonitorUpdateTexture(MArena *a_dest, Monitor *mon, f32 sprite_x0, f32 sprite_y0, s32 dest_width, s32 dest_height) {
    if (mon->mon_tpe != MT_2D) {
        return {};
    }

    mon->texture_2d.tpe = TT_RGBA;
    mon->texture_2d.width = mon->binm_x;
    mon->texture_2d.height = mon->binn_y;
    mon->texture_2d.px_sz = 1;
    mon->texture_2d.data = (u8*) MonitorDataBuffer(a_dest, mon->binm_x, mon->binn_y, mon->N);

    // TODO: how the feck to we remove that texture from the registration?

    u64 key = HashStringValue(mon->comp_name);
    MapPut(&cbui.map_textures, key, &mon->texture_2d);

    // construct the corresponding sprite
    Sprite s = {};
    s.tex_id = key;

    s.w = dest_width;
    s.h = dest_height;
    s.x0 = sprite_x0;
    s.y0 = sprite_y0;
    s.u0 = 0;
    s.u1 = 1;
    s.v0 = 0;
    s.v1 = 1;

    return s;
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

    MArena *a_tmp = cbui.ctx->a_tmp;

    s32 n = 0;
    s32 idx = 0;
    for (s32 j = 0; j < grid.rows; ++j) {
        for (s32 i = 0; i < grid.cols; ++i) {
            if (n++ < monitors.len) {
                char buff[50];
                _memzero(&buff[0], 50);
                sprintf(buff, "mongrid_%d", idx);

                Widget *w = WidgetGetCached( StrZ(StrL(buff)) );
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

                Monitor *mon = monitors.arr + idx;
                if (mon->mon_tpe == MT_2D) {
                    Sprite s = MonitorUpdateTexture(a_tmp, mon, w->rect.x0, w->rect.y0, grid.c, grid.c);
                    SpriteBufferPush(s);
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

Widget *DrawComponentInfoBox(Component *comp) {
    Widget *w = UI_LayoutVertical();
    w->SetFlag(WF_LAYOUT_VERTICAL);
    w->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
    w->SetFlag(WF_ABSREL_POSITION);
    w->col_bckgrnd = COLOR_WHITE;
    w->col_border = COLOR_BLACK;
    w->sz_border = 1;
    w->padding = 5;
    w->y0 = 15;

    UI_Label("COMPONENT");

    Str line1 = StrCat(comp->name, " = ");
    line1 = StrCat(line1, comp->type_name);
    line1 = StrCat(line1, "(");

    UI_Label(StrZ(line1));
    comp->transform;

    s32 space_paragraph = 10;
    UI_SpaceV(space_paragraph);

    ComponentSharedHeader *hdr = comp->GetHeader();
    MArena *a_tmp = cbui.ctx->a_tmp;
    for (s32 i = 0; i < comp->parameters.len; ++i) {
        CompPar par = comp->parameters.arr[i];
        Str lbl = { (char*) "  ", 2};
        lbl = StrCat(lbl, par.name);
        lbl = StrCat(lbl, " = ");
        lbl = StrCat(lbl, par.ValueAsString(a_tmp));
        if (i + 1 == comp->parameters.len) {
            lbl = StrCat(lbl, ")");
        }
        UI_Label(StrZ(lbl));
    }

    double rot_x, rot_y, rot_z;
    RotationToEulerAnglesDegs(hdr->rotation_absolute, &rot_y, &rot_x, &rot_z);

    UI_SpaceV(space_paragraph);

    char *buff = (char*) ArenaAlloc(cbui.ctx->a_tmp, 200);
    sprintf(buff, "AT (%.2f, %.2f, %.2f)", hdr->position_absolute.x, hdr->position_absolute.y, hdr->position_absolute.z);
    UI_Label(buff);

    buff = (char*) ArenaAlloc(cbui.ctx->a_tmp, 200);
    sprintf(buff, "ROTATED (%.2f, %.2f, %.2f)", rot_x, rot_y, rot_z);
    UI_Label(buff);

    UI_Pop();

    // return the layout, in case someone wants to add to that
    return w;
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

void DoComponentSelectionAnnotations(McTraceApp *app, bool fat_monitors) {
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
}

Widget *DoComponentSelectionActions(McTraceApp *app) {
    Widget *w = NULL;

    // hover / click  / double-click
    if (app->comp_hover) {
        if (g_mouse_coolided_last_frame == false) {
            DrawComponentHover(app->comp_hover);
        }
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
        
    }

    if (g_mouse_coolided_last_frame == false) {
        // selection off
        if (app->comp_clicked == NULL && MouseLeft().clicked && true ) {
            app->comp_selected = NULL;
        }
    }

    if (app->comp_selected) {
        w = DrawComponentInfoBox(app->comp_selected);
    }

    return w;
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

Component *_SelectPrevComponent(Array<Component*> comps, Component *selected, bool filter_monitors) {
    s32 idx = 0;

    Component *to_select = NULL;
    if (selected) {
        idx = selected->GetHeader()->index;
        if (idx > 0) {
            idx--;
        }
    }
    to_select = comps.arr[idx];

    if (idx == 0) {
        while (to_select->interactable == false && idx < comps.len - 1) {
            idx++;
            to_select = comps.arr[idx];
        }
    }
    else {
        while (to_select->interactable == false && idx > 0) {
            idx--;
            to_select = comps.arr[idx];
        }
    }

    return to_select;
}

s32 IdxClamp(s32 idx, s32 start, s32 end) {
    s32 min = MinS32(start, end);
    s32 max = MaxS32(start, end);

    idx = MinS32(idx, max);
    idx = MaxS32(idx, min);
    return idx;
}
bool IdxBefore(s32 idx, s32 start, s32 end) {
    s32 min = MinS32(start, end);
    s32 max = MaxS32(start, end);
    return idx < max;
}
bool IdxAfter(s32 idx, s32 start, s32 end) {
    s32 min = MinS32(start, end);
    s32 max = MaxS32(start, end);
    return idx > min;
}
bool FilterIsInteractiblaOrIsMonitor(Component *comp, bool filter_monitor) {
    if (filter_monitor) {
        bool is_monitor = (comp->monitor.mon_tpe == MT_2D) || (comp->monitor.mon_tpe == MT_1D) || (comp->monitor.mon_tpe == MT_0D);
        return is_monitor;
    }
    else {
        return comp->interactable;
    }
}
Component *_SelectPrevOrNextComponent(Array<Component*> comps, Component *selected, s32 idx_start, s32 idx_end, bool filter_monitors) {
    s32 idx = idx_end;
    s32 sign = 1;
    if (idx_start > idx_end) {
        sign = -1;
    }

    Component *to_select = NULL;
    if (selected) {
        idx = selected->GetHeader()->index;
        idx = IdxClamp(idx + sign, idx_start, idx_end);
    }
    to_select = comps.arr[idx];

    if (FilterIsInteractiblaOrIsMonitor(to_select, filter_monitors) == false) {
        if (idx == idx_end) {
            while (FilterIsInteractiblaOrIsMonitor(to_select, filter_monitors) == false && IdxBefore(idx, idx_start, idx_end)) {
                idx = IdxClamp(idx - sign, idx_start, idx_end);
                to_select = comps.arr[idx];
            }
        }
        else {
            while (FilterIsInteractiblaOrIsMonitor(to_select, filter_monitors) == false && IdxAfter(idx, idx_start, idx_end)) {
                idx = IdxClamp(idx + sign, idx_start, idx_end);
                to_select = comps.arr[idx];
            }
        }
    }

    return to_select;
}
void DoScrollBtn(McTraceApp *app, const char* btn_label, s32 direction) {
    Widget *btn = NULL;
    bool clicked = false;

    clicked = UI_Button(btn_label, &btn);
    btn->y0 -= 1;
    btn->w = 40;
    btn->h = 20;
    UI_SpaceH(10);

    if (clicked) {
        s32 idx_start = 0;
        s32 idx_end = app->config.comps.len - 1;
        if (direction == -1) {
            idx_start = idx_end;
            idx_end = 0;
        }

        Component *to_select = _SelectPrevOrNextComponent(app->config.comps, app->comp_selected, idx_start, idx_end, app->mode == MTM_MONITORS);

        if (to_select) { 
            app->comp_selected = to_select;
            app->comp_clicked = app->comp_selected;
            app->comp_dbl_clicked = app->comp_selected;

            Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, app->comp_dbl_clicked->display, 0.02f);
            app->cam.Update( TransformGetTranslation( box.transform ) );
        }
    }
}

void DoTabMenu(McTraceApp *app) {
    UI_SetFontSize(FS_18);

    // frame
    {
        Widget *m2 = UI_Center();
        m2->sz_border = 1;
        m2->padding = 20;

        Widget *p = UI_LayoutVertical();
        p->SetFlag(WF_EXPAND_VERTICAL);
        p->SetFlag(WF_EXPAND_HORIZONTAL);
        p->padding = 10;
        if (app->mode == MTM_SIM || app->mode == MTM_PLOT) {
            p->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER); // TODO: what if we had a WF_DRAW_BORDER flag, ignoring the fill
            p->col_bckgrnd = COLOR_WHITE;
            p->col_border = COLOR_BLACK;
            p->sz_border = 1;
        }

        UI_SetFontSize(FS_18);
        UI_SpaceV(10);
    }

    // tabs
    {
        Widget *menu_align = UI_LayoutVertical(0);

        s32 padding_1 = 20;
        s32 padding_2 = 10;

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
        UI_Pop();
    }

}

void DoLeftRightButtons(McTraceApp *app) {
    UI_LayoutHorizontal();

    DoScrollBtn(app, "<", -1);
    DoScrollBtn(app, ">", 1);

    UI_Pop();
}

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

    DoTabMenu(app);


    // tab contents
    if (app->mode == MTM_TRACE) {
        app->draw_plane = true;
        app->draw_rays = true;

        DoLeftRightButtons(app);
        DoComponentSelectionAnnotations(app, false);
        DoComponentSelectionActions(app);
    }

    else if (app->mode == MTM_MONITORS) {
        app->draw_plane = true;
        app->draw_rays = false;

        DoLeftRightButtons(app);
        DoComponentSelectionAnnotations(app, true);
        Widget *info_box = DoComponentSelectionActions(app);

        if (app->comp_selected && app->comp_selected->monitor.mon_tpe != MT_NOT) {
            // display the monitor blit

            Monitor *mon = &app->comp_selected->monitor;

            assert(info_box != NULL);
            g_w_layout = info_box;

            UI_SpaceV(10);

            Widget *w = WidgetGetCached("monitor_selection_blit_area");
            WidgetTreeSibling(w);
            w->w = 128;
            w->h = 128;
            w->col_bckgrnd = COLOR_BLUE;

            Sprite s = MonitorUpdateTexture(cbui.ctx->a_tmp, mon, w->rect.x0, w->rect.y0, 128, 128);
            SpriteBufferPush(s);

            UI_Pop();
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

}


#endif

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

Widget *DoComponentInfoBox(Component *comp) {
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
        Param par = comp->parameters.arr[i];
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
    Array<Component*> comps = app->config->comps;
    Button lft = MouseLeft();

    Ray mouse_ray = CameraGetRayWorld(app->cam.view, app->persp.fov, app->persp.aspect, cbui.plf.cursorpos.x_frac, cbui.plf.cursorpos.y_frac);
    Vector3f cam_at = app->cam.Position();
    f32 mouse_ray_hit_dist = INFINITY;
    Component * comp_hover = NULL;

    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];
        comp->collided_this_frame = false;

        Wireframe display = comp->display;
        display.color = MCT_COLOR_DEFOCUSED;
        if (comp->cat == CCAT_samples || comp->cat == CCAT_sources) {
            display.color = app->colors.sourcesample;
        }
        else if (comp->cat == CCAT_optics) {
            display.color = app->colors.optics;
        }
        else if (comp->cat == CCAT_monitors) {
            display.color = app->colors.monitors;
            if (fat_monitors) { display.style = WFR_FAT; }
        }

        if (comp->interactable_this_frame) {
            Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, comp->display, 0.02f);

            Vector3f hit;
            comp->collided_this_frame = BoxCollideSLAB(mouse_ray, box, &hit);

            if (comp->collided_this_frame) {
                f32 dist = (hit - cam_at).Norm();
                if (dist < mouse_ray_hit_dist) {
                    mouse_ray_hit_dist = dist;
                    comp_hover = comp;
                }
            }
        }

        app->scene_objs.Add(display);
    }

    if (comp_hover) {
        app->comp_hover = comp_hover;

        if (lft.clicked) {
            app->comp_clicked = comp_hover;
        }
        if (lft.dblclicked) {
            app->comp_dbl_clicked = comp_hover;
        }
    }
}

Widget *DoComponentSelectionActions(McTraceApp *app) {
    Widget *w = NULL;

    if (GetEnter() && app->comp_selected) {
        Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, app->comp_selected->display, 0.02f);
        app->cam.SetRelativeTo(box.transform, box.SizeBallpark() * 1.25f);
    }

    // hover / click  / double-click
    if (UI_DidCollide() == false) {
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

        if (app->comp_clicked == NULL && MouseLeft().clicked && true ) {
            app->comp_selected = NULL;
        }
    }

    // selection bounding-box
    if (app->comp_selected) {
        Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, app->comp_selected->display, 0.02f);
        box.style = WFR_FAT;
        box.color = MCT_COLOR_DEFOCUSED;
        app->scene_objs.Add(box);

        //w = DrawComponentInfoBox(app->comp_selected);
    }

    return w;
}

void OnSwitchToMode(McTraceApp *app) {
    app->colors.SetToMode(app->mode);
    app->comp_selected = NULL;

    Array<Component*> comps = app->config->comps;
    if (app->mode == MTM_TRACE) {
        for (s32 i = 0; i < comps.len; ++i) {
            Component *comp = comps.arr[i];

            comp->interactable_this_frame = comp->interactable;
        }
    }
    else if (app->mode == MTM_MONITORS) {
        for (s32 i = 0; i < comps.len; ++i) {
            Component *comp = comps.arr[i];

            comp->interactable_this_frame = comp->cat == CCAT_monitors;
        }
    }
}

inline
s32 FirstTrue(Array<bool> selector) {
    for (s32 i = 0; i < selector.len; ++i) {
        if (selector.arr[i]) {
            return i;
        }
    }
    return -1;
}
inline
s32 LastTrue(Array<bool> selector) {
    for (s32 i = selector.len - 1; i >= 0; --i) {
        if (selector.arr[i]) {
            return i;
        }
    }
    return -1;
}
inline
s32 NextTrue(Array<bool> selector, s32 initial) {
    assert(initial < selector.len);

    if (initial < selector.len) {
        for (s32 i = initial + 1; i < selector.len; ++i) {
            if (selector.arr[i]) {
                return i;
            }
        }
    }

    if (selector.arr[initial]) {
        return initial;
    }
    else {
        return -1;
    }
}
inline
s32 PrevTrue(Array<bool> selector, s32 initial) {
    assert(initial < selector.len);

    if (initial > 0) {
        for (s32 i = initial - 1; i >= 0; --i) {
            if (selector.arr[i]) {
                return i;
            }
        }
    }

    if (selector.arr[initial]) {
        return initial;
    }
    else {
        return -1;
    }
}


static bool tab_state[4] = { false, true, false, false };

void DoEnableTabButton(McTraceMode mode) {
    if (mode == MTM_SIM) {
        tab_state[0] = true;
        tab_state[1] = false;
        tab_state[2] = false;
        tab_state[3] = false;
    }
    else if (mode == MTM_TRACE) {
        tab_state[0] = false;
        tab_state[1] = true;
        tab_state[2] = false;
        tab_state[3] = false;
    }
    else if (mode == MTM_MONITORS) {
        tab_state[0] = false;
        tab_state[1] = false;
        tab_state[2] = true;
        tab_state[3] = false;
    }
    else if (mode == MTM_PLOT) {
        tab_state[0] = false;
        tab_state[1] = false;
        tab_state[2] = false;
        tab_state[3] = true;
    }
}

void DoScrollBtn(McTraceApp *app, const char* btn_label, s32 direction, Array<bool> selector) {
    assert(direction == -1 || direction == 1);
    Array<Component*> comps = app->config->comps;

    Widget *btn = NULL;
    bool clicked = false;

    clicked = UI_Button(btn_label, &btn);
    btn->y0 -= 1;
    btn->w = 40;
    btn->h = 20;
    UI_SpaceH(10);

    if (clicked) {
        s32 idx = -1;
        if (app->comp_selected) {
            idx = app->comp_selected->GetHeader()->index;
            if (direction == 1) {
                idx = NextTrue(selector, idx);
            }
            else if (direction == -1) {
                idx = PrevTrue(selector, idx);
            }
        }
        else {
            if (direction == 1) {
                idx = LastTrue(selector);
            }
            else if (direction == -1) {
                idx = FirstTrue(selector);
            }
        }

        if (idx >= 0) { 
            Component *to_select = comps.arr[idx];

            app->comp_selected = to_select;
            app->comp_clicked = app->comp_selected;

            Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, app->comp_selected->display, 0.02f);
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
        if (UI_ToggleTabButton(" Simulate ", &tab_state[0])) {
            DoEnableTabButton(MTM_SIM);
            app->mode = MTM_SIM;
            OnSwitchToMode(app);
        }
        if (UI_ToggleTabButton("   Trace  ", &tab_state[1])) {
            DoEnableTabButton(MTM_TRACE);
            app->mode = MTM_TRACE;
            OnSwitchToMode(app);
        }
        if (UI_ToggleTabButton(" Monitors ", &tab_state[2])) {
            DoEnableTabButton(MTM_MONITORS);
            app->mode = MTM_MONITORS;
            OnSwitchToMode(app);
        }
        if (UI_ToggleTabButton("   Plot   ", &tab_state[3])) {
            DoEnableTabButton(MTM_PLOT);
            app->mode = MTM_PLOT;
            OnSwitchToMode(app);
        }
        UI_Pop();
    }
}

void DoLeftRightButtons(McTraceApp *app, Array<bool> selector) {
    UI_LayoutHorizontal();

    DoScrollBtn(app, "<", -1, selector);
    DoScrollBtn(app, ">", 1, selector);

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
            DoEnableTabButton(MTM_TRACE);
            app->mode = MTM_MONITORS;
            OnSwitchToMode(app);
        }
        else if (app->mode == MTM_MONITORS) {
            app->comp_selected = NULL;
            DoEnableTabButton(MTM_MONITORS);
            app->mode = MTM_TRACE;
            OnSwitchToMode(app);
        }
    }

    // UI

    if (app->mode == MTM_SIM) {
        Array<Component*> comps = app->config->comps;
        DoTabMenu(app);

        Widget *layout = UI_LayoutVertical();
        layout->SetFlag(WF_EXPAND_HORIZONTAL);

        {
            Instrument *instr = &app->config->instr;
            Str line1 = StrCat(StrL("INSTRUMENT "), StrL(instr->name));
            UI_Label(StrZ(line1));

            MArena *a_tmp = cbui.ctx->a_tmp;
            for (s32 i = 0; i < instr->parameters.len; ++i) {
                Param par = instr->parameters.arr[i];

                Str lbl = { (char*) "  ", 2};
                lbl = StrCat(lbl, par.name);
                lbl = StrCat(lbl, " = ");
                lbl = StrCat(lbl, par.ValueAsString(a_tmp));
                UI_Label(StrZ(lbl));
            }
        }

        UI_SpaceV(10);

        bool sim = app->simulation_active;
        bool trc = app->trace_active;
        assert(!trc || !sim);

        {
            UI_Label("TRACE");

            char buff[200];
            sprintf(buff, "ncount (final):   %d", *app->ncount_target);
            Widget *w1 = UI_Label(buff);
            w1->text = StrL(buff);

            sprintf(buff, "ncount (current): %d", *app->ncount_current);
            Widget *w2 = UI_Label(buff);
            w2->text = StrL(buff);

            UI_SpaceV(10);
            UI_LayoutHorizontal();

            if (!trc) {
                if (UI_Button("Trace", NULL, sim)) {
                    g_do_trace_trajectories = true;
                    app->trace_active = true;
                }
            }
            else {
                if (UI_Button("Pause", NULL, sim)) {
                    app->trace_active = false;
                }
            }

            UI_SpaceH(10);
            if (UI_Button("Reset", NULL, sim)) {

                // TODO: extact into "trace control functionality" unit
                {
                    app->trace_active = false;
                    for (s32 i = 0; i < comps.len; ++i) {
                        Monitor *mon = &comps.arr[i]->monitor;
                        if (mon->mon_tpe != MT_NOT) {
                            MonitorClear(mon);
                        }
                    }
                    TrajectoryContainerClear(&app->config->container);
                    *app->ncount_current = 0;
                }
            }
            UI_Pop();


            // TODO: Implement the case where trace has completed
            //      In fact, it might never end

        }

        //UI_Pop();
        UI_SpaceV(10);

        {
            UI_Label("SIMULATE");

            /*
            char buff[200];
            sprintf(buff, "ncount (final):   %d", *app->ncount_target);
            Widget *w1 = UI_Label(buff);
            w1->text = StrL(buff);

            sprintf(buff, "ncount (current): %d", *app->ncount_current);
            Widget *w2 = UI_Label(buff);
            w2->text = StrL(buff);
            */

            UI_SpaceV(10);
            UI_LayoutHorizontal();

            if (!sim) {
                if (UI_Button("Run 1e8", NULL, trc)) {

                    // TODO: extact reset into "trace control functionality" unit
                    {
                        app->trace_active = false;
                        for (s32 i = 0; i < comps.len; ++i) {
                            Monitor *mon = &comps.arr[i]->monitor;
                            if (mon->mon_tpe != MT_NOT) {
                                MonitorClear(mon);
                            }
                        }
                        g_do_trace_trajectories = false;
                        TrajectoryContainerClear(&app->config->container);
                        *app->ncount_current = 0;
                    }

                    app->simulation_active = true;
                }
            }
            else {
                if (UI_Button("Abort", NULL, trc)) {
                    app->simulation_active = false;
                }
            }

            UI_Pop();


            // TODO: Implement the case where the simulation is completed


        }
    }

    else if (app->mode == MTM_TRACE) {
        DoTabMenu(app);

        app->draw_plane = true;
        app->draw_rays = true;

        DoLeftRightButtons(app, app->config->comps_interactible);

        UI_LayoutHorizontal();
        if (UI_Button("PrvConf") && app->config->prev) {
            McTraceSetConfig(app, app->config->prev);
        }
        if (UI_Button("NxtConf") && app->config->next) {
            McTraceSetConfig(app, app->config->next);
        }
        UI_Pop();

        DoComponentSelectionAnnotations(app, false);
        DoComponentSelectionActions(app);

        if (app->comp_selected) {
            DoComponentInfoBox(app->comp_selected);
        }
    }

    else if (app->mode == MTM_MONITORS) {
        DoTabMenu(app);

        app->draw_plane = true;
        app->draw_rays = false;

        DoLeftRightButtons(app, app->config->comps_monitors);
        DoComponentSelectionAnnotations(app, true);
        DoComponentSelectionActions(app);

        if (app->comp_selected && app->comp_selected->monitor.mon_tpe != MT_NOT) {
            // display the monitor blit
            Widget *info_box_panel = DoComponentInfoBox(app->comp_selected);

            assert(info_box_panel != NULL);
            UI_SetCurrentLayout(info_box_panel);

            UI_SpaceV(10);

            Widget *w = WidgetGetCached("monitor_selection_blit_area");
            WidgetTreeSibling(w);
            w->w = 128;
            w->h = 128;
            w->col_bckgrnd = COLOR_BLUE;

            Sprite s = MonitorUpdateTexture(cbui.ctx->a_tmp, &app->comp_selected->monitor, w->rect.x0, w->rect.y0, 128, 128);
            SpriteBufferPush(s);

            UI_Pop();
        }
    }

    else if (app->mode == MTM_PLOT) {
        DoTabMenu(app);

        UI_Pop();

        app->draw_plane = false;
        app->draw_rays = false;

        Component *monitor_clicked = RenderMonitorGrid(app->config->monitors);
        if (monitor_clicked) {
            app->mode = MTM_MONITORS;
            DoEnableTabButton(MTM_MONITORS);
            OnSwitchToMode(app);
            app->comp_selected = monitor_clicked;
            Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, app->comp_selected->display, 0.02f);
            app->cam.SetRelativeTo(box.transform, box.SizeBallpark() * 1.25f);
        }

    }
}


#endif

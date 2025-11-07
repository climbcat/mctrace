#ifndef __MCT_UI_H__
#define __MCT_UI_H__


Component *RenderMonitors(Array<Monitor> monitors) {
    UI_LayoutVertical();
    UI_SpaceV(10);
    UI_LayoutHorizontal();
    UI_SpaceH(10);

    s32 plot_area_width = 128;
    s32 plot_area_height = 128;

    Component *result_clicked = NULL;

    // labels
    for (u32 i = 0; i < monitors.len; ++i) {
        Monitor mon = monitors.arr[i];

        if (mon.mon_tpe == MT_2D) {
            Widget *l = WidgetGetCached( (const char*) StrZ(StrCat(mon.comp_name, "_pnl")) );
            WidgetTreeBranch(l);
            l->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
            l->SetFlag(WF_CAN_COLLIDE);
            l->SetFlag(WF_LAYOUT_VERTICAL);
            l->SetFlag(WF_ALIGN_CENTER);
            l->sz_border = 1;
            l->col_border = COLOR_BLACK;
            l->col_bckgrnd = COLOR_WHITE;
            l->w = plot_area_height * 1.4;
            l->h = plot_area_width * 1.4;
            if (l->hot) {
                l->col_border = COLOR_RED;
            }
            if (l->clicked) {
                result_clicked = (Component*) mon.comp;
            }

            Widget *lbl = UI_Label( (const char*) StrZ(StrCat(mon.comp_name, " ")) );
            lbl->sz_font = FS_18;
            UI_SpaceV(10);

            Widget *w = WidgetGetCached( (const char*) StrZ(StrCat(mon.comp_name, "_plot")) );
            w->features_flg |= WF_DRAW_BACKGROUND_AND_BORDER;
            w->w = plot_area_width;
            w->h = plot_area_height;
            w->sz_border = 0;
            w->col_bckgrnd = COLOR_WHITE;
            w->col_bckgrnd.a = 0;
            w->col_border = ColorGray(0.7f);
            WidgetTreeSibling(w);

            // TODO: we want to express this as a sprite, which will then get properly blitted during FrameEnd
            //
            // jg-250922: This can be done by pushing a sprite with SpriteBufferPush().
            //      However, we still need a way to push it to the "top" of the stack.
            //      This also works with a "texture" that has a "texture id". This means registering textures,
            //      per-frame or persistently. It is fine to put it on the appropriate lifescale areana, but 
            //      we also need to register the texture with an ID in a persistent hashmap; How should it be
            //      de-registered, then?
            //
            //      Temporary hack: We store the widget in our Monitor and draw it at the right time.
            MonitorBlit(cbui.ctx->a_tmp, mon, w->x0, w->y0, plot_area_width, plot_area_height, cbui.plf.width, cbui.plf.height, (Color*) cbui.image_buffer);
            UI_SpaceV(10);

            UI_Pop();
            UI_SpaceH(10);
        }
    }

    return result_clicked;
}

void DrawComponentInfoBox(Component *comp) {
    Widget *w = UI_LayoutVertical();
    w->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
    w->SetFlag(WF_ABSREL_POSITION);
    w->col_bckgrnd = COLOR_WHITE;
    w->col_border = COLOR_BLACK;
    w->sz_border = 1;
    w->padding = 5;
    w->x0 = 15;
    w->y0 = 15;

    Str line1 = StrCat(comp->name, " (");
    line1 = StrCat(line1, comp->type_name);
    line1 = StrCat(line1, ")");
    
    UI_Label(StrZ(line1));
    comp->transform;

    ComponentSharedHeader *hdr = comp->GetHeader();

    char *buff = (char*) ArenaAlloc(cbui.ctx->a_tmp, 16);
    sprintf(buff, "x = %.2f", hdr->position_absolute.x);
    UI_Label(buff);

    buff = (char*) ArenaAlloc(cbui.ctx->a_tmp, 16);
    sprintf(buff, "y = %.2f", hdr->position_absolute.y);
    UI_Label(buff);

    buff = (char*) ArenaAlloc(cbui.ctx->a_tmp, 16);
    sprintf(buff, "z = %.2f", hdr->position_absolute.z);
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

void DoUI(McTraceApp *app) {
    app->scene_objs.len = 0;
    app->comp_hover = NULL;
    app->comp_clicked = NULL;
    app->comp_dbl_clicked = NULL;

    // handle input
    if (GetSpace()) {
        app->comp_selected = NULL;
        if (app->mode == MTM_TRACE) {
            app->mode = MTM_MONITORS;
        }
        else if (app->mode == MTM_MONITORS) {
            app->mode = MTM_PLOT;
        }
        else {
            app->mode = MTM_TRACE;
        }

        OnSwitchToMode(app);
    }
    if (GetChar('p')) { app->draw_plane = !app->draw_plane; }
    if (GetChar('r')) { app->draw_rays = !app->draw_rays; }

    UI_SetFontSize(FS_24);

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
            RenderMonitors( Array<Monitor> { &app->comp_selected->monitor, 1, 1 } );
        }
    }

    if (app->mode == MTM_PLOT) {
        app->draw_plane = false;
        app->draw_rays = false;

        Component *monitor_clicked = RenderMonitors(app->monitors);
    }
}


#endif

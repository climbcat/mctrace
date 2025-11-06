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


void DoComponentSelectionAndDrawInfoHover(McTraceApp *app, Array<Component*> comps, Perspective *persp, OrbitCamera *cam, Array<Wireframe> *scene_objs) {
    bool collision_this_frame = false;
    Button lft = MouseLeft();

    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];
        comp->collided_this_frame = false;
        scene_objs->Add(comp->display);

        if (comp->interactable) {
            Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, comp->display, 0.02f);
            box.color = MCT_COLOR_SELECTION_BOX;

            Ray mouse_ray = CameraGetRayWorld(cam->view, persp->fov, persp->aspect, cbui.plf.cursorpos.x_frac, cbui.plf.cursorpos.y_frac);
            comp->collided_this_frame = BoxCollideSLAB(mouse_ray, box);

            if (comp->collided_this_frame && (collision_this_frame == false)) {
                collision_this_frame = true;

                if (lft.dblclicked) {
                    box.style = WFR_FAT;
                    cam->SetRelativeTo(box.transform, box.SizeBallpark() * 1.25f);
                    app->comp_selected = comp;
                }
                else if (lft.clicked) {
                    box.style = WFR_FAT;
                    app->comp_selected = comp;
                }

                // draw hover component name
                DrawComponentHover(comp);
                scene_objs->Add(box);
            }

            if (comp == app->comp_selected) {
                box.style = WFR_FAT;
                scene_objs->Add(box);

                // draw selected component info box
                DrawComponentInfoBox(comp);
            }
        }
    }

    // selection off
    if (collision_this_frame == false && lft.clicked) {
        app->comp_selected = NULL;
    }
}


void DoPlotMode(McTraceApp *app, Array<Component*> comps, Array<Monitor> monitors, OrbitCamera *cam, Array<Wireframe> *scene_objs) {
    Button lft = MouseLeft();

    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];
        Wireframe comp_wireframe = comp->display;
        if (comp->cat == CCAT_monitors) {
            comp_wireframe.color = MCT_COLOR_MONITOR;
            comp_wireframe.style = WFR_FAT;
        }
        else {
            comp_wireframe.color = MCT_COLOR_DEFOCUSED;
        }

        scene_objs->Add(comp_wireframe);
    }

    Component *monitor_clicked = RenderMonitors(monitors);

    if (monitor_clicked) {
        app->comp_selected = monitor_clicked;
    }

    if (app->comp_selected && app->comp_selected->monitor.mon_tpe == MT_2D) {

        Wireframe box = CreateAABoundingBox(cbui.ctx->a_tmp, app->comp_selected->display, 0.02f);
        box.color = MCT_COLOR_SELECTION_BOX;
        scene_objs->Add(box);

        if (monitor_clicked && lft.dblclicked) {
            cam->SetRelativeTo(box.transform, box.SizeBallpark() * 2);
        }
        else if (monitor_clicked) {
            cam->SetRelativeTo(box.transform);
        }
    }
}


#endif

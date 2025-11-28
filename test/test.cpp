
void TestComponentFuncitonsRun() {
    TimeFunction;
    printf("TestComponentFuncitonsRun\n\n");

    // init
    MContext *ctx = InitBaselayer();


    s32 ncount = 1e6;
    InstrumentConfig config = InitInstrument(cbui.ctx->a_pers, IC_PSI_DMC, ncount);


    // run display & calculate helper matrices:
    Matrix4f t_world_prev = Matrix4f_Identity();
    for (s32 i = 0; i < config.comps.len; ++i) {
        Component *comp = config.comps.arr[i];
        McDisplayNext(ctx->a_pers, comp->transform->t_world);

        DisplayComponent(comp);

        Matrix4f t_world = comp->transform->t_world;
        comp->t_prev2loc = TransformGetInverse(t_world) * t_world_prev;
        t_world_prev = t_world;

        ComponentSharedHeader *hdr = comp->GetHeader();
        hdr-> position_absolute.x =  comp->transform->t_world.m[0][3];
        hdr-> position_absolute.y =  comp->transform->t_world.m[1][3];
        hdr-> position_absolute.z =  comp->transform->t_world.m[2][3];
        hdr-> position_relative.x =  comp->transform->t_loc.m[0][3];
        hdr-> position_relative.y =  comp->transform->t_loc.m[1][3];
        hdr-> position_relative.z =  comp->transform->t_loc.m[2][3];
        for (u32 i = 0; i < 3; i++) {
            for (u32 j = 0; j < 3; j++) {
                // NOTE: testing will tell if we may need to mirror the rot matrix, or what

                hdr->rotation_absolute[i][j] = comp->transform->t_world.m[i][j];
                hdr->rotation_relative[i][j] = comp->transform->t_loc.m[i][j];
            }
        }
    }


    // trace
    u32 DBG_break_after_ncount = 5;
    for (u32 j = 0; j < ncount; ++j) {
        Neutron particle = {};

        for (s32 i = 0; i < config.comps.len; ++i) {
            Component *comp = config.comps.arr[i];

            ParticlePrintWorld(comp->transform->t_world, particle);

            // previous local system -> current local system
            ParticleTransform(comp->t_prev2loc, &particle);

            // run trace code
            TraceComponent(comp, &particle, &config.instr);
        }

        if (j + 1 == DBG_break_after_ncount) {
            break;
        }
    }


    // finally
    for (s32 i = 0; i < config.comps.len; ++i) {
        Component *comp = config.comps.arr[i];

        FinallyComponent(comp);
    }
}


void TestMainLayout() {
    printf("TestMainLayout\n\n");


    CbuiInit("TestMainLayout", false);

    // TODO: init

    s32 padding_1 = 20;
    s32 padding_2 = 10;

    bool tab_sim = true;
    bool tab_monitors = false;
    bool tab_trace = false;
    bool tab_plot = false;

    bool toggle_test = false;
    bool toggle_test2 = false;

    while (cbui.running) {
        CbuiFrameStart();


        // basic layout
        {
            Widget *m2 = UI_Center();
            m2->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
            m2->col_bckgrnd = COLOR_GREEN;
            m2->col_border = COLOR_BLACK;
            m2->sz_border = 1;
            m2->padding = padding_1;
            m2->DBG_tag = StrL("B");


            Widget *p = UI_LayoutVertical();
            p->SetFlag(WF_EXPAND_VERTICAL);
            p->SetFlag(WF_EXPAND_HORIZONTAL);
            p->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER); // TODO: what if we had a WF_DRAW_BORDER flag, ignoring the fill
            p->col_bckgrnd = COLOR_WHITE;
            p->col_border = COLOR_BLACK;
            p->sz_border = 1;
            p->padding = padding_2;
            p->DBG_tag = StrL("V");
            UI_SetFontSize(FS_18);
            UI_SpaceV(10);
        }

        // tab menu
        {
            Widget *menu_align = UI_LayoutVertical(0);
            menu_align->SetFlag(WF_ABSREL_POSITION);
            menu_align->SetFlag(WF_EXPAND_HORIZONTAL);
            menu_align->y0 = - (padding_1 + padding_2) + 5;
            Widget *menu_2 = UI_LayoutHorizontal();
            menu_2->padding = padding_2;
            if (UI_ToggleTabButton(" Simulate ", &tab_sim)) { tab_sim = true; tab_monitors = false; tab_trace = false; tab_plot = false; }
            if (UI_ToggleTabButton("   Trace  ", &tab_trace)) { tab_sim = false; tab_monitors = false; tab_trace = true; tab_plot = false; }
            if (UI_ToggleTabButton(" Monitors ", &tab_monitors)) { tab_sim = false; tab_monitors = true; tab_trace = false; tab_plot = false; }
            if (UI_ToggleTabButton("   Plot   ", &tab_plot)) { tab_sim = false; tab_monitors = false; tab_trace = false; tab_plot = true; }
            UI_Pop();
            UI_Pop();
        }

        // tab contents
        if (tab_sim)
        {
            UI_Label("Simulate");
            UI_Button("Run");
        }
        else if (tab_monitors) {
            UI_Label("Monitors");
        }
        else if (tab_trace) {
            UI_Label("Trace");
        }
        else if (tab_plot) {
            UI_Label("Plot");
        }


        if (GetUp()) {
            padding_1++;
        }
        else if (GetDown()) {
            padding_1--;
            if (padding_1 < 0) {
                padding_1 = 0;
            }
        }
        else if (GetRight()) {
            padding_2++;
        }
        else if (GetLeft()) {
            padding_2--;
            if (padding_2 < 0) {
                padding_2 = 0;
            }
        }

        CbuiFrameEnd();
    }
    CbuiExit();

}

void TestGridLayoutCalculations() {
    printf("TestGridLayoutCalculations\n");

    CbuiInit("TestGridLayoutCalculations", false);
    while (cbui.running) {
        CbuiFrameStart();

        // basic layout
        Widget *m2 = UI_Center();
        m2->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
        m2->col_bckgrnd = COLOR_GREEN;
        m2->col_border = COLOR_BLACK;
        m2->sz_border = 1;
        m2->padding = 5;

        Widget *p = WidgetGetCached( (const char*) "grid_container_pnl" );
        WidgetTreeBranch(p);
        p->SetFlag(WF_LAYOUT_VERTICAL);
        p->SetFlag(WF_EXPAND_VERTICAL);
        p->SetFlag(WF_EXPAND_HORIZONTAL);
        p->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER); // TODO: what if we had a WF_DRAW_BORDER flag, ignoring the fill
        p->features_flg |= WF_CAN_COLLIDE;
        p->col_bckgrnd = COLOR_BLUE;
        p->col_border = COLOR_BLACK;
        p->sz_border = 1;
        p->padding = 5;
        p->DBG_tag = StrL("V");
        UI_SetFontSize(FS_18);
        UI_SpaceV(10);

        f32 grid_w = p->rect.x1 - p->rect.x0 - p->padding * 2;
        f32 grid_h = p->rect.y1 - p->rect.y0 - p->padding * 2;
        GridLayout grid = GridCalculate(grid_w, grid_h, 20);

        s32 n = 0;
        for (s32 j = 0; j < grid.rows; ++j) {
            for (s32 i = 0; i < grid.cols; ++i) {
                if (n++ < 20) {
                    Widget *w = UI_LayoutHorizontal();
                    w->SetFlag(WF_ABSREL_POSITION);
                    w->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
                    w->col_bckgrnd = COLOR_WHITE;
                    w->col_border = COLOR_BLACK;
                    w->sz_border = 1;
                    w->w = grid.c;
                    w->h = grid.c;
                    w->x0 = i * grid.c;
                    w->y0 = j * grid.c;

                    UI_Pop();
                }
            }
        }

        if (GetSpace()) {
            grid.Print();
        }

        CbuiFrameEnd();
    }
    CbuiExit();
}

void TestIndexSelector() {
    printf("TestIndexSelector\n");

    MArena *a_tmp = InitBaselayer()->a_tmp;

    s32 len = 16;
    Array<bool> selector = InitArray<bool>(a_tmp, len);

    for (s32 i = 0; i < len; ++i) {
        if ((i > 0) && (i % 4 == 0)) {
            printf(" ");
        }

        selector.Add(RandMinMaxI(0, 1));
        printf("%d", selector.arr[i]);
        
    }
    printf("\n");

    s32 first = FirstTrue(selector);
    s32 last = LastTrue(selector);


    s32 idx = 5;
    s32 prev = PrevTrue(selector, idx);
    s32 next = NextTrue(selector, idx);

    printf("idx_first: %d, idx_last: %d\n", first, last);
    printf("idx_prev: %d << at idx: %d >> idx_next: %d\n", prev, idx, next);
}


struct MBlit {
    Str title;
    Str xmin;
    Str xmax;
    Str ymin;
    Str ymax;

    s32 w;
    s32 h;

    Array<f32> x;
    Array<f32> y;
};


MBlit MBlitGetExample(MArena *a_dest) {
    MBlit blit = {};

    blit.title = StrL("Monitor PSD Data");
    blit.xmin = StrL("0");
    blit.xmax = StrL("10");
    blit.ymin = StrL("0");
    blit.ymax = StrL("5");

    blit.w = 200;
    blit.h = 200;

    s32 len = 20;
    s32 xmin = 0;
    s32 xmax = 10;
    s32 ymin = 0;
    s32 ymax = 5;
    blit.x = InitArray<f32>(a_dest, len);
    blit.y = InitArray<f32>(a_dest, len);
    for (s32 i = 0; i < len; ++i) {
        blit.x.Add( RandMinMaxI_f32(xmin, xmax) );
        blit.y.Add( RandMinMaxI_f32(ymin, ymax) );
    }

    return blit;
}


void TestBlitMonitors() {
    printf("TestBlitMonitors\n");

    CbuiInit("TestBlitMonitors", false);
    while (cbui.running) {
        CbuiFrameStart();

        Widget *wrap = UI_Center();
        wrap->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
        wrap->col_bckgrnd = COLOR_GREEN;
        wrap->col_border = COLOR_BLACK;
        wrap->sz_border = 1;
        wrap->padding = 5;
        wrap->SetFlag(WF_ALIGN_CENTER);

        MBlit blit = MBlitGetExample(cbui.ctx->a_tmp);

        //Widget *plot = UI_LayoutHorizontal();
        Widget *plot = WidgetGetCached("plot1D");
        WidgetTreeSibling(plot);
        plot->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
        plot->col_bckgrnd = COLOR_WHITE;
        plot->col_border = COLOR_BLACK;
        plot->sz_border = 1;
        plot->w = blit.w;
        plot->h = blit.h;

        plot->rect.y0;

        UI_SetFontSize(FS_18);

        // plot the title
        s32 sz_x;
        s32 sz_y;
        s32 txt_l;
        s32 txt_t;
        TextPlot(blit.title, plot->rect.x0, plot->rect.y0, blit.w, blit.h, &sz_x, &sz_y, COLOR_BLUE, 0, 1);


        CbuiFrameEnd();
    }
    CbuiExit();
}


void Test() {
    //TestComponentFuncitonsRun();
    //TestMainLayout();
    //TestGridLayoutCalculations();
    //TestIndexSelector();
    TestBlitMonitors();
}

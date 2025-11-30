
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
    Str xlabel;
    
    f32 xmin;
    f32 xmax;
    f32 ymin;
    f32 ymax;

    s32 w;
    s32 h;

    Array<f32> x;
    Array<f32> y;
};

MBlit MBlitGetExample(MArena *a_dest) {
    MBlit blit = {};

    blit.title = StrL("Monitor Lin Data");
    blit.xlabel = StrL("dist [m]");

    blit.w = 200;
    blit.h = 200;

    s32 len = 20;
    blit.xmin = 0;
    blit.xmax = 10;
    blit.ymin = 0;
    blit.ymax = 5;
    blit.x = InitArray<f32>(a_dest, len);
    blit.y = InitArray<f32>(a_dest, len);

    f32 scale_x = (blit.xmax - blit.xmin) / len;
    f32 offset_x = blit.xmin;
    for (s32 i = 0; i < len; ++i) {
        blit.x.Add( i * scale_x + offset_x );
        blit.y.Add( RandMinMaxI_f32(blit.ymin, blit.ymax) );
    }

    return blit;
}


void TestBlitMonitors() {
    printf("TestBlitMonitors\n");

    CbuiInit("TestBlitMonitors", false);
    MBlit blit = MBlitGetExample(cbui.ctx->a_life);

    while (cbui.running) {
        CbuiFrameStart();

        Widget *wrap = UI_Center();
        wrap->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
        wrap->col_bckgrnd = COLOR_GREEN;
        wrap->col_border = COLOR_BLACK;
        wrap->sz_border = 1;
        wrap->padding = 5;
        wrap->SetFlag(WF_ALIGN_CENTER);

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

        // push a temp buffer
        Color *tmp_buff;
        Sprite s_mon = SpriteTexture_32it(cbui.ctx->a_tmp, "monitor_1D_example", blit.w, blit.h, plot->rect.x0, plot->rect.y0, &cbui.map_textures, &tmp_buff);
        SpriteBufferPush(s_mon);

        // get title and blit into our tmp buffer
        s32 sz_x;
        s32 sz_y;
        Array<Sprite> title = TextPlot(cbui.ctx->a_tmp, blit.title, 0, 0, blit.w, blit.h, &sz_x, &sz_y, COLOR_BLACK, 0, 1);
        SpriteArrayBlit(title, cbui.map_textures, s_mon.w, s_mon.h, tmp_buff);

        // x-label
        Array<Sprite> xlabel = TextPlot(cbui.ctx->a_tmp, blit.xlabel, 0, 0, blit.w, blit.h, &sz_x, &sz_y, COLOR_BLACK, 0, -1);
        SpriteArrayBlit(xlabel, cbui.map_textures, s_mon.w, s_mon.h, tmp_buff);


        // draw the axes
        s32 space_top = 15;
        s32 space_left = 15;
        s32 space_below = 15;
        RenderLineRGBA((u8*) tmp_buff, s_mon.w, s_mon.h, 0 + space_left, 0 + space_top, 0 + space_left, s_mon.h - 1 - space_below, COLOR_BLACK);
        RenderLineRGBA((u8*) tmp_buff, s_mon.w, s_mon.h, 0 + space_left, s_mon.h - 1 - space_below, s_mon.w, s_mon.h - 1 - space_below, COLOR_BLACK);

        // draw data segments
        f32 scale_x = (blit.w - space_left) / (blit.xmax - blit.xmin);
        f32 offset_x = space_left;
        f32 scale_y = -1 * (blit.h - space_top - space_below) / (blit.ymax - blit.ymin);
        f32 offset_y = blit.h - space_below;
        for (s32 i = 0; i < blit.x.len - 1; ++i) {
            f32 x1 = (blit.x.arr[i] - blit.xmin) * scale_x + offset_x;
            f32 y1 = (blit.y.arr[i] - blit.ymin) * scale_y + offset_y;

            f32 x2 = (blit.x.arr[i+1] - blit.xmin) * scale_x + offset_x;
            f32 y2 = (blit.y.arr[i+1] - blit.ymin) * scale_y + offset_y;

            RenderLineRGBA((u8*) tmp_buff, s_mon.w, s_mon.h, x1, y1-1, x2, y2-1, COLOR_BLACK);
        }

        CbuiFrameEnd();
    }
    CbuiExit();
}


void TestBlitSubRect() {
    printf("TestBlitSubRect\n");

    CbuiInit("TestBlitSubRect", false);

    s32 w = 200;
    s32 h = 200;
    // push a temp buffer
    Color *tmp_buff;
    Sprite s_mon = SpriteTexture_32it(cbui.ctx->a_life, "subrect_example", w, h, 20, 20, &cbui.map_textures, &tmp_buff);

    // fill the are with a green
    for (s32 j = 0; j < h; j++) {
        for (s32 i = 0; i < w; i++) {
            tmp_buff[ w * j + i ] = COLOR_BLUE;
        }
    }

    // the sub-rect is described using t_sub, l_sub, w_sub, h_sub
    //
    // the data is descried using an array of y values with xmin,xmax for the x-axis and ymin,ymax for the y axis
    // first, we need to create an index-map: It takes indices from the sub-rect into the buffer
    //
    //  x -> i
    //  y -> j
    //
    //  here i and j span the dest buffer from ULC: (0, 0) to LRC: (w, h)
    //  and we are blitting into the buffer 'buff' of size w * h
    //
    //  this is a linear function taking:
    //
    //  0, 0 -> t + h_sub, l
    //  w_sub, h_sub -> t, l + w_sub
    //
    //  thus:
    //  i = x + l_sub
    //  j = t_sub + h_sub - y

    //  2D:
    //  x and y maps to actual data using a sampling fucntion (we are essentailly re-doing the sprite blitting function)
    //  value = sample(x, y) is drawn at i,j:
    //
    //  sample(x, y) is the sprite sampling function: SampleTexture on colors_data
    //
    //  with:
    //
    //  w_data = (xmax - xmin)
    //  h_data = (ymax - ymin)
    //
    //  scale_x = 1 / w_sub
    //  scale_y = 1 / h_sub
    //
    //  x_frac = x * scale_x
    //  y_frac = y * scale_y
    //
    //  thus x_frac and y_frac go from zero to one when iterating from 0 to w_sub and h_sub (e.g. the sub rect)
    //  
    //  then:
    //
    //  color_i,j = SampleTexture(x_frac, y_frac, w_data, h_data, colors_data);
    //
    //  colors_data: zmin, zmax, k, l -> color which is constructed in the function MonitorDataBuffer2D
    //  (BUT we are looking to in-line it, and not have that be a separater loop though)
    //
    //  we can now write the loop that blits this data into buff.

    // create monitor-like 2D data (this is given by the system)
    s32 w_data = 500;
    s32 h_data = 500;
    f32 zmin = 5000;
    f32 zmax = 8000;
    f32 *data = (f32*) ArenaAlloc(cbui.ctx->a_life, w_data * h_data * sizeof(f32));
    for (s32 j = 0; j < h_data; ++j) {
        for (s32 i = 0; i < w_data; ++i) {

            f32 val = zmin;
            if (i > 100) {
                val = zmin + (zmax-zmin) * Rand01_f32();
            }
            else {
                val = 6000;
            }

            data[ j * w_data + i ] = val;
        }
    }

    // blit data buffer into the subrect
    s32 t_sub = 5;
    s32 l_sub = 15;
    s32 w_sub = 180;
    s32 h_sub = 180;

    f32 scale_x = 1.0f / w_sub;
    f32 scale_y = 1.0f / h_sub;

    s32 i, j;
    for (s32 y = 0; y < h_sub; y++) {
        j = t_sub + h_sub - 1 - y;
        for (s32 x = 0; x < w_sub; x++) {

            s32 x_data = (s32) round(w_data * scale_x * x);
            s32 y_data = (s32) round(h_data * scale_y * y);
            u32 idx = w_data * j + i;

            // get the proportianal data point
            f32 data_val = data[ w_data * y_data + x_data ];
            // scale the color map from zmin to zmax
            Color color_ij = ColorMapGet((data_val - zmin) / (zmax - zmin), colormap_paletted_jet);

            i = x + l_sub;
            tmp_buff[ w * j + i ] = color_ij;
        }
    }

    //  1D:
    //  We have xmin, xmax, ymin, ymax and an array of y-values
    //  We just need the x,y indices here, given each data "anchor"
    //
    //  x1, y1 -> x, y
    //  x = (x1 - xmin) / w_data
    //  y = (y1 - ymin) / h_data
    //
    //  lines are now drawn between pairs of consecutive anchors


    while (cbui.running) {
        CbuiFrameStart();

        SpriteBufferPush(s_mon);

        CbuiFrameEnd();
    }
    CbuiExit();
}


void Test() {
    //TestComponentFuncitonsRun();
    //TestMainLayout();
    //TestGridLayoutCalculations();
    //TestIndexSelector();
    //TestBlitMonitors();
    TestBlitSubRect();
}

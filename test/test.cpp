
void TestComponentFuncitonsRun() {
    TimeFunction;
    printf("TestComponentFuncitonsRun\n\n");

    // init
    MContext *ctx = InitBaselayer();


    s32 ncount = 1e6;
    InstrumentConfig config = InitAndConfig_PSI_DMC(ctx->a_pers, ncount);


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
    for (u32 j = 0; j < mcncount; ++j) {
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


    CbuiInit("insert_project_name", false);

    // TODO: init

    s32 padding_1 = 20;
    s32 padding_2 = 10;

    bool b1 = true;
    bool b2 = false;
    bool b3 = false;
    bool b4 = false;

    while (cbui.running) {
        CbuiFrameStart();

        //

        /*
        Widget *m1 = UI_Branch();
        m1->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
        m1->SetFlag(WF_EXPAND_HORIZONTAL);
        m1->SetFlag(WF_EXPAND_VERTICAL);
        m1->col_bckgrnd = COLOR_GREEN;
        */

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

        UI_Label("First");
        UI_Label("Second");
        UI_Label("Third");


        Widget *menu_align = UI_LayoutVertical(0);
        menu_align->SetFlag(WF_ABSREL_POSITION);
        menu_align->SetFlag(WF_EXPAND_HORIZONTAL);
        menu_align->y0 = - (padding_1 + padding_2) + 5;
        //menu_align->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
        //menu_align->col_border = COLOR_BLACK;
        //menu_align->sz_border = 1;


        Widget *menu_2 = UI_LayoutHorizontal();
        //menu_2->SetFlag(WF_ABSREL_POSITION);
        //menu_2->SetFlag(WF_EXPAND_HORIZONTAL);
        //menu_2->SetFlag(WF_EXPAND_VERTICAL);
        //menu_2->SetFlag(WF_DRAW_BACKGROUND_AND_BORDER);
        //menu_2->col_bckgrnd = COLOR_WHITE;
        //menu_2->col_border = COLOR_BLACK;
        menu_2->padding = padding_2;
        //menu_2->sz_border = 1;
        //menu_2->DBG_tag = StrL("H");


        if (UI_ToggleButton2("  Sim  ", &b1)) { b1 = true; b2 = false; b3 = false; b4 = false; }
        if (UI_ToggleButton2("  Run  ", &b2)) { b1 = false; b2 = true; b3 = false; b4 = false; }
        if (UI_ToggleButton2(" Trace ", &b3)) { b1 = false; b2 = false; b3 = true; b4 = false; }
        if (UI_ToggleButton2(" Plot ", &b4)) { b1 = false; b2 = false; b3 = false; b4 = true; }

        /*
        UI_Button("[Trace]");
        UI_Label("h_1");
        UI_Label("h_2");
        UI_Label("h_3");
        */


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
    }
    CbuiExit();

}


void Test() {
    //TestComponentFuncitonsRun();
    TestMainLayout();
}

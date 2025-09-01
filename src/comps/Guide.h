#ifndef __Guide__
#define __Guide__

// share block



struct Guide {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters
    char *reflect = (char*) 0;
    double w1;
    double h1;
    double w2 = 0;
    double h2 = 0;
    double l;
    double R0 = 0.99;
    double Qc = 0.0219;
    double alpha = 6.07;
    double m = 2;
    double W = 0.003;

    // declares
    t_Table pTable;
    int table_present;
};

Guide Create_Guide(s32 index, char *name) {
    Guide _comp = {};
    Guide *comp = &_comp;
    comp->type = (char*) "Guide";
    comp->name = name;
    comp->index = index;

    return _comp;
}

void Init_Guide(Guide *comp, Instrument *instrument) {

    #define reflect comp->reflect
    #define w1 comp->w1
    #define h1 comp->h1
    #define w2 comp->w2
    #define h2 comp->h2
    #define l comp->l
    #define R0 comp->R0
    #define Qc comp->Qc
    #define alpha comp->alpha
    #define m comp->m
    #define W comp->W

    #define pTable comp->pTable
    #define table_present comp->table_present
    ////////////////////////////////////////////////////////////////


    if (mcgravitation)
        fprintf(stderr,"WARNING: Guide: %s: "
            "This component produces wrong results with gravitation !\n"
            "Use Guide_gravity.\n",
            NAME_CURRENT_COMP);

    if (!w2) w2=w1;
    if (!h2) h2=h1;

    if (reflect && strlen(reflect) && strcmp(reflect,"NULL") && strcmp(reflect,"0")) {
        /* read 1st block data from file into pTable */
        if (Table_Read(&pTable, reflect, 1) <= 0) 
            exit(fprintf(stderr,"Guide: %s: can not read file %s\n", NAME_CURRENT_COMP, reflect));

        table_present=1;
    }
    else {
        table_present = 0;
        if (W < 0 || R0 < 0 || Qc < 0 || m < 0) {
            fprintf(stderr,"Guide: %s: W R0 Qc must be >0.\n", NAME_CURRENT_COMP);
            exit(-1); }
    }


    ////////////////////////////////////////////////////////////////
    #undef reflect
    #undef w1
    #undef h1
    #undef w2
    #undef h2
    #undef l
    #undef R0
    #undef Qc
    #undef alpha
    #undef m
    #undef W

    #undef pTable
    #undef table_present

}

void Trace_Guide(Guide *comp, Neutron *particle, Instrument *instrument) {
    #define x particle->x
    #define y particle->y
    #define z particle->z
    #define vx particle->vx
    #define vy particle->vy
    #define vz particle->vz
    #define sx particle->sx
    #define sy particle->sy
    #define sz particle->sz
    #define t particle->t
    #define p particle->p

    #define reflect comp->reflect
    #define w1 comp->w1
    #define h1 comp->h1
    #define w2 comp->w2
    #define h2 comp->h2
    #define l comp->l
    #define R0 comp->R0
    #define Qc comp->Qc
    #define alpha comp->alpha
    #define m comp->m
    #define W comp->W

    #define pTable comp->pTable
    #define table_present comp->table_present
    ////////////////////////////////////////////////////////////////


    double t1,t2;                                 /* Intersection times. */
    double av,ah,bv,bh,cv1,cv2,ch1,ch2,d;         /* Intermediate values */
    double weight;                                /* Internal probability weight */
    double vdotn_v1,vdotn_v2,vdotn_h1,vdotn_h2;   /* Dot products. */
    int i;                                        /* Which mirror hit? */
    double q;                                     /* Q [1/AA] of reflection */
    double nlen2;                                 /* Vector lengths squared */
    double par[5] = {R0, Qc, alpha, m, W};
    
    /* ToDo: These could be precalculated. */
    double ww = .5*(w2 - w1), hh = .5*(h2 - h1);
    double whalf = .5*w1, hhalf = .5*h1;

    /* Propagate neutron to guide entrance. */
    PROP_Z0;
    /* Scatter here to ensure that fully transmitted neutrons will not be
        absorbed in a GROUP construction, e.g. all neutrons - even the
        later absorbed ones are scattered at the guide entry. */
    SCATTER;
    if(x <= -whalf || x >= whalf || y <= -hhalf || y >= hhalf)
        ABSORB;

    for(;;)
    {
        /* Compute the dot products of v and n for the four mirrors. */
        av = l*vx; bv = ww*vz;
        ah = l*vy; bh = hh*vz;
        vdotn_v1 = bv + av;         /* Left vertical */
        vdotn_v2 = bv - av;         /* Right vertical */
        vdotn_h1 = bh + ah;         /* Lower horizontal */
        vdotn_h2 = bh - ah;         /* Upper horizontal */
        /* Compute the dot products of (O - r) and n as c1+c2 and c1-c2 */
        cv1 = -whalf*l - z*ww; cv2 = x*l;
        ch1 = -hhalf*l - z*hh; ch2 = y*l;

        /* Compute intersection times. */
        t1 = (l - z)/vz;
        i = 0;
        if(vdotn_v1 < 0 && (t2 = (cv1 - cv2)/vdotn_v1) < t1)
        {
            t1 = t2;
            i = 1;
        }
        if(vdotn_v2 < 0 && (t2 = (cv1 + cv2)/vdotn_v2) < t1)
        {
            t1 = t2;
            i = 2;
        }
        if(vdotn_h1 < 0 && (t2 = (ch1 - ch2)/vdotn_h1) < t1)
        {
            t1 = t2;
            i = 3;
        }
        if(vdotn_h2 < 0 && (t2 = (ch1 + ch2)/vdotn_h2) < t1)
        {
            t1 = t2;
            i = 4;
        }
        if(i == 0)
            break;                    /* Neutron left guide. */
        PROP_DT(t1);

        switch(i)
        {
        case 1:                   /* Left vertical mirror */
            nlen2 = l*l + ww*ww;
            q = V2Q*(-2)*vdotn_v1/sqrt(nlen2);
            d = 2*vdotn_v1/nlen2;
            vx = vx - d*l;
            vz = vz - d*ww;
            break;
        case 2:                   /* Right vertical mirror */
            nlen2 = l*l + ww*ww;
            q = V2Q*(-2)*vdotn_v2/sqrt(nlen2);
            d = 2*vdotn_v2/nlen2;
            vx = vx + d*l;
            vz = vz - d*ww;
            break;
        case 3:                   /* Lower horizontal mirror */
            nlen2 = l*l + hh*hh;
            q = V2Q*(-2)*vdotn_h1/sqrt(nlen2);
            d = 2*vdotn_h1/nlen2;
            vy = vy - d*l;
            vz = vz - d*hh;
            break;
        case 4:                   /* Upper horizontal mirror */
            nlen2 = l*l + hh*hh;
            q = V2Q*(-2)*vdotn_h2/sqrt(nlen2);
            d = 2*vdotn_h2/nlen2;
            vy = vy + d*l;
            vz = vz - d*hh;
            break;
        }

        /* Now compute reflectivity. */
        weight = 1.0; /* Initial internal weight factor */
        if(m == 0)
            ABSORB;
        if (reflect && table_present==1)
            TableReflecFunc(q, &pTable, &weight);
        else {
            StdReflecFunc(q, par, &weight);
        }
        if (weight > 0)
            p *= weight;
        else
            ABSORB;
        SCATTER;
    }


    ////////////////////////////////////////////////////////////////
    #undef reflect
    #undef w1
    #undef h1
    #undef w2
    #undef h2
    #undef l
    #undef R0
    #undef Qc
    #undef alpha
    #undef m
    #undef W

    #undef pTable
    #undef table_present

    #undef x
    #undef y
    #undef z
    #undef vx
    #undef vy
    #undef vz
    #undef sx
    #undef sy
    #undef sz
    #undef t
    #undef p
}

void Save_Guide(Guide *comp) {

}

void Finally_Guide(Guide *comp) {

}

void Display_Guide(Guide *comp) {
    #define magnify mcdis_magnify
    #define line mcdis_line
    #define dashed_line mcdis_dashed_line
    #define multiline mcdis_multiline
    #define rectangle mcdis_rectangle
    #define box mcdis_box
    #define circle mcdis_circle
    #define Circle mcdis_Circle
    #define cylinder mcdis_cylinder
    #define cone mcdis_cone
    #define sphere mcdis_sphere

    #define reflect comp->reflect
    #define w1 comp->w1
    #define h1 comp->h1
    #define w2 comp->w2
    #define h2 comp->h2
    #define l comp->l
    #define R0 comp->R0
    #define Qc comp->Qc
    #define alpha comp->alpha
    #define m comp->m
    #define W comp->W

    #define pTable comp->pTable
    #define table_present comp->table_present
    ////////////////////////////////////////////////////////////////


    /* V2, draw top, bottom, sides independently: */
    // TOP
    multiline(5,
              -w1/2.0, h1/2.0, 0.0,
               w1/2.0, h1/2.0, 0.0,
    	     w2/2.0, h2/2.0, (double)l,
              -w2/2.0, h2/2.0, (double)l,
              -w1/2.0, h1/2.0, 0.0);
    // BOTTOM
    multiline(5,
              -w1/2.0, -h1/2.0, 0.0,
               w1/2.0, -h1/2.0, 0.0,
    	     w2/2.0, -h2/2.0, (double)l,
              -w2/2.0, -h2/2.0, (double)l,
              -w1/2.0, -h1/2.0, 0.0);

    // RIGHT
    multiline(5,
              -w1/2.0, h1/2.0, 0.0,
              -w1/2.0, -h1/2.0, 0.0,
    	    -w2/2.0, -h2/2.0, (double)l,
              -w2/2.0, h2/2.0, (double)l,
    	    -w1/2.0, h1/2.0, 0.0);

    // LEFT
    multiline(5,
              w1/2.0, h1/2.0, 0.0,
              w1/2.0, -h1/2.0, 0.0,
    	    w2/2.0, -h2/2.0, (double)l,
              w2/2.0, h2/2.0, (double)l,
    	    w1/2.0, h1/2.0, 0.0);



    /* Original implementation: 
    multiline(5,
        -w1/2.0, -h1/2.0, 0.0,
        w1/2.0, -h1/2.0, 0.0,
        w1/2.0,  h1/2.0, 0.0,
        -w1/2.0,  h1/2.0, 0.0,
        -w1/2.0, -h1/2.0, 0.0);
    multiline(5,
        -w2/2.0, -h2/2.0, (double)l,
        w2/2.0, -h2/2.0, (double)l,
        w2/2.0,  h2/2.0, (double)l,
        -w2/2.0,  h2/2.0, (double)l,
        -w2/2.0, -h2/2.0, (double)l);
    line(-w1/2.0, -h1/2.0, 0, -w2/2.0, -h2/2.0, (double)l);
    line( w1/2.0, -h1/2.0, 0,  w2/2.0, -h2/2.0, (double)l);
    line( w1/2.0,  h1/2.0, 0,  w2/2.0,  h2/2.0, (double)l);
    line(-w1/2.0,  h1/2.0, 0, -w2/2.0,  h2/2.0, (double)l);
    */


    ////////////////////////////////////////////////////////////////
    #undef reflect
    #undef w1
    #undef h1
    #undef w2
    #undef h2
    #undef l
    #undef R0
    #undef Qc
    #undef alpha
    #undef m
    #undef W

    #undef pTable
    #undef table_present

    #undef magnify
    #undef line
    #undef dashed_line
    #undef multiline
    #undef rectangle
    #undef box
    #undef circle
    #undef Circle
    #undef cylinder
    #undef cone
    #undef sphere
}

#endif

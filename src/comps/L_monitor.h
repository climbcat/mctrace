#ifndef __L_monitor__
#define __L_monitor__

// share block



struct L_monitor {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters
    int nL = 20;
    char *filename = (char*) 0;
    int nowritefile = 0;
    double xmin = -0.05;
    double xmax = 0.05;
    double ymin = -0.05;
    double ymax = 0.05;
    double xwidth = 0;
    double yheight = 0;
    double Lmin;
    double Lmax;
    int restore_neutron = 0;

    // declares
    DArray1d L_N;
    DArray1d L_p;
    DArray1d L_p2;
};

L_monitor Create_L_monitor(s32 index, char *name) {
    L_monitor _comp = {};
    L_monitor *comp = &_comp;
    comp->type = (char*) "L_monitor";
    comp->name = name;
    comp->index = index;

    return _comp;
}

void Init_L_monitor(L_monitor *comp, Instrument *instrument) {

    #define nL comp->nL
    #define filename comp->filename
    #define nowritefile comp->nowritefile
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define Lmin comp->Lmin
    #define Lmax comp->Lmax
    #define restore_neutron comp->restore_neutron

    #define L_N comp->L_N
    #define L_p comp->L_p
    #define L_p2 comp->L_p2
    ////////////////////////////////////////////////////////////////


    if (xwidth  > 0) { xmax = xwidth/2;  xmin = -xmax; }
    if (yheight > 0) { ymax = yheight/2; ymin = -ymax; }

    if ((xmin >= xmax) || (ymin >= ymax)) {
        printf("L_monitor: %s: Null detection area !\n"
        "ERROR      (xwidth,yheight,xmin,xmax,ymin,ymax). Exiting",
        NAME_CURRENT_COMP);
        exit(0);
    }

    L_N = create_darr1d(nL);
    L_p = create_darr1d(nL);
    L_p2 = create_darr1d(nL);

    // Use instance name for monitor output if no input was given
    if (!strcmp(filename,"\0")) sprintf(filename,"%s",NAME_CURRENT_COMP);


    ////////////////////////////////////////////////////////////////
    #undef nL
    #undef filename
    #undef nowritefile
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef xwidth
    #undef yheight
    #undef Lmin
    #undef Lmax
    #undef restore_neutron

    #undef L_N
    #undef L_p
    #undef L_p2

}

void Trace_L_monitor(L_monitor *comp, Neutron *particle, Instrument *instrument) {
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

    #define nL comp->nL
    #define filename comp->filename
    #define nowritefile comp->nowritefile
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define Lmin comp->Lmin
    #define Lmax comp->Lmax
    #define restore_neutron comp->restore_neutron

    #define L_N comp->L_N
    #define L_p comp->L_p
    #define L_p2 comp->L_p2
    ////////////////////////////////////////////////////////////////


    PROP_Z0;
    if (x>xmin && x<xmax && y>ymin && y<ymax) {
        double L = (2*PI/V2K)/sqrt(vx*vx + vy*vy + vz*vz);
        int i = floor((L-Lmin)*nL/(Lmax-Lmin));
        if(i >= 0 && i < nL) {
            double p2 = p*p;
            #pragma acc atomic
            L_N[i] = L_N[i] +1;
            #pragma acc atomic
            L_p[i] = L_p[i] + p;
            #pragma acc atomic
            L_p2[i] = L_p2[i] + p2;
            SCATTER;
        }
    }
    if (restore_neutron) {
        RESTORE_NEUTRON(INDEX_CURRENT_COMP, x, y, z, vx, vy, vz, t, sx, sy, sz, p);
    }


    ////////////////////////////////////////////////////////////////
    #undef nL
    #undef filename
    #undef nowritefile
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef xwidth
    #undef yheight
    #undef Lmin
    #undef Lmax
    #undef restore_neutron

    #undef L_N
    #undef L_p
    #undef L_p2

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

void Save_L_monitor(L_monitor *comp) {

    #define nL comp->nL
    #define filename comp->filename
    #define nowritefile comp->nowritefile
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define Lmin comp->Lmin
    #define Lmax comp->Lmax
    #define restore_neutron comp->restore_neutron

    #define L_N comp->L_N
    #define L_p comp->L_p
    #define L_p2 comp->L_p2
    ////////////////////////////////////////////////////////////////


    if (!nowritefile) {
        DETECTOR_OUT_1D(
            "Wavelength monitor",
            "Wavelength [AA]",
            "Intensity",
            "L", Lmin, Lmax, nL,
            &L_N[0],&L_p[0],&L_p2[0],
            filename);
    }


    ////////////////////////////////////////////////////////////////
    #undef nL
    #undef filename
    #undef nowritefile
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef xwidth
    #undef yheight
    #undef Lmin
    #undef Lmax
    #undef restore_neutron

    #undef L_N
    #undef L_p
    #undef L_p2
}

void Finally_L_monitor(L_monitor *comp) {

    #define nL comp->nL
    #define filename comp->filename
    #define nowritefile comp->nowritefile
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define Lmin comp->Lmin
    #define Lmax comp->Lmax
    #define restore_neutron comp->restore_neutron

    #define L_N comp->L_N
    #define L_p comp->L_p
    #define L_p2 comp->L_p2
    ////////////////////////////////////////////////////////////////


    destroy_darr1d(L_N);
    destroy_darr1d(L_p);
    destroy_darr1d(L_p2);


    ////////////////////////////////////////////////////////////////
    #undef nL
    #undef filename
    #undef nowritefile
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef xwidth
    #undef yheight
    #undef Lmin
    #undef Lmax
    #undef restore_neutron

    #undef L_N
    #undef L_p
    #undef L_p2
}

void Display_L_monitor(L_monitor *comp) {
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

    #define nL comp->nL
    #define filename comp->filename
    #define nowritefile comp->nowritefile
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define Lmin comp->Lmin
    #define Lmax comp->Lmax
    #define restore_neutron comp->restore_neutron

    #define L_N comp->L_N
    #define L_p comp->L_p
    #define L_p2 comp->L_p2
    ////////////////////////////////////////////////////////////////


    multiline(5, (double)xmin, (double)ymin, 0.0,
                (double)xmax, (double)ymin, 0.0,
                (double)xmax, (double)ymax, 0.0,
                (double)xmin, (double)ymax, 0.0,
                (double)xmin, (double)ymin, 0.0);


    ////////////////////////////////////////////////////////////////
    #undef nL
    #undef filename
    #undef nowritefile
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef xwidth
    #undef yheight
    #undef Lmin
    #undef Lmax
    #undef restore_neutron

    #undef L_N
    #undef L_p
    #undef L_p2

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

#ifndef __PSDlin_monitor__
#define __PSDlin_monitor__

// share block



struct PSDlin_monitor {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters
    int nbins = 20;
    char *filename = (char*) 0;
    double xmin = -0.05;
    double xmax = 0.05;
    double ymin = -0.05;
    double ymax = 0.05;
    int nowritefile = 0;
    double xwidth = 0;
    double yheight = 0;
    int restore_neutron = 0;
    int vertical = 0;

    // declares
    DArray1d PSDlin_N;
    DArray1d PSDlin_p;
    DArray1d PSDlin_p2;
};

PSDlin_monitor Create_PSDlin_monitor(s32 index, char *name) {
    PSDlin_monitor _comp = {};
    PSDlin_monitor *comp = &_comp;
    comp->type = (char*) "PSDlin_monitor";
    comp->name = name;
    comp->index = index;

    return _comp;
}

void Init_PSDlin_monitor(PSDlin_monitor *comp, Instrument *instrument) {

    #define nbins comp->nbins
    #define filename comp->filename
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define nowritefile comp->nowritefile
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define restore_neutron comp->restore_neutron
    #define vertical comp->vertical

    #define PSDlin_N comp->PSDlin_N
    #define PSDlin_p comp->PSDlin_p
    #define PSDlin_p2 comp->PSDlin_p2
    ////////////////////////////////////////////////////////////////


    if (xwidth  > 0) { xmax = xwidth/2;  xmin = -xmax; }
    if (yheight > 0) { ymax = yheight/2; ymin = -ymax; }

    if ((xmin >= xmax) || (ymin >= ymax)) {
        printf("PSDlin_monitor: %s: Null detection area !\n"
            "ERROR           (xwidth,yheight,xmin,xmax,ymin,ymax). Exiting",
            NAME_CURRENT_COMP);
        exit(0);
    }

    PSDlin_N = create_darr1d(nbins);
    PSDlin_p = create_darr1d(nbins);
    PSDlin_p2 = create_darr1d(nbins);

    // Use instance name for monitor output if no input was given
    if (!strcmp(filename,"\0")) sprintf(filename,"%s",NAME_CURRENT_COMP);


    ////////////////////////////////////////////////////////////////
    #undef nbins
    #undef filename
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef nowritefile
    #undef xwidth
    #undef yheight
    #undef restore_neutron
    #undef vertical

    #undef PSDlin_N
    #undef PSDlin_p
    #undef PSDlin_p2

}

void Trace_PSDlin_monitor(PSDlin_monitor *comp, Neutron *particle, Instrument *instrument) {
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

    #define nbins comp->nbins
    #define filename comp->filename
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define nowritefile comp->nowritefile
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define restore_neutron comp->restore_neutron
    #define vertical comp->vertical

    #define PSDlin_N comp->PSDlin_N
    #define PSDlin_p comp->PSDlin_p
    #define PSDlin_p2 comp->PSDlin_p2
    ////////////////////////////////////////////////////////////////


    int i;

    PROP_Z0;
    if (x>xmin && x<xmax && y>ymin && y<ymax) {
        /* Bin number */
        if (!vertical) {
            i = floor(nbins*(x-xmin)/(xmax-xmin));
        }
        else {
            i = floor(nbins*(y-ymin)/(ymax-ymin));
        }
        if((i >= nbins) || (i<0)) {
            printf("ERROR: (%s) wrong positioning in linear PSD. i= %i \n",NAME_CURRENT_COMP,i);
            exit(1);
        }

        double p2 = p*p;

        #pragma acc atomic
        PSDlin_N[i] = PSDlin_N[i] +1;
        #pragma acc atomic
        PSDlin_p[i] = PSDlin_p[i] + p;
        #pragma acc atomic
        PSDlin_p2[i] = PSDlin_p2[i] + p2;

        SCATTER;
    }
    if (restore_neutron) {
        RESTORE_NEUTRON(INDEX_CURRENT_COMP, x, y, z, vx, vy, vz, t, sx, sy, sz, p);
    }


    ////////////////////////////////////////////////////////////////
    #undef nbins
    #undef filename
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef nowritefile
    #undef xwidth
    #undef yheight
    #undef restore_neutron
    #undef vertical

    #undef PSDlin_N
    #undef PSDlin_p
    #undef PSDlin_p2

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

void Save_PSDlin_monitor(PSDlin_monitor *comp) {

    #define nbins comp->nbins
    #define filename comp->filename
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define nowritefile comp->nowritefile
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define restore_neutron comp->restore_neutron
    #define vertical comp->vertical

    #define PSDlin_N comp->PSDlin_N
    #define PSDlin_p comp->PSDlin_p
    #define PSDlin_p2 comp->PSDlin_p2
    ////////////////////////////////////////////////////////////////


    if (!nowritefile) {
        if (!vertical) {
            DETECTOR_OUT_1D(
                "Linear PSD monitor","x-Position [m]","Intensity","x", xmin, xmax, nbins,
                &PSDlin_N[0],&PSDlin_p[0],&PSDlin_p2[0],filename);
        }
        else {
            DETECTOR_OUT_1D(
                "Linear PSD monitor","y-Position [m]","Intensity","y", ymin, ymax, nbins,
                &PSDlin_N[0],&PSDlin_p[0],&PSDlin_p2[0],filename);
        }
    }


    ////////////////////////////////////////////////////////////////
    #undef nbins
    #undef filename
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef nowritefile
    #undef xwidth
    #undef yheight
    #undef restore_neutron
    #undef vertical

    #undef PSDlin_N
    #undef PSDlin_p
    #undef PSDlin_p2
}

void Finally_PSDlin_monitor(PSDlin_monitor *comp) {

    #define nbins comp->nbins
    #define filename comp->filename
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define nowritefile comp->nowritefile
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define restore_neutron comp->restore_neutron
    #define vertical comp->vertical

    #define PSDlin_N comp->PSDlin_N
    #define PSDlin_p comp->PSDlin_p
    #define PSDlin_p2 comp->PSDlin_p2
    ////////////////////////////////////////////////////////////////


    destroy_darr1d(PSDlin_N);
    destroy_darr1d(PSDlin_p);
    destroy_darr1d(PSDlin_p2);


    ////////////////////////////////////////////////////////////////
    #undef nbins
    #undef filename
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef nowritefile
    #undef xwidth
    #undef yheight
    #undef restore_neutron
    #undef vertical

    #undef PSDlin_N
    #undef PSDlin_p
    #undef PSDlin_p2
}

void Display_PSDlin_monitor(PSDlin_monitor *comp) {
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

    #define nbins comp->nbins
    #define filename comp->filename
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define nowritefile comp->nowritefile
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define restore_neutron comp->restore_neutron
    #define vertical comp->vertical

    #define PSDlin_N comp->PSDlin_N
    #define PSDlin_p comp->PSDlin_p
    #define PSDlin_p2 comp->PSDlin_p2
    ////////////////////////////////////////////////////////////////


    multiline(5,
        (double)xmin, (double)ymin, 0.0,
        (double)xmax, (double)ymin, 0.0,
        (double)xmax, (double)ymax, 0.0,
        (double)xmin, (double)ymax, 0.0,
        (double)xmin, (double)ymin, 0.0);


    ////////////////////////////////////////////////////////////////
    #undef nbins
    #undef filename
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef nowritefile
    #undef xwidth
    #undef yheight
    #undef restore_neutron
    #undef vertical

    #undef PSDlin_N
    #undef PSDlin_p
    #undef PSDlin_p2

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

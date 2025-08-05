#ifndef __PSD_monitor__
#define __PSD_monitor__

// share block



struct PSD_monitor {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters
    int nx = 90;
    int ny = 90;
    char *filename = (char*) 0;
    double xmin = -0.05;
    double xmax = 0.05;
    double ymin = -0.05;
    double ymax = 0.05;
    double xwidth = 0;
    double yheight = 0;
    int restore_neutron = 0;
    int nowritefile = 0;

    // declares
    DArray2d PSD_N;
    DArray2d PSD_p;
    DArray2d PSD_p2;
};

PSD_monitor Create_PSD_monitor(s32 index, char *name) {
    PSD_monitor _comp = {};
    PSD_monitor *comp = &_comp;
    comp->type = (char*) "PSD_monitor";
    comp->name = name;
    comp->index = index;

    return _comp;
}

void Init_PSD_monitor(PSD_monitor *comp, Instrument *instrument) {

    #define nx comp->nx
    #define ny comp->ny
    #define filename comp->filename
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define restore_neutron comp->restore_neutron
    #define nowritefile comp->nowritefile

    #define PSD_N comp->PSD_N
    #define PSD_p comp->PSD_p
    #define PSD_p2 comp->PSD_p2
    ////////////////////////////////////////////////////////////////


    if (xwidth  > 0) { xmax = xwidth/2;  xmin = -xmax; }
    if (yheight > 0) { ymax = yheight/2; ymin = -ymax; }

    if ((xmin >= xmax) || (ymin >= ymax)){
        printf("PSD_monitor: %s: Null detection area !\n"
            "ERROR        (xwidth,yheight,xmin,xmax,ymin,ymax). Exiting",
        NAME_CURRENT_COMP);
        exit(0);
    }

    PSD_N = create_darr2d(nx, ny);
    PSD_p = create_darr2d(nx, ny);
    PSD_p2 = create_darr2d(nx, ny);

    // Use instance name for monitor output if no input was given
    if (!strcmp(filename,"\0")) sprintf(filename,"%s",NAME_CURRENT_COMP);


    ////////////////////////////////////////////////////////////////
    #undef nx
    #undef ny
    #undef filename
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef xwidth
    #undef yheight
    #undef restore_neutron
    #undef nowritefile

    #undef PSD_N
    #undef PSD_p
    #undef PSD_p2

}

void Trace_PSD_monitor(PSD_monitor *comp, Neutron *particle, Instrument *instrument) {
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

    #define nx comp->nx
    #define ny comp->ny
    #define filename comp->filename
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define restore_neutron comp->restore_neutron
    #define nowritefile comp->nowritefile

    #define PSD_N comp->PSD_N
    #define PSD_p comp->PSD_p
    #define PSD_p2 comp->PSD_p2
    ////////////////////////////////////////////////////////////////


    PROP_Z0;
    if (x>xmin && x<xmax && y>ymin && y<ymax){
        int i = floor((x - xmin)*nx/(xmax - xmin));
        int j = floor((y - ymin)*ny/(ymax - ymin));

        double p2 = p*p;
        #pragma acc atomic
        PSD_N[i][j] = PSD_N[i][j]+1;

        #pragma acc atomic
        PSD_p[i][j] = PSD_p[i][j]+p;
        
        #pragma acc atomic
        PSD_p2[i][j] = PSD_p2[i][j] + p2;
        
        SCATTER;
    }
    if (restore_neutron) {
        RESTORE_NEUTRON(INDEX_CURRENT_COMP, x, y, z, vx, vy, vz, t, sx, sy, sz, p);
    }


    ////////////////////////////////////////////////////////////////
    #undef nx
    #undef ny
    #undef filename
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef xwidth
    #undef yheight
    #undef restore_neutron
    #undef nowritefile

    #undef PSD_N
    #undef PSD_p
    #undef PSD_p2

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

void Save_PSD_monitor(PSD_monitor *comp) {

    #define nx comp->nx
    #define ny comp->ny
    #define filename comp->filename
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define restore_neutron comp->restore_neutron
    #define nowritefile comp->nowritefile

    #define PSD_N comp->PSD_N
    #define PSD_p comp->PSD_p
    #define PSD_p2 comp->PSD_p2
    ////////////////////////////////////////////////////////////////


    if (!nowritefile) {
        DETECTOR_OUT_2D(
            "PSD monitor",
            "X position [cm]",
            "Y position [cm]",
            xmin*100.0, xmax*100.0, ymin*100.0, ymax*100.0,
            nx, ny,
            &PSD_N[0][0],&PSD_p[0][0],&PSD_p2[0][0],
            filename);
    }


    ////////////////////////////////////////////////////////////////
    #undef nx
    #undef ny
    #undef filename
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef xwidth
    #undef yheight
    #undef restore_neutron
    #undef nowritefile

    #undef PSD_N
    #undef PSD_p
    #undef PSD_p2
}

void Finally_PSD_monitor(PSD_monitor *comp) {

    #define nx comp->nx
    #define ny comp->ny
    #define filename comp->filename
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define restore_neutron comp->restore_neutron
    #define nowritefile comp->nowritefile

    #define PSD_N comp->PSD_N
    #define PSD_p comp->PSD_p
    #define PSD_p2 comp->PSD_p2
    ////////////////////////////////////////////////////////////////


    destroy_darr2d(PSD_N);
    destroy_darr2d(PSD_p);
    destroy_darr2d(PSD_p2);


    ////////////////////////////////////////////////////////////////
    #undef nx
    #undef ny
    #undef filename
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef xwidth
    #undef yheight
    #undef restore_neutron
    #undef nowritefile

    #undef PSD_N
    #undef PSD_p
    #undef PSD_p2
}

void Display_PSD_monitor(PSD_monitor *comp) {
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

    #define nx comp->nx
    #define ny comp->ny
    #define filename comp->filename
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define restore_neutron comp->restore_neutron
    #define nowritefile comp->nowritefile

    #define PSD_N comp->PSD_N
    #define PSD_p comp->PSD_p
    #define PSD_p2 comp->PSD_p2
    ////////////////////////////////////////////////////////////////


    multiline(5, (double)xmin, (double)ymin, 0.0,
        (double)xmax, (double)ymin, 0.0,
        (double)xmax, (double)ymax, 0.0,
        (double)xmin, (double)ymax, 0.0,
        (double)xmin, (double)ymin, 0.0);


    ////////////////////////////////////////////////////////////////
    #undef nx
    #undef ny
    #undef filename
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef xwidth
    #undef yheight
    #undef restore_neutron
    #undef nowritefile

    #undef PSD_N
    #undef PSD_p
    #undef PSD_p2

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

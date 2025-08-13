#ifndef __Beamstop__
#define __Beamstop__

// share block



struct Beamstop {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters
    double xmin = -0.05;
    double xmax = 0.05;
    double ymin = -0.05;
    double ymax = 0.05;
    double xwidth = 0;
    double yheight = 0;
    double radius = 0;

    // declares
};

Beamstop Create_Beamstop(s32 index, char *name) {
    Beamstop _comp = {};
    Beamstop *comp = &_comp;
    comp->type = (char*) "Beamstop";
    comp->name = name;
    comp->index = index;

    return _comp;
}

void Init_Beamstop(Beamstop *comp, Instrument *instrument) {

    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define radius comp->radius

    ////////////////////////////////////////////////////////////////


    if (xwidth  > 0) {
        xmax = xwidth/2;  xmin = -xmax;
    }
    if (yheight > 0) {
        ymax = yheight/2; ymin = -ymax;
    }
    if (xmin == 0 && xmax == 0 && ymin == 0 & ymax == 0 && radius == 0) {
        fprintf(stderr,"Beamstop: %s: Error: give geometry\n", NAME_CURRENT_COMP); exit(-1);
    }


    ////////////////////////////////////////////////////////////////
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef xwidth
    #undef yheight
    #undef radius


}

void Trace_Beamstop(Beamstop *comp, Neutron *particle, Instrument *instrument) {
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

    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define radius comp->radius

    ////////////////////////////////////////////////////////////////


    double Time = t;
    ALLOW_BACKPROP;
    PROP_Z0;
    Time = t - Time;
    if ((Time>=0) && ((radius!=0) && (x*x + y*y <= radius*radius))
        || ((Time>=0) && (radius==0) && (x>xmin && x<xmax && y>ymin && y<ymax)))
        ABSORB;
    else
        RESTORE_NEUTRON(INDEX_CURRENT_COMP, x, y, z, vx, vy, vz, t, sx, sy, sz, p);


    ////////////////////////////////////////////////////////////////
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef xwidth
    #undef yheight
    #undef radius


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

void Save_Beamstop(Beamstop *comp) {

}

void Finally_Beamstop(Beamstop *comp) {

}

void Display_Beamstop(Beamstop *comp) {
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

    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define radius comp->radius

    ////////////////////////////////////////////////////////////////


    if (radius != 0)
        circle( (char*) "xy", 0, 0, 0, radius);
    else
        multiline(5,
            (double)xmin, (double)ymin, 0.0,
            (double)xmax, (double)ymin, 0.0,
            (double)xmax, (double)ymax, 0.0,
            (double)xmin, (double)ymax, 0.0,
            (double)xmin, (double)ymin, 0.0
        );


    ////////////////////////////////////////////////////////////////
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef xwidth
    #undef yheight
    #undef radius


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

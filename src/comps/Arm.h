#ifndef __Arm__
#define __Arm__

// share block



struct Arm {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters

    // declares
};

Arm Create_Arm(s32 index, char *name) {
    Arm _comp = {};
    Arm *comp = &_comp;
    comp->type = (char*) "Arm";
    comp->name = name;
    comp->index = index;

    return _comp;
}

void Init_Arm(Arm *comp, Instrument *instrument) {

}

void Trace_Arm(Arm *comp, Neutron *particle, Instrument *instrument) {
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


    ////////////////////////////////////////////////////////////////




    ////////////////////////////////////////////////////////////////


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

void Save_Arm(Arm *comp) {

}

void Finally_Arm(Arm *comp) {

}

void Display_Arm(Arm *comp) {
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


    ////////////////////////////////////////////////////////////////


    /* A bit ugly; hard-coded dimensions. */
    line(0,0,0,0.2,0,0);
    line(0,0,0,0,0.2,0);
    line(0,0,0,0,0,0.2);

    cone(0.2,0,0,0.01,0.02,1,0,0);
    cone(0,0.2,0,0.01,0.02,0,1,0);
    cone(0,0,0.2,0.01,0.02,0,0,1);


    ////////////////////////////////////////////////////////////////


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

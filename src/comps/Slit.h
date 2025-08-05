#ifndef __Slit__
#define __Slit__

// share block


void slit_print_if(int condition, char* level, char* message, char* component){
    if (condition) fprintf(stderr, "Slit: %s: %s: %s\n", component, level, message);
} 
void slit_error_if(int condition, char* message, char* component){
    slit_print_if(condition, (char*) "Error", message, component);
    if (condition) exit(-1);
}
void slit_warning_if(int condition, char* message, char* component){
    slit_print_if(condition, (char*) "Warning", message, component);
}


struct Slit {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters
    double xmin = UNSET;
    double xmax = UNSET;
    double ymin = UNSET;
    double ymax = UNSET;
    double radius = UNSET;
    double xwidth = UNSET;
    double yheight = UNSET;

    // declares
    char isradial;
};

Slit Create_Slit(s32 index, char *name) {
    Slit _comp = {};
    Slit *comp = &_comp;
    comp->type = (char*) "Slit";
    comp->name = name;
    comp->index = index;

    return _comp;
}

void Init_Slit(Slit *comp, Instrument *instrument) {

    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define radius comp->radius
    #define xwidth comp->xwidth
    #define yheight comp->yheight

    #define isradial comp->isradial
    ////////////////////////////////////////////////////////////////


    if (is_unset(radius)) {
        isradial = 0;
        if (all_set(3, xwidth, xmin, xmax)){
            slit_error_if(xwidth != xmax - xmin, (char*) "specifying xwidth, xmin and xmax requires consistent parameters", NAME_CURRENT_COMP);
        } else {
            slit_error_if(is_unset(xwidth) && any_unset(2, xmin, xmax), (char*) "specify either xwidth or xmin & xmax", NAME_CURRENT_COMP);
        }
        if (all_set(3, yheight, ymin, ymax)){
            slit_error_if(yheight != ymax - ymin, (char*) "specifying yheight, ymin and ymax requires consistent parameters", NAME_CURRENT_COMP);
        } else {
            slit_error_if(is_unset(yheight) && any_unset(2, ymin, ymax), (char*) "specify either yheight or ymin & ymax", NAME_CURRENT_COMP);
        }
        if (is_unset(xmin)) { // xmax also unset but xwidth *is* set
            xmax = xwidth/2;
            xmin = -xmax;
        }
        if (is_unset(ymin)) { // ymax also unset but yheight *is* set
            ymax = yheight/2;
            ymin = -ymax;
        }
        slit_warning_if(xmin == xmax || ymin == ymax, (char*) "Running with CLOSED rectangular slit - is this intentional?", NAME_CURRENT_COMP);
    }
    else {
        isradial=1;
        slit_error_if(any_set(6, xwidth, xmin, xmax, yheight, ymin, ymax), (char*) "specify radius OR width and height parameters", NAME_CURRENT_COMP);
        slit_warning_if(radius == 0., (char*) "Running with CLOSED radial slit - is this intentional?", NAME_CURRENT_COMP);
    }


    ////////////////////////////////////////////////////////////////
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef radius
    #undef xwidth
    #undef yheight

    #undef isradial

}

void Trace_Slit(Slit *comp, Neutron *particle, Instrument *instrument) {
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
    #define radius comp->radius
    #define xwidth comp->xwidth
    #define yheight comp->yheight

    #define isradial comp->isradial
    ////////////////////////////////////////////////////////////////


    PROP_Z0;
    if (!isradial ? (x < xmin || x > xmax || y < ymin || y > ymax) : (x * x + y * y > radius * radius))
        ABSORB;
    else
        SCATTER;


    ////////////////////////////////////////////////////////////////
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef radius
    #undef xwidth
    #undef yheight

    #undef isradial

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

void Save_Slit(Slit *comp) {

}

void Finally_Slit(Slit *comp) {

}

void Display_Slit(Slit *comp) {
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
    #define radius comp->radius
    #define xwidth comp->xwidth
    #define yheight comp->yheight

    #define isradial comp->isradial
    ////////////////////////////////////////////////////////////////


    if (is_unset(radius)) {
        double xw, yh;
        xw = (xmax - xmin)/2.0;
        yh = (ymax - ymin)/2.0;
        multiline(3, xmin-xw, (double)ymax, 0.0,
                (double)xmin, (double)ymax, 0.0,
                (double)xmin, ymax+yh, 0.0);
        multiline(3, xmax+xw, (double)ymax, 0.0,
                (double)xmax, (double)ymax, 0.0,
                (double)xmax, ymax+yh, 0.0);
        multiline(3, xmin-xw, (double)ymin, 0.0,
                (double)xmin, (double)ymin, 0.0,
                (double)xmin, ymin-yh, 0.0);
        multiline(3, xmax+xw, (double)ymin, 0.0,
                (double)xmax, (double)ymin, 0.0,
                (double)xmax, ymin-yh, 0.0);
    }
    else {
        circle((char*)"xy",0,0,0,radius);
    }


    ////////////////////////////////////////////////////////////////
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef radius
    #undef xwidth
    #undef yheight

    #undef isradial

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

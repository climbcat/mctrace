#ifndef __Cyl_monitor__
#define __Cyl_monitor__


// share block


struct Cyl_monitor {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters
    int nr = 20;
    char *filename = (char*) 0;
    double yheight = 10;
    double radius = 1;
    int restore_neutron = 0;
    double thmin = -180;
    double thmax = 180;
    int nowritefile = 0;

    // declares
    DArray1d PSD_N;
    DArray1d PSD_p;
    DArray1d PSD_p2;
};

Cyl_monitor Create_Cyl_monitor(s32 index, char *name) {
    Cyl_monitor _comp = {};
    Cyl_monitor *comp = &_comp;
    comp->type = (char*) "Cyl_monitor";
    comp->name = name;
    comp->index = index;

    return _comp;
}

int GetParameterCount_Cyl_monitor() {
    return 8;
}

void GetParameters_Cyl_monitor(Array<Param> *pars, Cyl_monitor *comp) {
    pars->Add( Param { CPT_INT, "nr", &comp->nr } );
    pars->Add( Param { CPT_STRING, "filename", comp->filename } );
    pars->Add( Param { CPT_FLOAT, "yheight", &comp->yheight } );
    pars->Add( Param { CPT_FLOAT, "radius", &comp->radius } );
    pars->Add( Param { CPT_INT, "restore_neutron", &comp->restore_neutron } );
    pars->Add( Param { CPT_FLOAT, "thmin", &comp->thmin } );
    pars->Add( Param { CPT_FLOAT, "thmax", &comp->thmax } );
    pars->Add( Param { CPT_INT, "nowritefile", &comp->nowritefile } );
}

void Init_Cyl_monitor(Cyl_monitor *comp, Instrument *instrument) {

    #define nr comp->nr
    #define filename comp->filename
    #define yheight comp->yheight
    #define radius comp->radius
    #define restore_neutron comp->restore_neutron
    #define thmin comp->thmin
    #define thmax comp->thmax
    #define nowritefile comp->nowritefile

    #define PSD_N comp->PSD_N
    #define PSD_p comp->PSD_p
    #define PSD_p2 comp->PSD_p2
    ////////////////////////////////////////////////////////////////


    PSD_N =  create_darr1d(nr);
    PSD_p =  create_darr1d(nr);
    PSD_p2 = create_darr1d(nr);

    // Use instance name for monitor output if no input was given
    if (!strcmp(filename,"\0")) sprintf(filename,"%s",NAME_CURRENT_COMP);


    ////////////////////////////////////////////////////////////////
    #undef nr
    #undef filename
    #undef yheight
    #undef radius
    #undef restore_neutron
    #undef thmin
    #undef thmax
    #undef nowritefile

    #undef PSD_N
    #undef PSD_p
    #undef PSD_p2

}

void Trace_Cyl_monitor(Cyl_monitor *comp, Neutron *particle, Instrument *instrument) {
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

    #define nr comp->nr
    #define filename comp->filename
    #define yheight comp->yheight
    #define radius comp->radius
    #define restore_neutron comp->restore_neutron
    #define thmin comp->thmin
    #define thmax comp->thmax
    #define nowritefile comp->nowritefile

    #define PSD_N comp->PSD_N
    #define PSD_p comp->PSD_p
    #define PSD_p2 comp->PSD_p2
    ////////////////////////////////////////////////////////////////


    int i, j;
    double t0, t1, phi;

    if (cylinder_intersect(&t0, &t1, x, y, z, vx, vy, vz, radius, yheight) == 1) {
        if (t0<0) {
            if (t1>0) {
                PROP_DT(t1);

                /* Calculate pixel */
                if (fabs(y) <= yheight/2.0) {
                    phi=atan2(x,z)*RAD2DEG;

                    if (phi >= thmin && phi <= thmax) {
                        i = floor((nr) * (phi-thmin)/(thmax-thmin));

                        double p2 = p*p;
                        PSD_N[i] = PSD_N[i]+1;
                        PSD_p[i] = PSD_p[i]+p;
                        PSD_p2[i] = PSD_p2[i]+p2;
                    }
                }
            }
        }
    }
    if (restore_neutron) {
        RESTORE_NEUTRON(INDEX_CURRENT_COMP, x, y, z, vx, vy, vz, t, sx, sy, sz, p);
    }


    ////////////////////////////////////////////////////////////////
    #undef nr
    #undef filename
    #undef yheight
    #undef radius
    #undef restore_neutron
    #undef thmin
    #undef thmax
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

void Save_Cyl_monitor(Cyl_monitor *comp) {

    #define nr comp->nr
    #define filename comp->filename
    #define yheight comp->yheight
    #define radius comp->radius
    #define restore_neutron comp->restore_neutron
    #define thmin comp->thmin
    #define thmax comp->thmax
    #define nowritefile comp->nowritefile

    #define PSD_N comp->PSD_N
    #define PSD_p comp->PSD_p
    #define PSD_p2 comp->PSD_p2
    ////////////////////////////////////////////////////////////////


    if (!nowritefile) {   
        DETECTOR_OUT_1D(
            "Cylindrical monitor",
            "radial position [deg]",
            "Intensity",
            "Theta",
            thmin, thmax, nr, 
            &PSD_N[0],&PSD_p[0],&PSD_p2[0],
            filename);
    }


    ////////////////////////////////////////////////////////////////
    #undef nr
    #undef filename
    #undef yheight
    #undef radius
    #undef restore_neutron
    #undef thmin
    #undef thmax
    #undef nowritefile

    #undef PSD_N
    #undef PSD_p
    #undef PSD_p2
}

void Finally_Cyl_monitor(Cyl_monitor *comp) {

    #define nr comp->nr
    #define filename comp->filename
    #define yheight comp->yheight
    #define radius comp->radius
    #define restore_neutron comp->restore_neutron
    #define thmin comp->thmin
    #define thmax comp->thmax
    #define nowritefile comp->nowritefile

    #define PSD_N comp->PSD_N
    #define PSD_p comp->PSD_p
    #define PSD_p2 comp->PSD_p2
    ////////////////////////////////////////////////////////////////


    destroy_darr1d(PSD_N);
    destroy_darr1d(PSD_p);
    destroy_darr1d(PSD_p2);


    ////////////////////////////////////////////////////////////////
    #undef nr
    #undef filename
    #undef yheight
    #undef radius
    #undef restore_neutron
    #undef thmin
    #undef thmax
    #undef nowritefile

    #undef PSD_N
    #undef PSD_p
    #undef PSD_p2
}

void Display_Cyl_monitor(Cyl_monitor *comp) {
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

    #define nr comp->nr
    #define filename comp->filename
    #define yheight comp->yheight
    #define radius comp->radius
    #define restore_neutron comp->restore_neutron
    #define thmin comp->thmin
    #define thmax comp->thmax
    #define nowritefile comp->nowritefile

    #define PSD_N comp->PSD_N
    #define PSD_p comp->PSD_p
    #define PSD_p2 comp->PSD_p2
    ////////////////////////////////////////////////////////////////


    circle((char*) "xz", 0, 0, 0, radius);


    ////////////////////////////////////////////////////////////////
    #undef nr
    #undef filename
    #undef yheight
    #undef radius
    #undef restore_neutron
    #undef thmin
    #undef thmax
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

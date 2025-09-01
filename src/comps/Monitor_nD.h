#ifndef __Monitor_nD__
#define __Monitor_nD__

// share block



struct Monitor_nD {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters
    char *user1 = (char*) "";
    char *user2 = (char*) "";
    char *user3 = (char*) "";
    double xwidth = 0;
    double yheight = 0;
    double zdepth = 0;
    double xmin = 0;
    double xmax = 0;
    double ymin = 0;
    double ymax = 0;
    double zmin = 0;
    double zmax = 0;
    int bins = 0;
    double min = -1e40;
    double max = 1e40;
    int restore_neutron = 0;
    double radius = 0;
    char *options = (char*) "NULL";
    char *filename = (char*) "NULL";
    char *geometry = (char*) "NULL";
    int nowritefile = 0;
    char *username1 = (char*) "NULL";
    char *username2 = (char*) "NULL";
    char *username3 = (char*) "NULL";

    // declares
    MonitornD_Defines_type DEFS;
    MonitornD_Variables_type Vars;
    MCDETECTOR detector;
    off_struct offdata;
};

Monitor_nD Create_Monitor_nD(s32 index, char *name) {
    Monitor_nD _comp = {};
    Monitor_nD *comp = &_comp;
    comp->type = (char*) "Monitor_nD";
    comp->name = name;
    comp->index = index;

    return _comp;
}

void Init_Monitor_nD(Monitor_nD *comp, Instrument *instrument) {

    #define user1 comp->user1
    #define user2 comp->user2
    #define user3 comp->user3
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define zdepth comp->zdepth
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define zmin comp->zmin
    #define zmax comp->zmax
    #define bins comp->bins
    #define min comp->min
    #define max comp->max
    #define restore_neutron comp->restore_neutron
    #define radius comp->radius
    #define options comp->options
    #define filename comp->filename
    #define geometry comp->geometry
    #define nowritefile comp->nowritefile
    #define username1 comp->username1
    #define username2 comp->username2
    #define username3 comp->username3

    #define DEFS comp->DEFS
    #define Vars comp->Vars
    #define detector comp->detector
    #define offdata comp->offdata
    ////////////////////////////////////////////////////////////////


    char tmp[CHAR_BUF_LENGTH];
    strcpy(Vars.compcurname, NAME_CURRENT_COMP);
    Vars.compcurindex=INDEX_CURRENT_COMP;
    if (options != NULL)
            strncpy(Vars.option, options, CHAR_BUF_LENGTH);
    else {
            strcpy(Vars.option, "x y");
            printf("Monitor_nD: %s has no option specified. Setting to PSD ('x y') monitor.\n", NAME_CURRENT_COMP);
    }
    Vars.compcurpos = POS_A_CURRENT_COMP;

    if (strstr(Vars.option, "source"))
            strcat(Vars.option, " list, x y z vx vy vz t sx sy sz ");

    if (bins) { sprintf(tmp, " all bins=%ld ", (long)bins); strcat(Vars.option, tmp); }
    if (min > -FLT_MAX && max < FLT_MAX) { sprintf(tmp, " all limits=[%g %g]", min, max); strcat(Vars.option, tmp); }
    else if (min > -FLT_MAX) { sprintf(tmp, " all min=%g", min); strcat(Vars.option, tmp); }
    else if (max <  FLT_MAX) { sprintf(tmp, " all max=%g", max); strcat(Vars.option, tmp); }

    /* transfer, "zero", and check username- and user variable strings to Vars struct*/
    strncpy(Vars.UserName1,
        username1 && strlen(username1) && strcmp(username1, "0") && strcmp(username1, "NULL") ?
        username1 : "", 128);
    strncpy(Vars.UserName2,
        username2 && strlen(username2) && strcmp(username2, "0") && strcmp(username2, "NULL") ?
        username2 : "", 128);
    strncpy(Vars.UserName3,
        username3 && strlen(username3) && strcmp(username3, "0") && strcmp(username3, "NULL") ?
        username3 : "", 128);
    if(user1 && strlen(user1) && strcmp(user1, "0") && strcmp(user1, "NULL"))
    {
        strncpy(Vars.UserVariable1,user1,128);
        int fail;
        Neutron testparticle;
        particle_getvar(&testparticle,Vars.UserVariable1,&fail);
        if(fail){
            fprintf(stderr,"Warning (%s): user1=%s is unknown. The signal will not be resolved - this is likely not what you intended.\n",NAME_CURRENT_COMP,user1);
        }
    }
    if(user2 && strlen(user2) && strcmp(user2, "0") && strcmp(user2, "NULL"))
    {
        strncpy(Vars.UserVariable2,user2,128);
        int fail;
        Neutron testparticle;
        particle_getvar(&testparticle,Vars.UserVariable2,&fail);
        if(fail){
            fprintf(stderr,"Warning (%s): user2=%s is unknown. The signal will not be resolved - this is likely not what you intended.\n",NAME_CURRENT_COMP,user2);
        }
    }
    if(user3 && strlen(user3) && strcmp(user3, "0") && strcmp(user3, "NULL"))
    {
        strncpy(Vars.UserVariable3,user3,128);
        int fail;
        Neutron testparticle;
        particle_getvar(&testparticle,Vars.UserVariable3,&fail);
        if(fail){
            fprintf(stderr,"Warning (%s): user3=%s is unknown. The signal will not be resolved - this is likely not what you intended.\n",NAME_CURRENT_COMP,user3);
        }
    }
    
    /*sanitize parameters set for curved shapes*/
    if(strstr(Vars.option,"cylinder") || strstr(Vars.option,"banana") || strstr(Vars.option,"sphere"))
    {
        /*this _is_ an explicit curved shape. Should have a radius. Inherit from xwidth or zdepth (diameters), x has precedence.*/
        if (!radius)
        {
            if(xwidth)
            {
                radius=xwidth/2.0;
            }
            else {
                radius=zdepth/2.0;
            }
        }
        else {
            xwidth=2*radius;
        }
        if (!yheight)
        {
            /*if not set - use the diameter as height for the curved object. This will likely only happen for spheres*/
            yheight=2*radius;
        }
    }
    else if (radius)
    {
        /*radius is set - this must be a curved shape. Infer shape from yheight, and set remaining values (xwidth etc. They are used inside monitor_nd-lib.*/
        xwidth = zdepth = 2*radius;
        if (yheight) {
            /*a height is given (and no shape explitly set - assume cylinder*/
            strcat(Vars.option, " banana");
        }
        else {
            strcat(Vars.option, " sphere");
            yheight=2*radius;
        }
    }

    int offflag = 0;
    if (geometry && strlen(geometry) && strcmp(geometry,"0") && strcmp(geometry, "NULL"))
    {
        #ifndef USE_OFF
        fprintf(stderr,"Error: You are attempting to use an OFF geometry without -DUSE_OFF. You will need to recompile with that define set!\n");
        exit(-1);

        #else
        if (!off_init(  geometry, xwidth, yheight, zdepth, 1, &offdata )) {
            printf("Monitor_nD: %s could not initiate the OFF geometry %s. \n"
                    "            Defaulting to normal Monitor dimensions.\n",
                    NAME_CURRENT_COMP, geometry);
            strcpy(geometry, "");
        }
        else {
            offflag=1;
        }
        #endif
    }

    if (!radius && !xwidth && !yheight && !zdepth && !xmin && !xmax && !ymin && !ymax &&
        !strstr(Vars.option, "previous") && (!geometry || !strlen(geometry)))
        exit(printf("Monitor_nD: %s has no dimension specified. Aborting (radius, xwidth, yheight, zdepth, previous, geometry).\n", NAME_CURRENT_COMP));

    Monitor_nD_Init(&DEFS, &Vars, xwidth, yheight, zdepth, xmin,xmax,ymin,ymax,zmin,zmax,offflag);

    if (Vars.Flag_OFF) {
        offdata.mantidflag=Vars.Flag_mantid;
        offdata.mantidoffset=Vars.Coord_Min[Vars.Coord_Number-1];
    }

    if (filename && strlen(filename) && strcmp(filename,"NULL") && strcmp(filename,"0"))
        strncpy(Vars.Mon_File, filename, 128);

    /* check if user given filename with ext will be used more than once */
    if ( ((Vars.Flag_Multiple && Vars.Coord_Number > 1) || Vars.Flag_List) && strchr(Vars.Mon_File,'.') )
    {
        char *XY; XY = strrchr(Vars.Mon_File,'.'); *XY='_';
    }

    if (restore_neutron)
        Vars.Flag_parallel=1;
    detector.m = 0;


    ////////////////////////////////////////////////////////////////
    #undef user1
    #undef user2
    #undef user3
    #undef xwidth
    #undef yheight
    #undef zdepth
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef zmin
    #undef zmax
    #undef bins
    #undef min
    #undef max
    #undef restore_neutron
    #undef radius
    #undef options
    #undef filename
    #undef geometry
    #undef nowritefile
    #undef username1
    #undef username2
    #undef username3

    #undef DEFS
    #undef Vars
    #undef detector
    #undef offdata

}

void Trace_Monitor_nD(Monitor_nD *comp, Neutron *particle, Instrument *instrument) {
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

    #define user1 comp->user1
    #define user2 comp->user2
    #define user3 comp->user3
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define zdepth comp->zdepth
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define zmin comp->zmin
    #define zmax comp->zmax
    #define bins comp->bins
    #define min comp->min
    #define max comp->max
    #define restore_neutron comp->restore_neutron
    #define radius comp->radius
    #define options comp->options
    #define filename comp->filename
    #define geometry comp->geometry
    #define nowritefile comp->nowritefile
    #define username1 comp->username1
    #define username2 comp->username2
    #define username3 comp->username3

    #define DEFS comp->DEFS
    #define Vars comp->Vars
    #define detector comp->detector
    #define offdata comp->offdata
    ////////////////////////////////////////////////////////////////


    double  transmit_he3=1.0;
    double  multiplier_capture=1.0;
    double  t0 = 0;
    double  t1 = 0;
    int     pp;
    int     intersect   = 0;
    char    Flag_Restore = 0;

    #ifdef OPENACC
    #ifdef USE_OFF
    off_struct thread_offdata = offdata;
    #endif
    #else
    #define thread_offdata offdata
    #endif
    
    /* this is done automatically
        STORE_NEUTRON(INDEX_CURRENT_COMP, x, y, z, vx, vy, vz, t, sx, sy, sz, p);
    */

    #ifdef USE_OFF
    if (geometry && strlen(geometry) && strcmp(geometry,"0") && strcmp(geometry, "NULL"))
    {
        /* determine intersections with object */
        intersect = off_intersect_all(&t0, &t1, NULL, NULL, x,y,z, vx, vy, vz, 0, 0, 0, &thread_offdata );
        if (Vars.Flag_mantid) {
            if(intersect) {
                Vars.OFF_polyidx=thread_offdata.nextintersect;
            } else {
                Vars.OFF_polyidx=-1;
            }
        }
    }
    else

    #endif
    
    if ( (abs(Vars.Flag_Shape) == DEFS.SHAPE_SQUARE)
            || (abs(Vars.Flag_Shape) == DEFS.SHAPE_DISK) ) /* square xy or disk xy */
    {
        // propagate to xy plane and find intersection
        // make sure the event is recoverable afterwards
        t0 = t;
        ALLOW_BACKPROP;
        PROP_Z0;
        if ( (t>=t0) && (z==0.0) ) // forward propagation to xy plane was successful
        {
            if (abs(Vars.Flag_Shape) == DEFS.SHAPE_SQUARE)
            {
                // square xy
                intersect = (x>=Vars.mxmin && x<=Vars.mxmax && y>=Vars.mymin && y<=Vars.mymax);
            }
            else
            {
                // disk xy
                intersect = (SQR(x) + SQR(y)) <= SQR(Vars.Sphere_Radius);
            }
        }
        else
        {
        intersect=0;
        }
    }
    else if (abs(Vars.Flag_Shape) == DEFS.SHAPE_SPHERE) /* sphere */
    {
        intersect = sphere_intersect(&t0, &t1, x, y, z, vx, vy, vz, Vars.Sphere_Radius);
    /*      intersect = (intersect && t0 > 0); */
    }
    else if ((abs(Vars.Flag_Shape) == DEFS.SHAPE_CYLIND) || (abs(Vars.Flag_Shape) == DEFS.SHAPE_BANANA)) /* cylinder */
    {
        intersect = cylinder_intersect(&t0, &t1, x, y, z, vx, vy, vz, Vars.Sphere_Radius, Vars.Cylinder_Height);
    }
    else if (abs(Vars.Flag_Shape) == DEFS.SHAPE_BOX) /* box */
    {
        intersect = box_intersect(&t0, &t1, x, y, z, vx, vy, vz,
                                fabs(Vars.mxmax-Vars.mxmin), fabs(Vars.mymax-Vars.mymin), fabs(Vars.mzmax-Vars.mzmin));
    }
    else if (abs(Vars.Flag_Shape) == DEFS.SHAPE_PREVIOUS) /* previous comp */
    {
        intersect = 1;
    }

    if (intersect)
    {
        if ((abs(Vars.Flag_Shape) == DEFS.SHAPE_SPHERE) || (abs(Vars.Flag_Shape) == DEFS.SHAPE_CYLIND)
        || (abs(Vars.Flag_Shape) == DEFS.SHAPE_BOX) || (abs(Vars.Flag_Shape) == DEFS.SHAPE_BANANA)
        || (geometry && strlen(geometry) && strcmp(geometry,"0") && strcmp(geometry, "NULL")) )
        {
            /* check if we have to remove the top/bottom with BANANA shape */
            if (abs(Vars.Flag_Shape) == DEFS.SHAPE_BANANA) {
                if (intersect == 1) { // Entered and left through sides
                    if (t0 < 0 && t1 > 0) {
                        t0 = t;  /* neutron was already inside ! */
                    }
                    if (t1 < 0 && t0 > 0) { /* neutron exit before entering !! */
                        t1 = t;
                    }
                    /* t0 is now time of incoming intersection with the detection area */
                    if ((Vars.Flag_Shape < 0) && (t1 > 0)) {
                        PROP_DT(t1); /* t1 outgoing beam */
                    } else {
                        PROP_DT(t0); /* t0 incoming beam */
                    }
                } else if (intersect == 3 || intersect == 5) { // Entered from top or bottom, left through side
                    if ((Vars.Flag_Shape < 0) && (t1 > 0)) {
                        PROP_DT(t1); /* t1 outgoing beam */
                    } else {
                        intersect=0;
                        Flag_Restore=1;
                    }
                } else if (intersect == 9 || intersect == 17) { // Entered through side, left from top or bottom
                    if ((Vars.Flag_Shape < 0) && (t1 > 0)) {
                        intersect=0;
                        Flag_Restore=1;
                    } else {
                        PROP_DT(t0); /* t0 incoming beam */
                    }
                } else if (intersect == 13 || intersect == 19) { // Went through top/bottom on entry and exit
                    intersect=0;
                    Flag_Restore=1;
                } else {
                    printf("Cylinder_intersect returned unexpected value %i\n", intersect);
                }
            } else {
                // All other shapes than the BANANA 
                if (t0 < 0 && t1 > 0)
                    t0 = t;  /* neutron was already inside ! */
                if (t1 < 0 && t0 > 0) /* neutron exit before entering !! */
                    t1 = t;
                /* t0 is now time of incoming intersection with the detection area */
                if ((Vars.Flag_Shape < 0) && (t1 > 0))
                    PROP_DT(t1); /* t1 outgoing beam */
                else
                    PROP_DT(t0); /* t0 incoming beam */
            }

            /* Final test if we are on lid / bottom of banana/sphere */
            if (abs(Vars.Flag_Shape) == DEFS.SHAPE_BANANA || abs(Vars.Flag_Shape) == DEFS.SHAPE_SPHERE) {
                if (Vars.Cylinder_Height && fabs(y) >= Vars.Cylinder_Height/2 - FLT_EPSILON) {
                    intersect=0;
                    Flag_Restore=1;
                }
            }
        }
    }

    if (intersect) {
        /* Now get the data to monitor: current or keep from PreMonitor */
    /*    if (Vars.Flag_UsePreMonitor != 1)*/
    /*    {*/
    /*      Vars.cp  = p;*/
    /*      Vars.cx  = x;*/
    /*      Vars.cvx = vx;*/
    /*      Vars.csx = sx;*/
    /*      Vars.cy  = y;*/
    /*      Vars.cvy = vy;*/
    /*      Vars.csy = sy;*/
    /*      Vars.cz  = z;*/
    /*      Vars.cvz = vz;*/
    /*      Vars.csz = sz;*/
    /*      Vars.ct  = t;*/
    /*    }*/

        if ((Vars.He3_pressure > 0) && (t1 != t0) && ((abs(Vars.Flag_Shape) == DEFS.SHAPE_SPHERE) || (abs(Vars.Flag_Shape) == DEFS.SHAPE_CYLIND) || (abs(Vars.Flag_Shape) == DEFS.SHAPE_BOX))) {
            transmit_he3 = exp(-7.417*Vars.He3_pressure*fabs(t1-t0)*2*PI*K2V);
            /* will monitor the absorbed part */
            p = p * (1-transmit_he3);
        }

        if (Vars.Flag_capture) {
            multiplier_capture = V2K*sqrt(vx*vx+vy*vy+vz*vz);
            if (multiplier_capture != 0) multiplier_capture = 2*PI/multiplier_capture; /* lambda. lambda(2200 m/2) = 1.7985 Angs  */
            p = p * multiplier_capture/1.7985;
        }

        pp = Monitor_nD_Trace(&DEFS, &Vars, _particle);
        if (pp==0.0) {
            ABSORB;
        }
        else if(pp==1) {
            SCATTER;
        }

        /*set weight to undetected part if capture and/or he3_pressure*/
        if (Vars.He3_pressure > 0) {
            /* after monitor, only remains 1-p_detect */
            p = p * transmit_he3/(1.0-transmit_he3);
        }

        if (Vars.Flag_capture){
            p = p / multiplier_capture*1.7985;
        }

        if (Vars.Flag_parallel) /* back to neutron state before detection */
            Flag_Restore = 1;
    } /* end if intersection */
    else {
        if (Vars.Flag_Absorb && !Vars.Flag_parallel)
        {
            // restore neutron ray before absorbing for correct mcdisplay
            RESTORE_NEUTRON(INDEX_CURRENT_COMP, x, y, z, vx, vy, vz, t, sx, sy, sz, p);
            ABSORB;
        }
        else
            Flag_Restore = 1;  /* no intersection, back to previous state */
    }

    if (Flag_Restore) {
        RESTORE_NEUTRON(INDEX_CURRENT_COMP, x, y, z, vx, vy, vz, t, sx, sy, sz, p);
    }


    ////////////////////////////////////////////////////////////////
    #undef user1
    #undef user2
    #undef user3
    #undef xwidth
    #undef yheight
    #undef zdepth
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef zmin
    #undef zmax
    #undef bins
    #undef min
    #undef max
    #undef restore_neutron
    #undef radius
    #undef options
    #undef filename
    #undef geometry
    #undef nowritefile
    #undef username1
    #undef username2
    #undef username3

    #undef DEFS
    #undef Vars
    #undef detector
    #undef offdata

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

void Save_Monitor_nD(Monitor_nD *comp) {

    #define user1 comp->user1
    #define user2 comp->user2
    #define user3 comp->user3
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define zdepth comp->zdepth
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define zmin comp->zmin
    #define zmax comp->zmax
    #define bins comp->bins
    #define min comp->min
    #define max comp->max
    #define restore_neutron comp->restore_neutron
    #define radius comp->radius
    #define options comp->options
    #define filename comp->filename
    #define geometry comp->geometry
    #define nowritefile comp->nowritefile
    #define username1 comp->username1
    #define username2 comp->username2
    #define username3 comp->username3

    #define DEFS comp->DEFS
    #define Vars comp->Vars
    #define detector comp->detector
    #define offdata comp->offdata
    ////////////////////////////////////////////////////////////////


    if (!nowritefile) {
        /* save results, but do not free pointers */
        detector = Monitor_nD_Save(&DEFS, &Vars);
    }


    ////////////////////////////////////////////////////////////////
    #undef user1
    #undef user2
    #undef user3
    #undef xwidth
    #undef yheight
    #undef zdepth
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef zmin
    #undef zmax
    #undef bins
    #undef min
    #undef max
    #undef restore_neutron
    #undef radius
    #undef options
    #undef filename
    #undef geometry
    #undef nowritefile
    #undef username1
    #undef username2
    #undef username3

    #undef DEFS
    #undef Vars
    #undef detector
    #undef offdata
}

void Finally_Monitor_nD(Monitor_nD *comp) {

    #define user1 comp->user1
    #define user2 comp->user2
    #define user3 comp->user3
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define zdepth comp->zdepth
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define zmin comp->zmin
    #define zmax comp->zmax
    #define bins comp->bins
    #define min comp->min
    #define max comp->max
    #define restore_neutron comp->restore_neutron
    #define radius comp->radius
    #define options comp->options
    #define filename comp->filename
    #define geometry comp->geometry
    #define nowritefile comp->nowritefile
    #define username1 comp->username1
    #define username2 comp->username2
    #define username3 comp->username3

    #define DEFS comp->DEFS
    #define Vars comp->Vars
    #define detector comp->detector
    #define offdata comp->offdata
    ////////////////////////////////////////////////////////////////


    /* free pointers */
    Monitor_nD_Finally(&DEFS, &Vars);


    ////////////////////////////////////////////////////////////////
    #undef user1
    #undef user2
    #undef user3
    #undef xwidth
    #undef yheight
    #undef zdepth
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef zmin
    #undef zmax
    #undef bins
    #undef min
    #undef max
    #undef restore_neutron
    #undef radius
    #undef options
    #undef filename
    #undef geometry
    #undef nowritefile
    #undef username1
    #undef username2
    #undef username3

    #undef DEFS
    #undef Vars
    #undef detector
    #undef offdata
}

void Display_Monitor_nD(Monitor_nD *comp) {
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

    #define user1 comp->user1
    #define user2 comp->user2
    #define user3 comp->user3
    #define xwidth comp->xwidth
    #define yheight comp->yheight
    #define zdepth comp->zdepth
    #define xmin comp->xmin
    #define xmax comp->xmax
    #define ymin comp->ymin
    #define ymax comp->ymax
    #define zmin comp->zmin
    #define zmax comp->zmax
    #define bins comp->bins
    #define min comp->min
    #define max comp->max
    #define restore_neutron comp->restore_neutron
    #define radius comp->radius
    #define options comp->options
    #define filename comp->filename
    #define geometry comp->geometry
    #define nowritefile comp->nowritefile
    #define username1 comp->username1
    #define username2 comp->username2
    #define username3 comp->username3

    #define DEFS comp->DEFS
    #define Vars comp->Vars
    #define detector comp->detector
    #define offdata comp->offdata
    ////////////////////////////////////////////////////////////////


    if (geometry && strlen(geometry) && strcmp(geometry,"0") && strcmp(geometry, "NULL")) {
        off_display(offdata);
    }
    else {
        Monitor_nD_McDisplay(&DEFS, &Vars);
    }


    ////////////////////////////////////////////////////////////////
    #undef user1
    #undef user2
    #undef user3
    #undef xwidth
    #undef yheight
    #undef zdepth
    #undef xmin
    #undef xmax
    #undef ymin
    #undef ymax
    #undef zmin
    #undef zmax
    #undef bins
    #undef min
    #undef max
    #undef restore_neutron
    #undef radius
    #undef options
    #undef filename
    #undef geometry
    #undef nowritefile
    #undef username1
    #undef username2
    #undef username3

    #undef DEFS
    #undef Vars
    #undef detector
    #undef offdata

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

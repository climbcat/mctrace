#ifndef __Source_Maxwell_3__
#define __Source_Maxwell_3__

// share block


    /* A normalised Maxwellian distribution : Integral over all l = 1 */
    #pragma acc routine seq
    double SM3_Maxwell(double l, double temp)
    {
        double a=949.0/temp;
        return 2*a*a*exp(-a/(l*l))/(l*l*l*l*l);
    }


struct Source_Maxwell_3 {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters
    double size = 0;
    double yheight = 0;
    double xwidth = 0;
    double Lmin;
    double Lmax;
    double dist;
    double focus_xw;
    double focus_yh;
    double T1;
    double T2 = 300;
    double T3 = 300;
    double I1;
    double I2 = 0;
    double I3 = 0;
    int target_index = +1;
    double lambda0 = 0;
    double dlambda = 0;

    // declares
    double l_range;
    double w_mult;
    double w_source;
    double h_source;
};

Source_Maxwell_3 Create_Source_Maxwell_3(s32 index, char *name) {
    Source_Maxwell_3 _comp = {};
    Source_Maxwell_3 *comp = &_comp;
    comp->type = (char*) "Source_Maxwell_3";
    comp->name = name;
    comp->index = index;

    return _comp;
}

void Init_Source_Maxwell_3(Source_Maxwell_3 *comp, Instrument *instrument) {

    #define size comp->size
    #define yheight comp->yheight
    #define xwidth comp->xwidth
    #define Lmin comp->Lmin
    #define Lmax comp->Lmax
    #define dist comp->dist
    #define focus_xw comp->focus_xw
    #define focus_yh comp->focus_yh
    #define T1 comp->T1
    #define T2 comp->T2
    #define T3 comp->T3
    #define I1 comp->I1
    #define I2 comp->I2
    #define I3 comp->I3
    #define target_index comp->target_index
    #define lambda0 comp->lambda0
    #define dlambda comp->dlambda

    #define l_range comp->l_range
    #define w_mult comp->w_mult
    #define w_source comp->w_source
    #define h_source comp->h_source
    ////////////////////////////////////////////////////////////////


    if (target_index && !dist)
    {
        Coords ToTarget;
        double tx,ty,tz;
        ToTarget = coords_sub(POS_A_COMP_INDEX(INDEX_CURRENT_COMP + target_index), POS_A_CURRENT_COMP);
        ToTarget = rot_apply(ROT_A_CURRENT_COMP, ToTarget);
        coords_get(ToTarget, &tx, &ty, &tz);
        dist=sqrt(tx*tx+ty*ty+tz*tz);
    }

    if (size>0) {
        w_source = h_source = size;
    }
    else {
        w_source = xwidth;
        h_source = yheight;
    }
    if (lambda0) {
        Lmin=lambda0-dlambda;
        Lmax=lambda0+dlambda;
    }
    l_range = Lmax-Lmin;
    w_mult = w_source*h_source*1.0e4;     /* source area correction */
    w_mult *= l_range;            /* wavelength range correction */
    w_mult *= 1.0/mcget_ncount();   /* correct for # neutron rays */

    if (w_source <0 || h_source < 0 || Lmin <= 0 || Lmax <= 0 || dist <= 0 || T1 <= 0 || T2 <= 0|| T3 <= 0 || Lmax<=Lmin) {
        printf("Source_Maxwell_3: %s: Error in input parameter values!\n"
            "ERROR          Exiting\n",
            NAME_CURRENT_COMP);
        exit(0);
    }


    ////////////////////////////////////////////////////////////////
    #undef size
    #undef yheight
    #undef xwidth
    #undef Lmin
    #undef Lmax
    #undef dist
    #undef focus_xw
    #undef focus_yh
    #undef T1
    #undef T2
    #undef T3
    #undef I1
    #undef I2
    #undef I3
    #undef target_index
    #undef lambda0
    #undef dlambda

    #undef l_range
    #undef w_mult
    #undef w_source
    #undef h_source

}

void Trace_Source_Maxwell_3(Source_Maxwell_3 *comp, Neutron *particle, Instrument *instrument) {
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

    #define size comp->size
    #define yheight comp->yheight
    #define xwidth comp->xwidth
    #define Lmin comp->Lmin
    #define Lmax comp->Lmax
    #define dist comp->dist
    #define focus_xw comp->focus_xw
    #define focus_yh comp->focus_yh
    #define T1 comp->T1
    #define T2 comp->T2
    #define T3 comp->T3
    #define I1 comp->I1
    #define I2 comp->I2
    #define I3 comp->I3
    #define target_index comp->target_index
    #define lambda0 comp->lambda0
    #define dlambda comp->dlambda

    #define l_range comp->l_range
    #define w_mult comp->w_mult
    #define w_source comp->w_source
    #define h_source comp->h_source
    ////////////////////////////////////////////////////////////////


    double v,tau_l,E,lambda,k,r,xf,yf,dx,dy,w_focus;
    t=0;
    z=0;
    x = 0.5*w_source*randpm1();
    y = 0.5*h_source*randpm1();         /* Choose initial position */

    randvec_target_rect_real(&xf, &yf, &r, &w_focus, 0, 0, dist, focus_xw, focus_yh, ROT_A_CURRENT_COMP, x, y, z, 2);

    dx = xf-x;
    dy = yf-y;
    r = sqrt(dx*dx+dy*dy+dist*dist);

    lambda = Lmin+l_range*rand01();    /* Choose from uniform distribution */
    k = 2*PI/lambda;
    v = K2V*k;

    vz = v*dist/r;
    vy = v*dy/r;
    vx = v*dx/r;

    /*
    printf("pos0 (%g %g %g), pos1 (%g %g %g), r: %g, v (%g %g %g), v %g\n",
    x,y,z,xf,yf,dist,r,vx,vy,vz, v);
    printf("l %g, w_focus %g \n", lambda, w_focus);
    */

    p *= w_mult*w_focus;    /* Correct for target focusing etc */
    p *= I1*SM3_Maxwell(lambda,T1)+I2*SM3_Maxwell(lambda,T2)+I3*SM3_Maxwell(lambda,T3); /* Calculate true intensity */


    ////////////////////////////////////////////////////////////////
    #undef size
    #undef yheight
    #undef xwidth
    #undef Lmin
    #undef Lmax
    #undef dist
    #undef focus_xw
    #undef focus_yh
    #undef T1
    #undef T2
    #undef T3
    #undef I1
    #undef I2
    #undef I3
    #undef target_index
    #undef lambda0
    #undef dlambda

    #undef l_range
    #undef w_mult
    #undef w_source
    #undef h_source

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

void Save_Source_Maxwell_3(Source_Maxwell_3 *comp) {

}

void Finally_Source_Maxwell_3(Source_Maxwell_3 *comp) {

}

void Display_Source_Maxwell_3(Source_Maxwell_3 *comp) {
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

    #define size comp->size
    #define yheight comp->yheight
    #define xwidth comp->xwidth
    #define Lmin comp->Lmin
    #define Lmax comp->Lmax
    #define dist comp->dist
    #define focus_xw comp->focus_xw
    #define focus_yh comp->focus_yh
    #define T1 comp->T1
    #define T2 comp->T2
    #define T3 comp->T3
    #define I1 comp->I1
    #define I2 comp->I2
    #define I3 comp->I3
    #define target_index comp->target_index
    #define lambda0 comp->lambda0
    #define dlambda comp->dlambda

    #define l_range comp->l_range
    #define w_mult comp->w_mult
    #define w_source comp->w_source
    #define h_source comp->h_source
    ////////////////////////////////////////////////////////////////


  
    multiline(5,
        -(double)focus_xw/2.0, -(double)focus_yh/2.0, 0.0,
        (double)focus_xw/2.0, -(double)focus_yh/2.0, 0.0,
        (double)focus_xw/2.0, (double)focus_yh/2.0, 0.0,
        -(double)focus_xw/2.0, (double)focus_yh/2.0, 0.0,
        -(double)focus_xw/2.0, -(double)focus_yh/2.0, 0.0);
    if (dist) {
        dashed_line(0,0,0, -focus_xw/2,-focus_yh/2,dist, 4);
        dashed_line(0,0,0, focus_xw/2,-focus_yh/2,dist, 4);
        dashed_line(0,0,0, focus_xw/2, focus_yh/2,dist, 4);
        dashed_line(0,0,0, -focus_xw/2, focus_yh/2,dist, 4);
    }


    ////////////////////////////////////////////////////////////////
    #undef size
    #undef yheight
    #undef xwidth
    #undef Lmin
    #undef Lmax
    #undef dist
    #undef focus_xw
    #undef focus_yh
    #undef T1
    #undef T2
    #undef T3
    #undef I1
    #undef I2
    #undef I3
    #undef target_index
    #undef lambda0
    #undef dlambda

    #undef l_range
    #undef w_mult
    #undef w_source
    #undef h_source

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

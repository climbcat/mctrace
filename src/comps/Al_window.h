#ifndef __Al_window__
#define __Al_window__

// share block


/* ToDo: Should be component local names. */
#ifndef AL_WINDOW
#define avogadro 6.022 /* 10E23 Atoms per mole (mol-1) */
#define Al_sigma_a .231 /* Absorption cross section per atom (barns) at 2200m/s */
#define Al_sigma_i .0082 /* Incoherent scattering cross section per atom (barns) */
#define Al_rho 2.7 /* density (gcm-3) */
#define Al_mmol 27 /* molar mass Al (gmol-1) */
#define Al_my_s (Al_rho / Al_mmol * Al_sigma_i * avogadro * 10) /* inc. XS (barn) */
#define Al_my_a_v (Al_rho / Al_mmol * Al_sigma_a * avogadro * 10 * 2200 )
/* Define Constants for Polynomial Fit of
  sigma_tot(lambda)=A+B1*X+B2*X^2+B3*X^3+B4*X^4+... */
#define Al_pf_A 1.34722
#define Al_pf_B1 .12409
#define Al_pf_B2 .01078
#define Al_pf_B3 -3.25895e-5
#define Al_pf_B4 3.74731e-6
#define AL_WINDOW
#endif


struct Al_window {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters
    double thickness = 0.001;

    // declares
};

Al_window Create_Al_window(s32 index, char *name) {
    Al_window _comp = {};
    Al_window *comp = &_comp;
    comp->type = (char*) "Al_window";
    comp->name = name;
    comp->index = index;

    return _comp;
}

void Init_Al_window(Al_window *comp, Instrument *instrument) {

}

void Trace_Al_window(Al_window *comp, Neutron *particle, Instrument *instrument) {
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

    #define thickness comp->thickness

    ////////////////////////////////////////////////////////////////


    double v;       /* Neutron velocity */
    double dt0;     /* Flight times through sample */
    double dist;
    double Al_s_tot_lambda,Al_my_tot,Al_my_a ; /* total XS (barn), total scattering length (m-1), absorption scat. length */
    double lambda; /* neutrons wavelength */

    PROP_Z0;

    dt0 = thickness/vz;
    v = sqrt(vx*vx+vy*vy+vz*vz);
    PROP_DT(dt0);
    dist = v*dt0;

    lambda = sqrt(81.81/(VS2E*v*v));
    Al_s_tot_lambda = Al_pf_A + Al_pf_B1*lambda + Al_pf_B2*lambda*lambda + Al_pf_B3*lambda*lambda*lambda;
    Al_s_tot_lambda += Al_pf_B4*lambda*lambda*lambda*lambda;
    Al_my_tot = Al_rho / Al_mmol * Al_s_tot_lambda * avogadro * 10;
    Al_my_a = Al_my_a_v/v;

    p *=exp(-Al_my_a*dist);/* neutron passes window without any interaction */

    /* TODO: scatter in Debye-Scherrer cone */


    ////////////////////////////////////////////////////////////////
    #undef thickness


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

void Save_Al_window(Al_window *comp) {

}

void Finally_Al_window(Al_window *comp) {

}

void Display_Al_window(Al_window *comp) {
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

    #define thickness comp->thickness

    ////////////////////////////////////////////////////////////////


    /* A bit ugly; hard-coded dimensions. */
    
    line(0,0,0,0.2,0,0);
    line(0,0,0,0,0.2,0);
    line(0,0,0,0,0,0.2);


    ////////////////////////////////////////////////////////////////
    #undef thickness


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

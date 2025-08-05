#ifndef __SIMCORE_H__
#define __SIMCORE_H__


#include <float.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <ctype.h>

#include <math.h>

struct NeutronSmall {
    double x,y,z; /* position [m] */
    double vx,vy,vz; /* velocity [m/s] */
    double sx,sy,sz; /* spin [0-1] */
    float flags;
};

struct Neutron {
    double x,y,z; /* position [m] */
    double vx,vy,vz; /* velocity [m/s] */
    double sx,sy,sz; /* spin [0-1] */
    int mcgravitation; /* gravity-state */
    void *mcMagnet;    /* precession-state */
    int allow_backprop; /* allow backprop */
    /* Generic Temporaries: */
    /* May be used internally by components e.g. for special */
    /* return-values from functions used in trace, thusreturned via */
    /* particle struct. (Example: Wolter Conics from McStas, silicon slabs.) */
    double _mctmp_a; /* temp a */
    double _mctmp_b; /* temp b */
    double _mctmp_c; /* temp c */
    unsigned long randstate[7];
    double t, p;     /* time, event weight */
    long long _uid;  /* Unique event ID */
    long _index;     /* component index where to send this event */
    long _absorbed;  /* flag set to TRUE when this event is to be removed/ignored */
    long _scattered; /* flag set to TRUE when this event has interacted with the last component instance */
    long _restore;   /* set to true if neutron event must be restored */
    long flag_nocoordschange;   /* set to true if particle is jumping */
};
Neutron _particle_global_randnbuse_var;
Neutron* _particle = &_particle_global_randnbuse_var;


// defines & globals


typedef double MCNUM;
typedef struct {MCNUM x, y, z;} Coords;
typedef MCNUM Rotation[3][3];

#define randstate_t uint64_t
#define MC_PATHSEP_S "/"
#define MC_PATHSEP_C '/'
#define MCCODE_STRING "tracetool_string"
#define MCCODE_NAME "tracetool_name"

int  defaultmain         = 1;
int  traceenabled        = 0;
char instrument_name[200];
char instrument_source[200];
char *instrument_exe = NULL;
int  numipar             = 0;
FILE *siminfo_file        = NULL;


static int mcncount;
static int mcrun_num;
static int mcseed;
static int mcMagnet;

// NOTE: used to get the number of components in the instrument (used in Progress_bar.comp)
static int mcNUMCOMP;


static int mcallowbackprop;
static int mcgravitation;
static int mcdotrace;
long MONND_BUFSIZ = 10000000;

double particle_getvar(Neutron *p, char *name, int *suc) {
    return 0;
}
void* particle_getvar_void(Neutron *p, char *name, int *suc) {
    return NULL;
}


struct Instrument {
    // NOTE: required by certain macros s.a. POS_A_COMP_INDEX and more. We want to get rid of these ASAP.
    char *name; // used with: NAME_INSTRUMENT
    Coords *_position_absolute; // used with: POS_A_COMP_INDEX
    int counter_N;
    int counter_P;
    int counter_P2;
};


// macros extracted from the mcstas cogen:

#ifdef DEBUG
#define DEBUG_INSTR() if(!mcdotrace); else { printf("INSTRUMENT:\n"); printf("Instrument '%s' (%s)\n", instrument_name, instrument_source); }
#define DEBUG_COMPONENT(name,c,t) if(!mcdotrace); else {\
     printf("COMPONENT: \"%s\"\n"					  \
     "POS: %g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g\n", \
     name, c.x, c.y, c.z, t[0][0], t[0][1], t[0][2], \
     t[1][0], t[1][1], t[1][2], t[2][0], t[2][1], t[2][2]); \
     printf("Component %30s AT (%g,%g,%g)\n", name, c.x, c.y, c.z); }
#define DEBUG_INSTR_END() if(!mcdotrace); else printf("INSTRUMENT END:\n");
#define DEBUG_ENTER() if(!mcdotrace); else printf("ENTER:\n");
#define DEBUG_COMP(c) if(!mcdotrace); else printf("COMP: \"%s\"\n", c);
#define DEBUG_LEAVE() if(!mcdotrace); else printf("LEAVE:\n");
#define DEBUG_ABSORB() if(!mcdotrace); else printf("ABSORB:\n");
#else
#define DEBUG_INSTR()
#define DEBUG_COMPONENT(name,c,t)
#define DEBUG_INSTR_END()
#define DEBUG_ENTER()
#define DEBUG_COMP(c)
#define DEBUG_LEAVE()
#define DEBUG_ABSORB()
#endif


#define SCATTERED (particle->_scattered)
#define RESTORE (particle->_restore)
#define RESTORE_NEUTRON(_index, ...) particle->_restore = _index;
#define ABSORB0 do { DEBUG_STATE(); DEBUG_ABSORB(); MAGNET_OFF; ABSORBED++; return; } while(0)
#define ABSORBED (particle->_absorbed)
#define ABSORB ABSORB0


//
// mccode-r 
//


/* In case of gcc / clang, ensure to use
the built-in isnan/isinf functions */
#  ifdef isnan
#    undef isnan
#  endif
#  ifdef isinf
#    undef isinf
#  endif
#  define isnan(x) __builtin_isnan(x)
#  define isinf(x) __builtin_isinf(x)


/* Useful macros ============================================================ */


/* SECTION: Dynamic Arrays */
typedef int* IArray1d;
IArray1d create_iarr1d(int n);
void destroy_iarr1d(IArray1d a);

typedef int** IArray2d;
IArray2d create_iarr2d(int nx, int ny);
void destroy_iarr2d(IArray2d a);

typedef int*** IArray3d;
IArray3d create_iarr3d(int nx, int ny, int nz);
void destroy_iarr3d(IArray3d a);

typedef double* DArray1d;
DArray1d create_darr1d(int n);
void destroy_darr1d(DArray1d a);

typedef double** DArray2d;
DArray2d create_darr2d(int nx, int ny);
void destroy_darr2d(DArray2d a);

typedef double*** DArray3d;
DArray3d create_darr3d(int nx, int ny, int nz);
void destroy_darr3d(DArray3d a);


void mcset_ncount(unsigned long long count);    /* wrapper to get mcncount */
unsigned long long int mcget_ncount(void);            /* wrapper to set mcncount */
unsigned long long mcget_run_num(void);           /* wrapper to get mcrun_num=0:mcncount-1 */


/* Useful macros and constants ============================================== */


#ifndef FLT_MAX
#define FLT_MAX         3.40282347E+38F /* max decimal value of a "float" */
#endif

#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif
#ifndef SQR
#define SQR(x) ( (x) * (x) )
#endif
#ifndef SIGN
#define SIGN(x) (((x)>0.0)?(1):(-1))
#endif


#  ifndef M_E
#    define M_E        2.71828182845904523536  // e
#  endif
#  ifndef M_LOG2E
#    define M_LOG2E    1.44269504088896340736  //  log2(e)
#  endif
#  ifndef M_LOG10E
#    define M_LOG10E   0.434294481903251827651 //  log10(e)
#  endif
#  ifndef M_LN2
#    define M_LN2      0.693147180559945309417 //  ln(2)
#  endif
#  ifndef M_LN10
#    define M_LN10     2.30258509299404568402  //  ln(10)
#  endif
#  ifndef M_PI
#    define M_PI       3.14159265358979323846  //  pi
#  endif
#  ifndef PI
#    define PI       M_PI                      //  pi - also used in some places
#  endif
#  ifndef M_PI_2
#    define M_PI_2     1.57079632679489661923  //  pi/2
#  endif
#  ifndef M_PI_4
#    define M_PI_4     0.785398163397448309616 //  pi/4
#  endif
#  ifndef M_1_PI
#    define M_1_PI     0.318309886183790671538 //  1/pi
#  endif
#  ifndef M_2_PI
#    define M_2_PI     0.636619772367581343076 //  2/pi
#  endif
#  ifndef M_2_SQRTPI
#    define M_2_SQRTPI 1.12837916709551257390  //  2/sqrt(pi)
#  endif
#  ifndef M_SQRT2
#    define M_SQRT2    1.41421356237309504880  //  sqrt(2)
#  endif
#  ifndef M_SQRT1_2
#    define M_SQRT1_2  0.707106781186547524401 //  1/sqrt(2)
#  endif


#define RAD2MIN  ((180*60)/PI)
#define MIN2RAD  (PI/(180*60))
#define DEG2RAD  (PI/180)
#define RAD2DEG  (180/PI)
#define FWHM2RMS 0.424660900144    /* Convert between full-width-half-max and */
#define RMS2FWHM 2.35482004503     /* root-mean-square (standard deviation) */
#define HBAR     1.05457168e-34    /* [Js] h bar Planck constant CODATA 2002 */
#define MNEUTRON 1.67492728e-27    /* [kg] mass of neutron CODATA 2002 */
#define GRAVITY  9.81              /* [m/s^2] gravitational acceleration */
#define NA       6.02214179e23     /* [#atoms/g .mole] Avogadro's number*/


#define UNSET nan("0x6E6F74736574")
int nans_match(double, double);
int is_unset(double);
int is_valid(double);
int is_set(double);
int all_unset(int n, ...);
int all_set(int n, ...);
int any_unset(int n, ...);
int any_set(int n, ...);


/* wrapper to get absolute and relative position of comp */
/* mccomp_posa and mccomp_posr are defined in McStas generated C code */
#define POS_A_COMP_INDEX(index) (instrument->_position_absolute[index])

// NOTE: only used so rarely (a single "vitess" component), this has been disabled
//#define POS_R_COMP_INDEX(index) (instrument->_position_relative[index])

/* setting parameters based COMP_GETPAR (returned as pointer)         */
/* compname must be given as a string, type and par are symbols.      */
#define COMP_GETPAR3(type, compname, par) &( ((_class_ ## type ##_parameters *) _getvar_parameters(compname))->par )
/* the body of this function depends on component instances, and is cogen'd */
void* _getvar_parameters(char* compname);

int _getcomp_index(char* compname);

/* Note: The two-stage approach to COMP_GETPAR is NOT redundant; without it,
* after #define C sample, COMP_GETPAR(C,x) would refer to component C, not to
* component sample. Such are the joys of ANSI C.

* Anyway the usage of COMP_GETPAR requires that we use sometimes bare names...
* NOTE: This can ONLY be used in instrument descriptions, not components.
*/
#define COMP_GETPAR2(comp, par) (_ ## comp ## _var._parameters.par)
#define COMP_GETPAR(comp, par) COMP_GETPAR2(comp,par)

#define INSTRUMENT_GETPAR(par) (_instrument_var._parameters.par)

/* Current component name, index, position and orientation */
/* These macros work because, using class-based functions, "comp" is usually
*  the local variable of the active/current component. */
#define INDEX_CURRENT_COMP (comp->index)
#define NAME_CURRENT_COMP (comp->name)
#define TYPE_CURRENT_COMP (comp->type)
#define POS_A_CURRENT_COMP (comp->position_absolute)
#define POS_R_CURRENT_COMP (comp->position_relative)
#define ROT_A_CURRENT_COMP (comp->rotation_absolute)
#define ROT_R_CURRENT_COMP (comp->rotation_relative)
#define NAME_INSTRUMENT (instrument->name)


/* send MCDISPLAY message to stdout to show gemoetry */
void mcdis_magnify(char *what);
void mcdis_line(double x1, double y1, double z1, double x2, double y2, double z2);
void mcdis_dashed_line(double x1, double y1, double z1, double x2, double y2, double z2, int n);
void mcdis_multiline(int count, ...);
void mcdis_rectangle(char* plane, double x, double y, double z, double width, double height);
void mcdis_box(double x, double y, double z, double width, double height, double length, double thickness, double nx, double ny, double nz);
void mcdis_circle(char *plane, double x, double y, double z, double r);
void mcdis_Circle(double x, double y, double z, double r, double nx, double ny, double nz);
void mcdis_cylinder( double x, double y, double z, double r, double height, double thickness, double nx, double ny, double nz);
void mcdis_cone( double x, double y, double z, double r, double height, double nx, double ny, double nz);
void mcdis_sphere(double x, double y, double z, double r);


/* random number generation. ================================================ */


#if RNG_ALG == _RNG_ALG_MT  // MT (currently not functional for GPU)
#  define MC_RAND_MAX ((uint32_t)0xffffffffUL)
#  define RANDSTATE_LEN 1
#  define srandom(seed) mt_srandom_empty()
#  define random() mt_random()
#  define _random() mt_random()
#elif RNG_ALG == _RNG_ALG_KISS  // KISS
#  ifndef UINT64_MAX
#    define UINT64_MAX ((uint64_t)0xffffffffffffffffULL)
#  endif
#  define MC_RAND_MAX UINT64_MAX
#  define RANDSTATE_LEN 7
#  define srandom(seed) kiss_srandom(_particle->randstate, seed)
#  define random() kiss_random(_particle->randstate)
#  define _random() kiss_random(state)
#endif

double _randnorm2(randstate_t* state);

// Component writer interface
#define randnorm() _randnorm2(_particle->randstate)        // NOTE: can't use _randnorm on GPU
#define rand01() _rand01(_particle->randstate)
#define randpm1() _randpm1(_particle->randstate)
#define rand0max(p1) _rand0max(p1, _particle->randstate)
#define randminmax(p1, p2) _randminmax(p1, p2, _particle->randstate)
#define randtriangle() _randtriangle(_particle->randstate)

// Mersenne Twister rng
uint32_t mt_random(void);
void mt_srandom (uint32_t x);
void mt_srandom_empty();

// KISS rng
uint64_t *kiss_srandom(uint64_t state[7], uint64_t seed);
uint64_t kiss_random(uint64_t state[7]);

// Scrambler / hash function
randstate_t _hash(randstate_t x);

// internal RNG (transforms) interface
double _rand01(randstate_t* state);
double _randpm1(randstate_t* state);
double _rand0max(double max, randstate_t* state);
double _randminmax(double min, double max, randstate_t* state);
double _randtriangle(randstate_t* state);


int init(void);
int raytrace(Neutron*);
int save(FILE *) {
    // TODO: what does this function do? grep'ing into our mccode trimmed repo didn't provide any hits for a definition
    return 1;
}
int finally(void);
int display(void);


/* simple vector algebra ==================================================== */


#define vec_prod(x, y, z, x1, y1, z1, x2, y2, z2) vec_prod_func(&x, &y, &z, x1, y1, z1, x2, y2, z2)
void vec_prod_func(double *x, double *y, double *z, double x1, double y1, double z1, double x2, double y2, double z2);
double scalar_prod( double x1, double y1, double z1, double x2, double y2, double z2);
void norm_func(double *x, double *y, double *z);
#define NORM(x,y,z)	norm_func(&x, &y, &z)
void normal_vec(double *nx, double *ny, double *nz, double x, double y, double z);


/**
 * Rotate the vector vx,vy,vz psi radians around the vector ax,ay,az
 * and put the result in x,y,z.
 */
#define rotate(x, y, z, vx, vy, vz, phi, ax, ay, az) \
    do { \
        double mcrt_tmpx = (ax), mcrt_tmpy = (ay), mcrt_tmpz = (az); \
        double mcrt_vp, mcrt_vpx, mcrt_vpy, mcrt_vpz; \
        double mcrt_vnx, mcrt_vny, mcrt_vnz, mcrt_vn1x, mcrt_vn1y, mcrt_vn1z; \
        double mcrt_bx, mcrt_by, mcrt_bz; \
        double mcrt_cos, mcrt_sin; \
        NORM(mcrt_tmpx, mcrt_tmpy, mcrt_tmpz); \
        mcrt_vp = scalar_prod((vx), (vy), (vz), mcrt_tmpx, mcrt_tmpy, mcrt_tmpz); \
        mcrt_vpx = mcrt_vp*mcrt_tmpx; \
        mcrt_vpy = mcrt_vp*mcrt_tmpy; \
        mcrt_vpz = mcrt_vp*mcrt_tmpz; \
        mcrt_vnx = (vx) - mcrt_vpx; \
        mcrt_vny = (vy) - mcrt_vpy; \
        mcrt_vnz = (vz) - mcrt_vpz; \
        vec_prod(mcrt_bx, mcrt_by, mcrt_bz, \
                mcrt_tmpx, mcrt_tmpy, mcrt_tmpz, mcrt_vnx, mcrt_vny, mcrt_vnz); \
        mcrt_cos = cos((phi)); mcrt_sin = sin((phi)); \
        mcrt_vn1x = mcrt_vnx*mcrt_cos + mcrt_bx*mcrt_sin; \
        mcrt_vn1y = mcrt_vny*mcrt_cos + mcrt_by*mcrt_sin; \
        mcrt_vn1z = mcrt_vnz*mcrt_cos + mcrt_bz*mcrt_sin; \
        (x) = mcrt_vpx + mcrt_vn1x; \
        (y) = mcrt_vpy + mcrt_vn1y; \
        (z) = mcrt_vpz + mcrt_vn1z; \
    } while(0)


/**
 * Mirror (xyz) in the plane given by the point (rx,ry,rz) and normal (nx,ny,nz)
 *
 * TODO: This define is seemingly never used...
 */
#define mirror(x,y,z,rx,ry,rz,nx,ny,nz) \
    do { \
        double mcrt_tmpx= (nx), mcrt_tmpy = (ny), mcrt_tmpz = (nz); \
        double mcrt_tmpt; \
        NORM(mcrt_tmpx, mcrt_tmpy, mcrt_tmpz); \
        mcrt_tmpt=scalar_prod((rx),(ry),(rz),mcrt_tmpx,mcrt_tmpy,mcrt_tmpz); \
        (x) = rx -2 * mcrt_tmpt*mcrt_rmpx; \
        (y) = ry -2 * mcrt_tmpt*mcrt_rmpy; \
        (z) = rz -2 * mcrt_tmpt*mcrt_rmpz; \
    } while (0)

Coords coords_set(MCNUM x, MCNUM y, MCNUM z);
Coords coords_get(Coords a, MCNUM *x, MCNUM *y, MCNUM *z);
Coords coords_add(Coords a, Coords b);
Coords coords_sub(Coords a, Coords b);
Coords coords_neg(Coords a);
Coords coords_scale(Coords b, double scale);
double coords_sp(Coords a, Coords b);
Coords coords_xp(Coords b, Coords c);
double coords_len(Coords a);
void   coords_print(Coords a);
void coords_norm(Coords* c);
void rot_set_rotation(Rotation t, double phx, double phy, double phz);
int  rot_test_identity(Rotation t);
void rot_mul(Rotation t1, Rotation t2, Rotation t3);
void rot_copy(Rotation dest, Rotation src);
void rot_transpose(Rotation src, Rotation dst);
Coords rot_apply(Rotation t, Coords a);

void mccoordschange(Coords a, Rotation t, Neutron *particle);
void mccoordschange_polarisation(Rotation t, double *sx, double *sy, double *sz);

double mcestimate_error(double N, double p1, double p2);
void mcreadparams(void);

/* this is now in mcstas-r.h and mcxtrace-r.h as the number of state parameters
is no longer equal */

Neutron mcgenstate(void);

// trajectory/shape intersection routines
int inside_rectangle(double, double, double, double);
int box_intersect(double *dt_in, double *dt_out, double x, double y, double z, double vx, double vy, double vz, double dx, double dy, double dz);
int cylinder_intersect(double *t0, double *t1, double x, double y, double z, double vx, double vy, double vz, double r, double h);
int sphere_intersect(double *t0, double *t1, double x, double y, double z, double vx, double vy, double vz, double r);

// second order equation roots
int solve_2nd_order(double *t1, double *t2, double A, double B, double C);

// random vector generation to shape
// defines silently introducing _particle as the last argument
#define randvec_target_circle(xo, yo, zo, solid_angle, xi, yi, zi, radius) \
    _randvec_target_circle(xo, yo, zo, solid_angle, xi, yi, zi, radius, _particle)
#define randvec_target_rect_angular(xo, yo, zo, solid_angle, xi, yi, zi, height, width, A) \
    _randvec_target_rect_angular(xo, yo, zo, solid_angle, xi, yi, zi, height, width, A, _particle)
#define randvec_target_rect_real(xo, yo, zo, solid_angle, xi, yi, zi, height, width, A, lx, ly, lz, order) \
    _randvec_target_rect_real(xo, yo, zo, solid_angle, xi, yi, zi, height, width, A, lx, ly, lz, order, _particle)

// defines forwarding to "inner" functions
#define randvec_target_sphere randvec_target_circle
#define randvec_target_rect(p0,p1,p2,p3,p4,p5,p6,p7,p8,p9) \ randvec_target_rect_real(p0,p1,p2,p3,p4,p5,p6,p7,p8,p9,0,0,0,1)

// headers for randvec
void _randvec_target_circle(double *xo, double *yo, double *zo, double *solid_angle, double xi, double yi, double zi, double radius, Neutron* _particle);
void _randvec_target_rect_angular(double *xo, double *yo, double *zo, double *solid_angle, double xi, double yi, double zi, double height, double width, Rotation A, Neutron* _particle);
void _randvec_target_rect_real(double *xo, double *yo, double *zo, double *solid_angle, double xi, double yi, double zi, double height, double width, Rotation A, double lx, double ly, double lz, int order, Neutron* _particle);


// this is the main()
int mccode_main(int argc, char *argv[]);


#if (USE_NEXUS == 0)
#undef USE_NEXUS
#endif

#ifndef CHAR_BUF_LENGTH
#define CHAR_BUF_LENGTH 1024
#endif


/* I/O section part ========================================================= */

/* ========================================================================== */

/*                               MCCODE_R_IO_C                                */

/* ========================================================================== */


/* main DETECTOR structure which stores most information to write to data files */
struct mcdetector_struct {
    char   filename[CHAR_BUF_LENGTH];   /* file name of monitor */
    double Position[3];                 /* position of detector component*/
    char   position[CHAR_BUF_LENGTH];   /* position of detector component (string)*/

    // jg-250616: had to rename Rotaion member for C++ port
    //Rotation Rotation;                  /* position of detector component*/
    Rotation rot;

    char   options[CHAR_BUF_LENGTH];    /* Monitor_nD style list-mode'options' (string)*/
    char   component[CHAR_BUF_LENGTH];  /* component instance name */
    char   nexuscomp[CHAR_BUF_LENGTH];  /* component naming in NeXus/HDF case */
    char   instrument[CHAR_BUF_LENGTH]; /* instrument name */
    char   type[CHAR_BUF_LENGTH];       /* data type, e.g. 0d, 1d, 2d, 3d */
    char   user[CHAR_BUF_LENGTH];       /* user name, e.g. HOME */
    char   date[CHAR_BUF_LENGTH];       /* date of simulation end/write time */
    char   title[CHAR_BUF_LENGTH];      /* title of detector */
    char   xlabel[CHAR_BUF_LENGTH];     /* X axis label */
    char   ylabel[CHAR_BUF_LENGTH];     /* Y axis label */
    char   zlabel[CHAR_BUF_LENGTH];     /* Z axis label */
    char   xvar[CHAR_BUF_LENGTH];       /* X variable name */
    char   yvar[CHAR_BUF_LENGTH];       /* Y variable name */
    char   zvar[CHAR_BUF_LENGTH];       /* Z variable name */
    char   ncount[CHAR_BUF_LENGTH];     /* number of events initially generated */
    char   limits[CHAR_BUF_LENGTH];     /* X Y Z limits, e.g. [xmin xmax ymin ymax zmin zmax] */
    char   variables[CHAR_BUF_LENGTH];  /* variables written into data block */
    char   statistics[CHAR_BUF_LENGTH]; /* center, mean and half width along axis */
    char   signal[CHAR_BUF_LENGTH];     /* min max and mean of signal (data block) */
    char   values[CHAR_BUF_LENGTH];     /* integrated values e.g. [I I_err N] */
    double xmin,xmax;                   /* min max of axes */
    double ymin,ymax;
    double zmin,zmax;
    double intensity;                   /* integrated values for data block */
    double error;
    double events;
    double min;                         /* statistics for data block */
    double max;
    double mean;
    double centerX;                     /* statistics for axes */
    double halfwidthX;
    double centerY;
    double halfwidthY;
    int    rank;                        /* dimensionaly of monitor, e.g. 0 1 2 3 */
    char   istransposed;                /* flag to transpose matrix for some formats */

    long   m,n,p;                       /* dimensions of data block and along axes */
    long   date_l;                      /* same as date, but in sec since 1970 */

    double *p0, *p1, *p2;               /* pointers to saved data, NULL when freed */
    char   format[CHAR_BUF_LENGTH];    /* format for file generation */
};

typedef struct mcdetector_struct MCDETECTOR;

static   char *dirname             = NULL;      /* name of output directory */
static   const char *siminfo_name  = "mccode";  /* default output sim file name */
char    *mcformat                  = NULL;      /* NULL (default) or a specific format */



/* I/O function prototypes ================================================== */


/* output functions */
MCDETECTOR mcdetector_out_0D(char *t, double p0, double p1, double p2, char *c, Coords pos, Rotation rot, int index) {
    // jg-250617 TODO: implement / re-introduce

    MCDETECTOR result = {};
    return result;
}
MCDETECTOR mcdetector_out_0D(const char *t, double p0, double p1, double p2, char *c, Coords pos, Rotation rot, int index) {
    // jg-250618: const version

    MCDETECTOR result = {};
    return result;
}

MCDETECTOR mcdetector_out_1D(char *t, char *xl, char *yl,
            char *xvar, double x1, double x2, long n,
            double *p0, double *p1, double *p2, char *f, char *c, Coords pos, Rotation rot, int index)
{
    // jg-250617 TODO: implement / re-introduce

    MCDETECTOR result = {};
    return result;
}
MCDETECTOR mcdetector_out_1D(const char *t, const char *xl, const char *yl,
            const char *xvar, double x1, double x2, long n,
            double *p0, double *p1, double *p2, char *f, char *c, Coords pos, Rotation rot, int index)
{
    // jg-250618: const version

    MCDETECTOR result = {};
    return result;
}

MCDETECTOR mcdetector_out_2D(char *t, char *xl, char *yl,
            double x1, double x2, double y1, double y2, long m,
            long n, double *p0, double *p1, double *p2, char *f,
            char *c, Coords pos, Rotation rot, int index)
{
    // jg-250617 TODO: implement / re-introduce

    MCDETECTOR result = {};
    return result;
}
MCDETECTOR mcdetector_out_2D(const char *t, const char *xl, const char *yl,
            double x1, double x2, double y1, double y2, long m,
            long n, double *p0, double *p1, double *p2, char *f,
            char *c, Coords pos, Rotation rot, int index)
{
    // jg-250618: const version

    MCDETECTOR result = {};
    return result;
}

MCDETECTOR mcdetector_out_2D_list(char *t, char *xl, char *yl,
                  double x1, double x2, double y1, double y2,
                  long m, long n,
                  double *p0, double *p1, double *p2, char *f,
		  char *c, Coords posa, Rotation rota, char* options, int index)
{
    // jg-250617 TODO: implement / re-introduce

    MCDETECTOR result;
    return result;
}
MCDETECTOR mcdetector_out_2D_list(const char *t, const char *xl, const char *yl,
                  double x1, double x2, double y1, double y2,
                  long m, long n,
                  double *p0, double *p1, double *p2, char *f,
		  char *c, Coords posa, Rotation rota, char* options, int index)
{
    // jg-250618: const version

    MCDETECTOR result;
    return result;
}

MCDETECTOR mcdetector_out_list(char *t, char *xl, char *yl,
            long m, long n,
            double *p1, char *f,
            char *c, Coords posa, Rotation rot,char* options, int index)
{
    // jg-250617 TODO: implement / re-introduce

    MCDETECTOR result;
    return result;
}
MCDETECTOR mcdetector_out_list(const char *t, const char *xl, const char *yl,
            long m, long n,
            double *p1, char *f,
            char *c, Coords posa, Rotation rot,char* options, int index)
{
    // jg-250618: const version

    MCDETECTOR result;
    return result;
}


/* wrappers to output functions, that automatically set NAME and POSITION */
#define DETECTOR_OUT(p0,p1,p2) mcdetector_out_0D(NAME_CURRENT_COMP,p0,p1,p2,NAME_CURRENT_COMP,POS_A_CURRENT_COMP,ROT_A_CURRENT_COMP,INDEX_CURRENT_COMP)
#define DETECTOR_OUT_0D(t,p0,p1,p2) mcdetector_out_0D(t,p0,p1,p2,NAME_CURRENT_COMP,POS_A_CURRENT_COMP,ROT_A_CURRENT_COMP,INDEX_CURRENT_COMP)
#define DETECTOR_OUT_1D(t,xl,yl,xvar,x1,x2,n,p0,p1,p2,f) \
    mcdetector_out_1D(t,xl,yl,xvar,x1,x2,n,p0,p1,p2,f,NAME_CURRENT_COMP,POS_A_CURRENT_COMP,ROT_A_CURRENT_COMP,INDEX_CURRENT_COMP)
#define DETECTOR_OUT_2D(t,xl,yl,x1,x2,y1,y2,m,n,p0,p1,p2,f) \
    mcdetector_out_2D(t,xl,yl,x1,x2,y1,y2,m,n,p0,p1,p2,f,NAME_CURRENT_COMP,POS_A_CURRENT_COMP,ROT_A_CURRENT_COMP,INDEX_CURRENT_COMP)


//
//  Implementation


/* SECTION: Predefine (component) parameters ================================= */


int nans_match(double a, double b){
    return (*(uint64_t*)&a == *(uint64_t*)&b);
}
int is_unset(double x){
    return nans_match(x, UNSET);
}
int is_set(double x){
    return !nans_match(x, UNSET);
}
int is_valid(double x){
    return !isnan(x)||is_unset(x);
}
int all_unset(int n, ...){
    va_list ptr;
    va_start(ptr, n);
    int ret=1;
    for (int i=0; i<n; ++i) if(is_set(va_arg(ptr, double))) ret=0;
    va_end(ptr);
    return ret;
}
int all_set(int n, ...){
    va_list ptr;
    va_start(ptr, n);
    int ret=1;
    for (int i=0; i<n; ++i) if(is_unset(va_arg(ptr, double))) ret=0;
    va_end(ptr);
    return ret;
}
int any_unset(int n, ...){
    va_list ptr;
    va_start(ptr, n);
    int ret=0;
    for (int i=0; i<n; ++i) if(is_unset(va_arg(ptr, double))) ret=1;
    va_end(ptr);
    return ret;
}
int any_set(int n, ...){
    va_list ptr;
    va_start(ptr, n);
    int ret=0;
    for (int i=0; i<n; ++i) if(is_set(va_arg(ptr, double))) ret=1;
    va_end(ptr);
    return ret;
}


/* SECTION: Dynamic Arrays ================================================== */
IArray1d create_iarr1d(int n){
    IArray1d arr2d;
    arr2d = (IArray1d) calloc(n, sizeof(int));
    return arr2d;
}
void destroy_iarr1d(IArray1d a){
    free(a);
}

IArray2d create_iarr2d(int nx, int ny){
    IArray2d arr2d;
    arr2d = (IArray2d) calloc(nx, sizeof(int *));

    int *p1;
    p1 = (int*) calloc(nx*ny, sizeof(int));

    int i;
    for (i=0; i<nx; i++){
        arr2d[i] = &(p1[i*ny]);
    }
    return arr2d;
    }
    void destroy_iarr2d(IArray2d a){
    free(a[0]);
    free(a);
}

IArray3d create_iarr3d(int nx, int ny, int nz){
    IArray3d arr3d;
    int i, j;

    // 1d
    arr3d = (IArray3d) calloc(nx, sizeof(int **));

    // d2
    int **p1;
    p1 = (int**) calloc(nx*ny, sizeof(int *));

    for (i=0; i<nx; i++){
        arr3d[i] = &(p1[i*ny]);
    }

    // 3d
    int *p2;
    p2 = (int*) calloc(nx*ny*nz, sizeof(int));
    for (i=0; i<nx; i++){
        for (j=0; j<ny; j++){
        arr3d[i][j] = &(p2[(i*ny+j)*nz]);
        }
    }
    return arr3d;
}

void destroy_iarr3d(IArray3d a){
    free(a[0][0]);
    free(a[0]);
    free(a);
}

DArray1d create_darr1d(int n){
    DArray1d arr2d;
    arr2d = (DArray1d) calloc(n, sizeof(double));
    return arr2d;
}

void destroy_darr1d(DArray1d a){
    free(a);
}

DArray2d create_darr2d(int nx, int ny){
    DArray2d arr2d;
    arr2d = (DArray2d) calloc(nx, sizeof(double *));

    double *p1;
    p1 = (double*) calloc(nx*ny, sizeof(double));

    int i;
    for (i=0; i<nx; i++){
        arr2d[i] = &(p1[i*ny]);
    }
    return arr2d;
}

void destroy_darr2d(DArray2d a){
    free(a[0]);
    free(a);
}

DArray3d create_darr3d(int nx, int ny, int nz){
    DArray3d arr3d;

    int i, j;

    // 1d
    arr3d = (DArray3d) calloc(nx, sizeof(double **));

    // d2
    double **p1;
    p1 = (double**) calloc(nx*ny, sizeof(double *));

    for (i=0; i<nx; i++){
        arr3d[i] = &(p1[i*ny]);
    }

    // 3d
    double *p2;
    p2 = (double*) calloc(nx*ny*nz, sizeof(double));
    for (i=0; i<nx; i++){
        for (j=0; j<ny; j++){
        arr3d[i][j] = &(p2[(i*ny+j)*nz]);
        }
    }
    return arr3d;
}

void destroy_darr3d(DArray3d a){
    free(a[0][0]);
    free(a[0]);
    free(a);
}


/*******************************************************************************
* mcset_ncount: set total number of rays to generate
*******************************************************************************/
void mcset_ncount(unsigned long long int count)
{
    mcncount = count;
}

/* mcget_ncount: get total number of rays to generate */
unsigned long long int mcget_ncount(void)
{
    return mcncount;
}

/* mcget_run_num: get curent number of rays */
/* Within the TRACE scope we are now using _particle->uid directly */
unsigned long long int mcget_run_num() // shuld be (Neutron* _particle) somehow
{
    /* This function only remains for the few cases outside TRACE where we need to know
        the number of simulated particles */
    return mcrun_num;
}

/* mcsetn_arg: get ncount from a string argument */
static void mcsetn_arg(char *arg)
{
    mcset_ncount((long long int) strtod(arg, NULL));
}

/* mcsetseed: set the random generator seed from a string argument */
static void mcsetseed(char *arg)
{
    mcseed = atol(arg);
    if(!mcseed) {
        srandom(mcseed);
    }
    else {
        fprintf(stderr, "Error: seed must not be zero (mcsetseed)\n");
        exit(1);
    }
}


/* SECTION: MCDISPLAY support. =============================================== */


/*******************************************************************************
* Just output MCDISPLAY keywords to be caught by an external plotter client.
*******************************************************************************/


void mcdis_magnify(char *what){
    // Do nothing here, better use interactive zoom from the tools
}

void mcdis_line(double x1, double y1, double z1, double x2, double y2, double z2){
    printf("MCDISPLAY: multiline(2,%g,%g,%g,%g,%g,%g)\n", x1,y1,z1,x2,y2,z2);
}

void mcdis_dashed_line(double x1, double y1, double z1, double x2, double y2, double z2, int n){
    int i;
    const double dx = (x2-x1)/(2*n+1);
    const double dy = (y2-y1)/(2*n+1);
    const double dz = (z2-z1)/(2*n+1);

    for(i = 0; i < n+1; i++)
        mcdis_line(x1 + 2*i*dx, y1 + 2*i*dy, z1 + 2*i*dz, x1 + (2*i+1)*dx, y1 + (2*i+1)*dy, z1 + (2*i+1)*dz);
    }

    void mcdis_multiline(int count, ...){
    va_list ap;
    double x,y,z;

    printf("MCDISPLAY: multiline(%d", count);
    va_start(ap, count);
    while(count--) {
        x = va_arg(ap, double);
        y = va_arg(ap, double);
        z = va_arg(ap, double);
        printf(",%g,%g,%g", x, y, z);
    }
    va_end(ap);
    printf(")\n");
}

void mcdis_rectangle(char* plane, double x, double y, double z, double width, double height){
    /* draws a rectangle in the plane           */
    /* x is ALWAYS width and y is ALWAYS height */
    if (strcmp("xy", plane)==0) {
        mcdis_multiline(5,
            x - width/2, y - height/2, z,
            x + width/2, y - height/2, z,
            x + width/2, y + height/2, z,
            x - width/2, y + height/2, z,
            x - width/2, y - height/2, z);
    }
    else if (strcmp("xz", plane)==0) {
        mcdis_multiline(5,
            x - width/2, y, z - height/2,
            x + width/2, y, z - height/2,
            x + width/2, y, z + height/2,
            x - width/2, y, z + height/2,
            x - width/2, y, z - height/2);
    }
    else if (strcmp("yz", plane)==0) {
        mcdis_multiline(5,
            x, y - height/2, z - width/2,
            x, y - height/2, z + width/2,
            x, y + height/2, z + width/2,
            x, y + height/2, z - width/2,
            x, y - height/2, z - width/2);
    }
    else {

        fprintf(stderr, "Error: Definition of plane %s unknown\n", plane);
        exit(1);
    }
}

void mcdis_circle(char *plane, double x, double y, double z, double r){
    printf("MCDISPLAY: circle('%s',%g,%g,%g,%g)\n", plane, x, y, z, r);
}

void mcdis_new_circle(double x, double y, double z, double r, double nx, double ny, double nz){
    printf("MCDISPLAY: new_circle(%g,%g,%g,%g,%g,%g,%g)\n", x, y, z, r, nx, ny, nz);
}


/* Draws a circle with center (x,y,z), radius (r), and in the plane
* with normal (nx,ny,nz)*/
void mcdis_Circle(double x, double y, double z, double r, double nx, double ny, double nz){
    int i;
    if (nx==0 && ny && nz==0) {
        for (i=0;i<24; i++) {
            mcdis_line(x+r*sin(i*2*PI/24),y,z+r*cos(i*2*PI/24), x+r*sin((i+1)*2*PI/24),y,z+r*cos((i+1)*2*PI/24));
        }
    }
    else{
        double mx,my,mz;
        /*generate perpendicular vector using (nx,ny,nz) and (0,1,0)*/
        vec_prod(mx,my,mz, 0,1,0, nx,ny,nz);
        NORM(mx,my,mz);
        /*draw circle*/
        for (i=0; i<24; i++) {
            double ux,uy,uz;
            double wx,wy,wz;
            rotate(ux,uy,uz, mx,my,mz, i*2*PI/24, nx,ny,nz);
            rotate(wx,wy,wz, mx,my,mz, (i+1)*2*PI/24, nx,ny,nz);
            mcdis_line(x+ux*r,y+uy*r,z+uz*r, x+wx*r,y+wy*r,z+wz*r);
        }
    }
}


/*  OLD IMPLEMENTATION
    draws a box with center at (x, y, z) and
    width (deltax), height (deltay), length (deltaz) */
void mcdis_legacy_box(double x, double y, double z, double width, double height, double length) {
    mcdis_rectangle((char*) "xy", x, y, z-length/2, width, height);
    mcdis_rectangle((char*) "xy", x, y, z+length/2, width, height);
    mcdis_line(x-width/2, y-height/2, z-length/2, x-width/2, y-height/2, z+length/2);
    mcdis_line(x-width/2, y+height/2, z-length/2, x-width/2, y+height/2, z+length/2);
    mcdis_line(x+width/2, y-height/2, z-length/2, x+width/2, y-height/2, z+length/2);
    mcdis_line(x+width/2, y+height/2, z-length/2, x+width/2, y+height/2, z+length/2);
}

/*  NEW 3D IMPLEMENTATION OF BOX SUPPORTS HOLLOW ALSO
    draws a box with center at (x, y, z) and
    width (deltax), height (deltay), length (deltaz) */
void mcdis_box(double x, double y, double z, double width, double height, double length, double thickness, double nx, double ny, double nz){
    mcdis_legacy_box(x, y, z, width, height, length);
    if (thickness)
        mcdis_legacy_box(x, y, z, width-thickness, height-thickness, length);
}


/* OLD IMPLEMENTATION
Draws a cylinder with center at (x,y,z) with extent (r,height).
* The cylinder axis is along the vector nx,ny,nz. */
void mcdis_legacy_cylinder( double x, double y, double z, double r, double height, int N, double nx, double ny, double nz){
    int i;
    /*no lines make little sense - so trigger the default*/
    if(N<=0) N=5;

    NORM(nx,ny,nz);
    double h_2=height/2.0;
    mcdis_Circle(x+nx*h_2,y+ny*h_2,z+nz*h_2,r,nx,ny,nz);
    mcdis_Circle(x-nx*h_2,y-ny*h_2,z-nz*h_2,r,nx,ny,nz);

    double mx,my,mz;
    /*generate perpendicular vector using (nx,ny,nz) and (0,1,0)*/
    if(nx==0 && ny && nz==0){
        mx=my=0;mz=1;
    }
    else {
        vec_prod(mx,my,mz, 0,1,0, nx,ny,nz);
        NORM(mx,my,mz);
    }
    /*draw circle*/
    for (i=0; i<24; i++){
        double ux,uy,uz;
        rotate(ux,uy,uz, mx,my,mz, i*2*PI/24, nx,ny,nz);
        mcdis_line(x+nx*h_2+ux*r, y+ny*h_2+uy*r, z+nz*h_2+uz*r, x-nx*h_2+ux*r, y-ny*h_2+uy*r, z-nz*h_2+uz*r);
    }
}

/* NEW 3D IMPLEMENTATION ALSO SUPPORTING HOLLOW
Draws a cylinder with center at (x,y,z) with extent (r,height).
* The cylinder axis is along the vector nx,ny,nz.*/
void mcdis_cylinder( double x, double y, double z, double r, double height, double thickness, double nx, double ny, double nz){
    mcdis_legacy_cylinder(x, y, z, r, height, 12, nx, ny, nz);
}

/* Draws a cone with center at (x,y,z) with extent (r,height).
* The cone axis is along the vector nx,ny,nz.*/
void mcdis_cone( double x, double y, double z, double r, double height, double nx, double ny, double nz)
{
    mcdis_Circle(x, y, z, r, nx, ny, nz);
    mcdis_Circle(x+0.25*height*nx, y+0.25*height*ny, z+0.25*height*nz, 0.75*r, nx, ny, nz);
    mcdis_Circle(x+0.5*height*nx, y+0.5*height*ny, z+0.5*height*nz, 0.5*r, nx, ny, nz);
    mcdis_Circle(x+0.75*height*nx, y+0.75*height*ny, z+0.75*height*nz, 0.25*r, nx, ny, nz);
    mcdis_line(x, y, z, x+height*nx, y+height*ny, z+height*nz);
}

/* Draws a disc with center at (x,y,z) with extent (r).
* The disc axis is along the vector nx,ny,nz.*/
void mcdis_disc( double x, double y, double z, double r, double nx, double ny, double nz){
    printf("MCDISPLAY: disc(%g, %g, %g, %g, %g, %g, %g)\n", x, y, z, r, nx, ny, nz);
}

/* Draws a annulus with center at (x,y,z) with extent (outer_radius) and remove inner_radius.
* The annulus axis is along the vector nx,ny,nz.*/
void mcdis_annulus( double x, double y, double z, double outer_radius, double inner_radius, double nx, double ny, double nz){
    printf("MCDISPLAY: annulus(%g, %g, %g, %g, %g, %g, %g, %g)\n", x, y, z, outer_radius, inner_radius, nx, ny, nz);
}

/* draws a sphere with center at (x,y,z) with extent (r)*/
void mcdis_sphere(double x, double y, double z, double r){
    double nx,ny,nz;
    int i;
    int N=12;

    nx=0;ny=0;nz=1;
    mcdis_Circle(x,y,z,r,nx,ny,nz);
    for (i=1;i<N;i++){
        rotate(nx,ny,nz, nx,ny,nz, PI/N, 0,1,0);
        mcdis_Circle(x,y,z,r,nx,ny,nz);
    }
    /*lastly draw a great circle perpendicular to all N circles*/
    //mcdis_Circle(x,y,z,radius,1,0,0);

    for (i=1;i<=N;i++){
        double yy=-r+ 2*r*((double)i/(N+1));
        mcdis_Circle(x,y+yy ,z,  sqrt(r*r-yy*yy) ,0,1,0);
    }
}
/* POLYHEDRON IMPLEMENTATION*/

void mcdis_polyhedron(char *vertices_faces){
    printf("MCDISPLAY: polyhedron %s\n", vertices_faces);
}

/* POLYGON IMPLEMENTATION */
void mcdis_polygon(int count, ...){
    va_list ap;
    double *x,*y,*z;

    double x0=0, y0=0, z0=0; /* Used for centre-of-mass in trace==2 */

    x = (double*) malloc(count*sizeof(double));
    y = (double*) malloc(count*sizeof(double));
    z = (double*) malloc(count*sizeof(double));

    va_start(ap, count);

    int j;
    for (j=0; j<count; j++) {
        x[j] = va_arg(ap, double);
        y[j] = va_arg(ap, double);
        z[j] = va_arg(ap, double);
        // Calculation of polygon centre of mass
        x0 += x[j]; y0 += y[j]; z0 += z[j];
    }
    va_end(ap);

    /* Patch data for multiline(count+1, ... use 0th point*/

    x0 /= count; y0 /= count; z0 /= count;
    /* Build up a json string for a "polyhedron" */
    // Estimate size of the JSON string
    const int VERTEX_OVERHEAD = 30;
    const int FACE_OVERHEAD_BASE = 20;
    const int FACE_INDEX_OVERHEAD = 15;
    int estimated_size = 256; // Base size
    estimated_size += count * VERTEX_OVERHEAD;

    int faceSize;
    int vtxSize;
    if (count > 3) {
        /* Split in triangles - as many as polygon rank */
        faceSize=count;
        vtxSize=count+1;
    } else {
        faceSize=1;
        vtxSize=count;
    }
    
    for (int i = 0; i < faceSize;) {
        int num_indices = 3;
        estimated_size += FACE_OVERHEAD_BASE + num_indices * FACE_INDEX_OVERHEAD;
        i += num_indices + 1;
    }

    char *json_string = (char*) malloc(estimated_size);
    if (json_string == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }

    char *ptr = json_string;
    ptr += sprintf(ptr, "{ \"vertices\": [");

    if (count==3) { // Single, basic triangle
        ptr += sprintf(ptr, "[%g, %g, %g], [%g, %g, %g], [%g, %g, %g]", x[0], y[0], z[0], x[1], y[1], z[1], x[2], y[2], z[2]);
    }
    else {
        for (int i = 0; i < vtxSize-1; i++) {
            ptr += sprintf(ptr, "[%g, %g, %g]", x[i], y[i], z[i]);
            if (i < vtxSize - 2) {
                ptr += sprintf(ptr, ", ");
            } else {
                ptr += sprintf(ptr, ", [%g, %g, %g]", x0, y0, z0);
            }
        }
    }
    ptr += sprintf(ptr, "], \"faces\": [");

    if (count==3) { // Single, basic triangle, 1 face...
        ptr += sprintf(ptr, "{ \"face\": [");
        ptr += sprintf(ptr, "0, 1, 2");
        ptr += sprintf(ptr, "]}");
    } else {
        for (int i = 0; i < faceSize; i++) {
            int num = 3;
            ptr += sprintf(ptr, "{ \"face\": [");
            if (i < faceSize - 1) {
                ptr += sprintf(ptr, "%d, %d, %d",i,i+1,count);
            } else {
                ptr += sprintf(ptr, "%d, %d, %d",i,count,0);
            }
            ptr += sprintf(ptr, "]}");
            if (i < faceSize-1) {
                ptr += sprintf(ptr, ", ");
            }
        }
    }
    ptr += sprintf(ptr, "]}");
    mcdis_polyhedron(json_string);

    free(json_string);

    free(x);free(y);free(z);
}
/* END NEW POLYGON IMPLEMENTATION*/

/*
void mcdis_polygon(double x1, double y1, double z1,
                double x2, double y2, double z2){
printf("MCDISPLAY: polygon(2,%g,%g,%g,%g,%g,%g)\n",
        x1,y1,z1,x2,y2,z2);
}
*/

/* SECTION: coordinates handling ============================================ */

/*******************************************************************************
* Since we use a lot of geometric calculations using Cartesian coordinates,
* we collect some useful routines here. However, it is also permissible to
* work directly on the underlying struct coords whenever that is most
* convenient (that is, the type Coords is not abstract).
*
* Coordinates are also used to store rotation angles around x/y/z axis.
*
* Since coordinates are used much like a basic type (such as double), the
* structure itself is passed and returned, rather than a pointer.
*
* At compile-time, the values of the coordinates may be unknown (for example
* a motor position). Hence coordinates are general expressions and not simple
* numbers. For this we used the type Coords_exp which has three CExp
* fields. For runtime (or calculations possible at compile time), we use
* Coords which contains three double fields.
*******************************************************************************/

/* coords_set: Assign coordinates. */
Coords coords_set(MCNUM x, MCNUM y, MCNUM z)
{
    Coords a;

    a.x = x;
    a.y = y;
    a.z = z;
    return a;
}

/* coords_get: get coordinates. Required when 'x','y','z' are #defined as ray pars */
Coords coords_get(Coords a, MCNUM *x, MCNUM *y, MCNUM *z)
{
    *x = a.x;
    *y = a.y;
    *z = a.z;
    return a;
}

/* coords_add: Add two coordinates. */
Coords coords_add(Coords a, Coords b)
{
    Coords c;

    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    if (fabs(c.z) < 1e-14) c.z=0.0;
    return c;
}

/* coords_sub: Subtract two coordinates. */
Coords coords_sub(Coords a, Coords b)
{
    Coords c;

    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    if (fabs(c.z) < 1e-14) c.z=0.0;
    return c;
}

/* coords_neg: Negate coordinates. */
Coords coords_neg(Coords a)
{
    Coords b;

    b.x = -a.x;
    b.y = -a.y;
    b.z = -a.z;
    return b;
}

/* coords_scale: Scale a vector. */
Coords coords_scale(Coords b, double scale) {
    Coords a;

    a.x = b.x*scale;
    a.y = b.y*scale;
    a.z = b.z*scale;
    return a;
}

/* coords_sp: Scalar product: a . b */
double coords_sp(Coords a, Coords b) {
    double value;

    value = a.x*b.x + a.y*b.y + a.z*b.z;
    return value;
}

/* coords_xp: Cross product: a = b x c. */
Coords coords_xp(Coords b, Coords c) {
    Coords a;

    a.x = b.y*c.z - c.y*b.z;
    a.y = b.z*c.x - c.z*b.x;
    a.z = b.x*c.y - c.x*b.y;
    return a;
}

/* coords_len: Gives length of coords set. */
double coords_len(Coords a) {
    return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

/* coords_mirror: Mirror a in plane (through the origin) defined by normal n*/
Coords coords_mirror(Coords a, Coords n) {
    double t = scalar_prod(n.x, n.y, n.z, n.x, n.y, n.z);
    Coords b;
    if (t!=1) {
        t = sqrt(t);
        n.x /= t;
        n.y /= t;
        n.z /= t;
    }
    t=scalar_prod(a.x, a.y, a.z, n.x, n.y, n.z);
    b.x = a.x-2*t*n.x;
    b.y = a.y-2*t*n.y;
    b.z = a.z-2*t*n.z;
    return b;
}

/* coords_print: Print out vector values. */
void coords_print(Coords a) {
    #ifndef OPENACC
    fprintf(stdout, "(%f, %f, %f)\n", a.x, a.y, a.z);
    #endif
    return;
}

void coords_norm(Coords* c) {
    double temp = coords_sp(*c,*c);

    // Skip if we will end dividing by zero
    if (temp == 0) return;

    temp = sqrt(temp);

    c->x /= temp;
    c->y /= temp;
    c->z /= temp;
}

/* coords_test_zero: check if zero vector*/
int coords_test_zero(Coords a){
    return ( a.x==0 && a.y==0 && a.z==0 );
}

/*******************************************************************************
* The Rotation type implements a rotation transformation of a coordinate
* system in the form of a double[3][3] matrix.
*
* Contrary to the Coords type in coords.c, rotations are passed by
* reference. Functions that yield new rotations do so by writing to an
* explicit result parameter; rotations are not returned from functions. The
* reason for this is that arrays cannot by returned from functions (though
* structures can; thus an alternative would have been to wrap the
* double[3][3] array up in a struct). Such are the ways of C programming.
*
* A rotation represents the tranformation of the coordinates of a vector when
* changing between coordinate systems that are rotated with respect to each
* other. For example, suppose that coordinate system Q is rotated 45 degrees
* around the Z axis with respect to coordinate system P. Let T be the
* rotation transformation representing a 45 degree rotation around Z. Then to
* get the coordinates of a vector r in system Q, apply T to the coordinates
* of r in P. If r=(1,0,0) in P, it will be (sqrt(1/2),-sqrt(1/2),0) in
* Q. Thus we should be careful when interpreting the sign of rotation angles:
* they represent the rotation of the coordinate systems, not of the
* coordinates (which has opposite sign).
*******************************************************************************/

/*******************************************************************************
* rot_set_rotation: Get transformation for rotation first phx around x axis,
* then phy around y, then phz around z.
*******************************************************************************/
void rot_set_rotation(Rotation t, double phx, double phy, double phz)
{
    if ((phx == 0) && (phy == 0) && (phz == 0)) {
        t[0][0] = 1.0;
        t[0][1] = 0.0;
        t[0][2] = 0.0;
        t[1][0] = 0.0;
        t[1][1] = 1.0;
        t[1][2] = 0.0;
        t[2][0] = 0.0;
        t[2][1] = 0.0;
        t[2][2] = 1.0;
    } else {
        double cx = cos(phx);
        double sx = sin(phx);
        double cy = cos(phy);
        double sy = sin(phy);
        double cz = cos(phz);
        double sz = sin(phz);

        t[0][0] = cy*cz;
        t[0][1] = sx*sy*cz + cx*sz;
        t[0][2] = sx*sz - cx*sy*cz;
        t[1][0] = -cy*sz;
        t[1][1] = cx*cz - sx*sy*sz;
        t[1][2] = sx*cz + cx*sy*sz;
        t[2][0] = sy;
        t[2][1] = -sx*cy;
        t[2][2] = cx*cy;
    }
}

/*******************************************************************************
* rot_test_identity: Test if rotation is identity
*******************************************************************************/
int rot_test_identity(Rotation t)
{
    return (t[0][0] + t[1][1] + t[2][2] == 3);
}

/*******************************************************************************
* rot_mul: Matrix multiplication of transformations (this corresponds to
* combining transformations). After rot_mul(T1, T2, T3), doing T3 is
* equal to doing first T2, then T1.
* Note that T3 must not alias (use the same array as) T1 or T2.
*******************************************************************************/
void rot_mul(Rotation t1, Rotation t2, Rotation t3)
    {
    if (rot_test_identity(t1)) {
        rot_copy(t3, t2);
    } else if (rot_test_identity(t2)) {
        rot_copy(t3, t1);
    } else {
        int i,j;
        for(i = 0; i < 3; i++)
        for(j = 0; j < 3; j++)
    t3[i][j] = t1[i][0]*t2[0][j] + t1[i][1]*t2[1][j] + t1[i][2]*t2[2][j];
    }
}

/*******************************************************************************
* rot_copy: Copy a rotation transformation (arrays cannot be assigned in C).
*******************************************************************************/
void rot_copy(Rotation dest, Rotation src)
{
    int i,j;
    for(i = 0; i < 3; i++)
        for(j = 0; j < 3; j++)
            dest[i][j] = src[i][j];
}

/*******************************************************************************
* rot_transpose: Matrix transposition, which is inversion for Rotation matrices
*******************************************************************************/
void rot_transpose(Rotation src, Rotation dst)
{
    dst[0][0] = src[0][0];
    dst[0][1] = src[1][0];
    dst[0][2] = src[2][0];
    dst[1][0] = src[0][1];
    dst[1][1] = src[1][1];
    dst[1][2] = src[2][1];
    dst[2][0] = src[0][2];
    dst[2][1] = src[1][2];
    dst[2][2] = src[2][2];
}

/*******************************************************************************
* rot_apply: returns t*a
*******************************************************************************/
Coords rot_apply(Rotation t, Coords a)
{
    Coords b;
    if (rot_test_identity(t)) {
        return a;
    } else {
        b.x = t[0][0]*a.x + t[0][1]*a.y + t[0][2]*a.z;
        b.y = t[1][0]*a.x + t[1][1]*a.y + t[1][2]*a.z;
        b.z = t[2][0]*a.x + t[2][1]*a.y + t[2][2]*a.z;
        return b;
    }
}

/**
    * Pretty-printing of rotation matrices.
    */
void rot_print(Rotation rot) {
    printf("[ %4.2f %4.2f %4.2f ]\n",
        rot[0][0], rot[0][1], rot[0][2]);
    printf("[ %4.2f %4.2f %4.2f ]\n",
        rot[1][0], rot[1][1], rot[1][2]);
    printf("[ %4.2f %4.2f %4.2f ]\n\n",
        rot[2][0], rot[2][1], rot[2][2]);
}

/**
    * Vector product: used by vec_prod (mccode-r.h). Use coords_xp for Coords.
    */
void vec_prod_func(double *x, double *y, double *z, double x1, double y1, double z1, double x2, double y2, double z2) {
    *x = (y1)*(z2) - (y2)*(z1);
    *y = (z1)*(x2) - (z2)*(x1);
    *z = (x1)*(y2) - (x2)*(y1);
}

/**
    * Scalar product: use coords_sp for Coords.
    */
double scalar_prod( double x1, double y1, double z1, double x2, double y2, double z2) {
    return ((x1 * x2) + (y1 * y2) + (z1 * z2));
}

void norm_func(double *x, double *y, double *z) {
    double temp = (*x * *x) + (*y * *y) + (*z * *z);
    if (temp != 0) {
        temp = sqrt(temp);
        *x /= temp;
        *y /= temp;
        *z /= temp;
    }
}


/*
*  Fallback serial version of the one above.
*/
long sort_absorb_last_serial(Neutron* particles, long len) {
    long i = 0;
    long j = len - 1;
    Neutron pbuffer;

    // bubble
    while (i < j) {
        while (!particles[i]._absorbed && i<j) i++;
        while (particles[j]._absorbed && i<j) j--;
        if (i < j) {
        pbuffer = particles[j];
        particles[j] = particles[i];
        particles[i] = pbuffer;
        i++;
        j--;
        }
    }

    // return new length
    if (i==j && !particles[i]._absorbed)
        return i + 1;
    else
        return i;
}

/*******************************************************************************
* mccoordschange: applies rotation to (x y z) and (vx vy vz) and Spin (sx,sy,sz)
*******************************************************************************/
void mccoordschange(Coords a, Rotation t, Neutron *particle)
{
    Coords b, c;

    b.x = particle->x;
    b.y = particle->y;
    b.z = particle->z;
    c = rot_apply(t, b);
    b = coords_add(c, a);
    particle->x = b.x;
    particle->y = b.y;
    particle->z = b.z;

#if MCCODE_PARTICLE_CODE == 2112
    if (particle->vz != 0.0 || particle->vx != 0.0 || particle->vy != 0.0)
    mccoordschange_polarisation(t, &(particle->vx), &(particle->vy), &(particle->vz));

    if (particle->sz != 0.0 || particle->sx != 0.0 || particle->sy != 0.0)
    mccoordschange_polarisation(t, &(particle->sx), &(particle->sy), &(particle->sz));
#elif MCCODE_PARTICLE_CODE == 22
    if (particle->kz != 0.0 || particle->kx != 0.0 || particle->ky != 0.0)
    mccoordschange_polarisation(t, &(particle->kx), &(particle->ky), &(particle->kz));

    if (particle->Ez != 0.0 || particle->Ex != 0.0 || particle->Ey != 0.0)
    mccoordschange_polarisation(t, &(particle->Ex), &(particle->Ey), &(particle->Ez));
#endif
}

/*******************************************************************************
* mccoordschange_polarisation: applies rotation to vector (sx sy sz)
*******************************************************************************/
void mccoordschange_polarisation(Rotation t, double *sx, double *sy, double *sz)
{
    Coords b, c;

    b.x = *sx;
    b.y = *sy;
    b.z = *sz;
    c = rot_apply(t, b);
    *sx = c.x;
    *sy = c.y;
    *sz = c.z;
}

/* SECTION: vector math  ==================================================== */

/* normal_vec_func: Compute normal vector to (x,y,z). */
void normal_vec(double *nx, double *ny, double *nz,
                double x, double y, double z)
{
    double ax = fabs(x);
    double ay = fabs(y);
    double az = fabs(z);
    double l;
    if(x == 0 && y == 0 && z == 0)
    {
        *nx = 0;
        *ny = 0;
        *nz = 0;
        return;
    }
    if(ax < ay)
    {
        if(ax < az)
        {                           /* Use X axis */
        l = sqrt(z*z + y*y);
        *nx = 0;
        *ny = z/l;
        *nz = -y/l;
        return;
        }
    }
    else
    {
        if(ay < az)
        {                           /* Use Y axis */
        l = sqrt(z*z + x*x);
        *nx = z/l;
        *ny = 0;
        *nz = -x/l;
        return;
        }
    }
    /* Use Z axis */
    l = sqrt(y*y + x*x);
    *nx = y/l;
    *ny = -x/l;
    *nz = 0;
} /* normal_vec */

/*******************************************************************************
    * solve_2nd_order: second order equation solve: A*t^2 + B*t + C = 0
    * solve_2nd_order(&t1, NULL, A,B,C)
    *   returns 0 if no solution was found, or set 't1' to the smallest positive
    *   solution.
    * solve_2nd_order(&t1, &t2, A,B,C)
    *   same as with &t2=NULL, but also returns the second solution.
    * EXAMPLE usage for intersection of a trajectory with a plane in gravitation
    * field (gx,gy,gz):
    * The neutron starts at point r=(x,y,z) with velocityv=(vx vy vz). The plane
    * has a normal vector n=(nx,ny,nz) and contains the point W=(wx,wy,wz).
    * The problem consists in solving the 2nd order equation:
    *      1/2.n.g.t^2 + n.v.t + n.(r-W) = 0
    * so that A = 0.5 n.g; B = n.v; C = n.(r-W);
    * Without acceleration, t=-n.(r-W)/n.v
    ******************************************************************************/
int solve_2nd_order_old(double *t1, double *t2,
                double A,  double B,  double C)
{
    int ret=0;

    if (!t1) return 0;
    *t1 = 0;
    if (t2) *t2=0;

    if (fabs(A) < 1E-10) /* approximate to linear equation: A ~ 0 */
    {
        if (B) {  *t1 = -C/B; ret=1; if (t2) *t2=*t1; }
        /* else no intersection: A=B=0 ret=0 */
    }
    else
    {
        double D;
        D = B*B - 4*A*C;
        if (D >= 0) /* Delta > 0: two solutions */
        {
        double sD, dt1, dt2;
        sD = sqrt(D);
        dt1 = (-B + sD)/2/A;
        dt2 = (-B - sD)/2/A;
        /* we identify very small values with zero */
        if (fabs(dt1) < 1e-10) dt1=0.0;
        if (fabs(dt2) < 1e-10) dt2=0.0;

        /* now we choose the smallest positive solution */
        if      (dt1<=0.0 && dt2>0.0) ret=2; /* dt2 positive */
        else if (dt2<=0.0 && dt1>0.0) ret=1; /* dt1 positive */
        else if (dt1> 0.0 && dt2>0.0)
        {  if (dt1 < dt2) ret=1; else ret=2; } /* all positive: min(dt1,dt2) */
        /* else two solutions are negative. ret=-1 */
        if (ret==1) { *t1 = dt1;  if (t2) *t2=dt2; }
        else        { *t1 = dt2;  if (t2) *t2=dt1; }
        ret=2;  /* found 2 solutions and t1 is the positive one */
        } /* else Delta <0: no intersection. ret=0 */
    }
    return(ret);
} /* solve_2nd_order */

int solve_2nd_order(double *t0, double *t1, double A, double B, double C){
    int retval=0;
    double sign=copysign(1.0,B);
    double dt0,dt1;

    dt0=0;
    dt1=0;
    if(t1){ *t1=0;}

    /*protect against rounding errors by locally equating DBL_EPSILON with 0*/
    if (fabs(A)<DBL_EPSILON){
        A=0;
    }
    if (fabs(B)<DBL_EPSILON){
        B=0;
    }
    if (fabs(C)<DBL_EPSILON){
        C=0;
    }

    /*check if coefficient are sane*/
    if( A==0  && B==0){
        retval=0;
    }else{
        if(A==0){
        /*equation is linear*/
        dt0=-C/B;
        retval=1;
        }else if (C==0){
        /*one root is 0*/
        if(sign<0){
            dt0=0;dt1=-B/A;
        }else{
            dt0=-B/A;dt1=0;
        }
        retval=2;
        }else{
        /*a regular 2nd order eq. Also works out fine for B==0.*/
        double D;
        D=B*B-4*A*C;
        if (D>=0){
            dt0=(-B - sign*sqrt(B*B-4*A*C))/(2*A);
            dt1=C/(A*dt0);
            retval=2;
        }else{
            /*no real roots*/
            retval=0;
        }
        }
        /*sort the solutions*/
        if (retval==1){
        /*put both solutions in t0 and t1*/
        *t0=dt0;
        if(t1) *t1=dt1;
        }else{
        /*we have two solutions*/
        /*swap if both are positive and t1 smaller than t0 or t1 the only positive*/
        int swap=0;
        if(dt1>0 && ( dt1<dt0 || dt0<=0) ){
            swap=1;
        }
        if (swap){
            *t0=dt1;
            if(t1) *t1=dt0;
        }else{
            *t0=dt0;
            if(t1) *t1=dt0;
        }
        }

    }
    return retval;
} /*solve_2nd_order_improved*/


/*******************************************************************************
    * randvec_target_circle: Choose random direction towards target at (x,y,z)
    * with given radius.
    * If radius is zero, choose random direction in full 4PI, no target.
    ******************************************************************************/
void _randvec_target_circle(double *xo, double *yo, double *zo, double *solid_angle,
        double xi, double yi, double zi, double radius,
        Neutron* _particle)
{
    double l2, phi, theta, nx, ny, nz, xt, yt, zt, xu, yu, zu;

    if(radius == 0.0)
    {
        /* No target, choose uniformly a direction in full 4PI solid angle. */
        theta = acos(1 - rand0max(2));
        phi = rand0max(2 * PI);
        if(solid_angle)
        *solid_angle = 4*PI;
        nx = 1;
        ny = 0;
        nz = 0;
        yi = sqrt(xi*xi+yi*yi+zi*zi);
        zi = 0;
        xi = 0;
    }
    else
    {
        double costheta0;
        l2 = xi*xi + yi*yi + zi*zi; /* sqr Distance to target. */
        costheta0 = sqrt(l2/(radius*radius+l2));
        if (radius < 0) costheta0 *= -1;
        if(solid_angle)
        {
        /* Compute solid angle of target as seen from origin. */
            *solid_angle = 2*PI*(1 - costheta0);
        }

        /* Now choose point uniformly on circle surface within angle theta0 */
        theta = acos (1 - rand0max(1 - costheta0)); /* radius on circle */
        phi = rand0max(2 * PI); /* rotation on circle at given radius */
        /* Now, to obtain the desired vector rotate (xi,yi,zi) angle theta around a
        perpendicular axis u=i x n and then angle phi around i. */
        if(xi == 0 && zi == 0)
        {
        nx = 1;
        ny = 0;
        nz = 0;
        }
        else
        {
        nx = -zi;
        nz = xi;
        ny = 0;
        }
    }

    /* [xyz]u = [xyz]i x n[xyz] (usually vertical) */
    vec_prod(xu,  yu,  zu, xi, yi, zi,        nx, ny, nz);
    /* [xyz]t = [xyz]i rotated theta around [xyz]u */
    rotate  (xt,  yt,  zt, xi, yi, zi, theta, xu, yu, zu);
    /* [xyz]o = [xyz]t rotated phi around n[xyz] */
    rotate (*xo, *yo, *zo, xt, yt, zt, phi, xi, yi, zi);
}
/* randvec_target_circle */

/*******************************************************************************
    * randvec_target_rect_angular: Choose random direction towards target at
    * (xi,yi,zi) with given ANGULAR dimension height x width. height=phi_x=[0,PI],
    * width=phi_y=[0,2*PI] (radians)
    * If height or width is zero, choose random direction in full 4PI, no target.
    *******************************************************************************/
void _randvec_target_rect_angular(double *xo, double *yo, double *zo, double *solid_angle,
        double xi, double yi, double zi, double width, double height, Rotation A,
        Neutron* _particle)
{
    double theta, phi, nx, ny, nz, xt, yt, zt, xu, yu, zu;
    Coords tmp;
    Rotation Ainverse;

    rot_transpose(A, Ainverse);

    if(height == 0.0 || width == 0.0)
    {
        randvec_target_circle(xo, yo, zo, solid_angle, xi, yi, zi, 0);
        return;
    }
    else
    {
        if(solid_angle)
        {
        /* Compute solid angle of target as seen from origin. */
        *solid_angle = 2*fabs(width*sin(height/2));
        }

        /* Go to global coordinate system */

        tmp = coords_set(xi, yi, zi);
        tmp = rot_apply(Ainverse, tmp);
        coords_get(tmp, &xi, &yi, &zi);

        /* Now choose point uniformly on the unit sphere segment with angle theta/phi */
        phi   = width*randpm1()/2.0;
        theta = asin(randpm1()*sin(height/2.0));
        /* Now, to obtain the desired vector rotate (xi,yi,zi) angle theta around
        n, and then phi around u. */
        if(xi == 0 && zi == 0)
        {
        nx = 1;
        ny = 0;
        nz = 0;
        }
        else
        {
        nx = -zi;
        nz = xi;
        ny = 0;
        }
    }

    /* [xyz]u = [xyz]i x n[xyz] (usually vertical) */
    vec_prod(xu,  yu,  zu, xi, yi, zi,        nx, ny, nz);
    /* [xyz]t = [xyz]i rotated theta around [xyz]u */
    rotate  (xt,  yt,  zt, xi, yi, zi, theta, nx, ny, nz);
    /* [xyz]o = [xyz]t rotated phi around n[xyz] */
    rotate (*xo, *yo, *zo, xt, yt, zt, phi, xu,  yu,  zu);

    /* Go back to local coordinate system */
    tmp = coords_set(*xo, *yo, *zo);
    tmp = rot_apply(A, tmp);
    coords_get(tmp, &*xo, &*yo, &*zo);
}
/* randvec_target_rect_angular */

/*******************************************************************************
    * randvec_target_rect_real: Choose random direction towards target at (xi,yi,zi)
    * with given dimension height x width (in meters !).
    *
    * Local emission coordinate is taken into account and corrected for 'order' times.
    * (See remarks posted to mcstas-users by George Apostolopoulus <gapost@ipta.demokritos.gr>)
    *
    * If height or width is zero, choose random direction in full 4PI, no target.
    *
    * Traditionally, this routine had the name randvec_target_rect - this is now a
    * a define (see mcstas-r.h) pointing here. If you use the old rouine, you are NOT
    * taking the local emmission coordinate into account.
*******************************************************************************/
void _randvec_target_rect_real(double *xo, double *yo, double *zo, double *solid_angle,
        double xi, double yi, double zi,
        double width, double height, Rotation A,
        double lx, double ly, double lz, int order,
        Neutron* _particle)
{
    double dx, dy, dist, dist_p, nx, ny, nz, mx, my, mz, n_norm, m_norm;
    double cos_theta;
    Coords tmp;
    Rotation Ainverse;

    rot_transpose(A, Ainverse);

    if(height == 0.0 || width == 0.0)
    {
        randvec_target_circle(xo, yo, zo, solid_angle,
                xi, yi, zi, 0);
        return;
    }
    else
    {
        /* Now choose point uniformly on rectangle within width x height */
        dx = width*randpm1()/2.0;
        dy = height*randpm1()/2.0;

        /* Determine distance to target plane*/
        dist = sqrt(xi*xi + yi*yi + zi*zi);
        /* Go to global coordinate system */

        tmp = coords_set(xi, yi, zi);
        tmp = rot_apply(Ainverse, tmp);
        coords_get(tmp, &xi, &yi, &zi);

        /* Determine vector normal to trajectory axis (z) and gravity [0 1 0] */
        vec_prod(nx, ny, nz, xi, yi, zi, 0, 1, 0);

        /* This now defines the x-axis, normalize: */
        n_norm=sqrt(nx*nx + ny*ny + nz*nz);
        nx = nx/n_norm;
        ny = ny/n_norm;
        nz = nz/n_norm;

        /* Now, determine our y-axis (vertical in many cases...) */
        vec_prod(mx, my, mz, xi, yi, zi, nx, ny, nz);
        m_norm=sqrt(mx*mx + my*my + mz*mz);
        mx = mx/m_norm;
        my = my/m_norm;
        mz = mz/m_norm;

        /* Our output, random vector can now be defined by linear combination: */

        *xo = xi + dx * nx + dy * mx;
        *yo = yi + dx * ny + dy * my;
        *zo = zi + dx * nz + dy * mz;

        /* Go back to local coordinate system */
        tmp = coords_set(*xo, *yo, *zo);
        tmp = rot_apply(A, tmp);
        coords_get(tmp, &*xo, &*yo, &*zo);

        /* Go back to local coordinate system */
        tmp = coords_set(xi, yi, zi);
        tmp = rot_apply(A, tmp);
        coords_get(tmp, &xi, &yi, &zi);

        if (solid_angle) {
        /* Calculate vector from local point to remote random point */
        lx = *xo - lx;
        ly = *yo - ly;
        lz = *zo - lz;
        dist_p = sqrt(lx*lx + ly*ly + lz*lz);

        /* Adjust the 'solid angle' */
        /* 1/r^2 to the chosen point times cos(\theta) between the normal */
        /* vector of the target rectangle and direction vector of the chosen point. */
        cos_theta = (xi * lx + yi * ly + zi * lz) / (dist * dist_p);
        *solid_angle = width * height / (dist_p * dist_p);
        int counter;
        for (counter = 0; counter < order; counter++) {
            *solid_angle = *solid_angle * cos_theta;
        }
        }
    }
}


/* Period parameters */
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfU   /* constant vector a */
#define UPPER_MASK 0x80000000U /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffU /* least significant r bits */

static uint32_t mt[N]; /* the array for the state vector  */
static int mti = N + 1; /* mti==N+1 means mt[N] is not initialized */

// Required for compatibility with common RNG interface (e.g., kiss/mt polymorphism)
void mt_srandom_empty(void) {}

// Initializes mt[N] with a seed
void mt_srandom(uint32_t seed) {
    mt[0] = seed;
    for (mti = 1; mti < N; mti++) {
        mt[mti] = 1812433253U * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti;
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffU;
        /* for >32 bit machines */
    }
}
/* Initialize by an array with array-length.
Init_key is the array for initializing keys.
key_length is its length. */
void init_by_array(uint32_t init_key[], size_t key_length) {
    size_t i = 1, j = 0, k;
    mt_srandom(19650218U);
    k = (N > key_length ? N : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525U))
            + init_key[j] + (uint32_t)j;
        mt[i] &= 0xffffffffU;
        i++; j++;
        if (i >= N) { mt[0] = mt[N - 1]; i = 1; }
        if (j >= key_length) j = 0;
    }
    for (k = N - 1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941U))
            - (uint32_t)i;
        mt[i] &= 0xffffffffU;
        i++;
        if (i >= N) { mt[0] = mt[N - 1]; i = 1; }
    }
    mt[0] = 0x80000000U; /* MSB is 1; ensuring non-zero initial array */
}

// Generates a random number on [0, 0xffffffff]-interval
uint32_t mt_random(void) {
    uint32_t y;
    static const uint32_t mag01[2] = { 0x0U, MATRIX_A };
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N + 1)   /* if mt_srandom() has not been called, */ 
            mt_srandom(5489U);  /* a default initial seed is used */

        for (kk = 0; kk < N - M; kk++) {
            y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
            mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1U];
        }
        for (; kk < N - 1; kk++) {
            y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
            mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1U];
        }
        y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
        mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1U];

        mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680U;
    y ^= (y << 15) & 0xefc60000U;
    y ^= (y >> 18);

    return y;
}
#undef N
#undef M
#undef MATRIX_A
#undef UPPER_MASK
#undef LOWER_MASK
/* End of "Mersenne Twister". */


/*
KISS

From: http://www.helsbreth.org/random/rng_kiss.html
Scott Nelson 1999

Based on Marsaglia's KISS or (KISS+SWB) <http://www.cs.yorku.ca/~oz/marsaglia-
rng.html>

KISS - Keep it Simple Stupid PRNG

the idea is to use simple, fast, individually promising
generators to get a composite that will be fast, easy to code
have a very long period and pass all the tests put to it.
The three components of KISS are
        x(n)=a*x(n-1)+1 mod 2^32
        y(n)=y(n-1)(I+L^13)(I+R^17)(I+L^5),
        z(n)=2*z(n-1)+z(n-2) +carry mod 2^32
The y's are a shift register sequence on 32bit binary vectors
period 2^32-1;
The z's are a simple multiply-with-carry sequence with period
2^63+2^32-1.  The period of KISS is thus
    2^32*(2^32-1)*(2^63+2^32-1) > 2^127

In 2025 adapted for consistent 64-bit behavior across platforms.
*/

/* the KISS state is stored as a vector of 7 uint64_t        */
/*   0  1  2  3  4      5  6   */
/* [ x, y, z, w, carry, k, m ] */

uint64_t *kiss_srandom(uint64_t state[7], uint64_t seed) {
    if (seed == 0) seed = 1ull;
    state[0] = seed | 1ull; // x
    state[1] = seed | 2ull; // y
    state[2] = seed | 4ull; // z
    state[3] = seed | 8ull; // w
    state[4] = 0ull;        // carry
    state[5] = 0ull;        // k
    state[6] = 0ull;        // m
    return state;
}

uint64_t kiss_random(uint64_t state[7]) {
    // Linear congruential generator
    state[0] = state[0] * 69069ull + 1ull;

    // Xorshift
    state[1] ^= state[1] << 13ull;
    state[1] ^= state[1] >> 17ull;
    state[1] ^= state[1] << 5ull;

    // Multiply-with-carry
    state[5] = (state[2] >> 2ull) + (state[3] >> 3ull) + (state[4] >> 2ull);
    state[6] = state[3] + state[3] + state[2] + state[4];
    state[2] = state[3];
    state[3] = state[6];
    state[4] = state[5] >> 62ull;  // Top bit of carry (adjusted for 64-bit)

    return state[0] + state[1] + state[3];
}
/* end of "KISS" rng */


/* FAST KISS in another implementation (Hundt) */

//////////////////////////////////////////////////////////////////////////////
// fast keep it simple stupid generator
//////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Thomas Mueller hash for initialization of rngs
// http://stackoverflow.com/questions/664014/
//        what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
//////////////////////////////////////////////////////////////////////////////
randstate_t _hash(randstate_t x) {
    x = ((x >> 16) ^ x) * (randstate_t)0x45d9f3b;
    x = ((x >> 16) ^ x) * (randstate_t)0x45d9f3b;
    x = ((x >> 16) ^ x);
    return x;
}


// SECTION: random number transforms ==========================================



// generate a random number from normal law
double _randnorm(randstate_t* state)
{
    static double v1, v2, s; /* removing static breaks comparison with McStas <= 2.5 */
    static int phase = 0;
    double X, u1, u2;

    if(phase == 0)
    {
        do
        {
        u1 = _rand01(state);
        u2 = _rand01(state);
        v1 = 2*u1 - 1;
        v2 = 2*u2 - 1;
        s = v1*v1 + v2*v2;
        } while(s >= 1 || s == 0);

        X = v1*sqrt(-2*log(s)/s);
    }
    else
    {
        X = v2*sqrt(-2*log(s)/s);
    }

    phase = 1 - phase;
    return X;
}
// another one
double _randnorm2(randstate_t* state) {
    double x, y, r;
    do {
        x = 2.0 * _rand01(state) - 1.0;
        y = 2.0 * _rand01(state) - 1.0;
        r = x*x + y*y;
    } while (r == 0.0 || r >= 1.0);
    return x * sqrt((-2.0 * log(r)) / r);
}

// Generate a random number from -1 to 1 with triangle distribution
double _randtriangle(randstate_t* state) {
    double randnum = _rand01(state);
    if (randnum>0.5) return(1-sqrt(2*(randnum-0.5)));
    else return(sqrt(2*randnum)-1);
}
double _rand01(randstate_t* state) {
    double randnum;
    randnum = (double) _random();
    // TODO: can we mult instead of div?
    randnum /= (double) MC_RAND_MAX + 1;
    return randnum;
}
// Return a random number between 1 and -1
double _randpm1(randstate_t* state) {
    double randnum;
    randnum = (double) _random();
    randnum /= ((double) MC_RAND_MAX + 1) / 2;
    randnum -= 1;
    return randnum;
}
// Return a random number between 0 and max.
double _rand0max(double max, randstate_t* state) {
    double randnum;
    randnum = (double) _random();
    randnum /= ((double) MC_RAND_MAX + 1) / max;
    return randnum;
}
// Return a random number between min and max.
double _randminmax(double min, double max, randstate_t* state) {
    return _rand0max(max - min, state) + max;
}


// mcstas-r


#define AA2MS    629.622368        /* Convert k[1/AA] to v[m/s] */
#define MS2AA    1.58825361e-3     /* Convert v[m/s] to k[1/AA] */
#define K2V      AA2MS
#define V2K      MS2AA
#define Q2V      AA2MS
#define V2Q      MS2AA
#define SE2V     437.393377        /* Convert sqrt(E)[meV] to v[m/s] */
#define VS2E     5.22703725e-6     /* Convert (v[m/s])**2 to E[meV] */

#define SCATTER0 do { DEBUG_SCATTER(); SCATTERED++; } while(0)
#define SCATTER SCATTER0


#define MAGNET_ON \
do { \
    mcMagnet = 1; \
} while(0)

#define MAGNET_OFF \
do { \
    mcMagnet = 0; \
} while(0)

#define ALLOW_BACKPROP \
do { \
    mcallowbackprop = 1; \
} while(0)

#define DISALLOW_BACKPROP \
do { \
    mcallowbackprop = 0; \
} while(0)

#define PROP_MAGNET(dt) \
do { \
} while (0)
    /* change coordinates from local system to magnet system */
/*    Rotation rotLM, rotTemp; \
    Coords   posLM = coords_sub(POS_A_CURRENT_COMP, mcMagnetPos); \
    rot_transpose(ROT_A_CURRENT_COMP, rotTemp); \
    rot_mul(rotTemp, mcMagnetRot, rotLM); \
    mcMagnetPrecession(x, y, z, t, vx, vy, vz, \
            &sx, &sy, &sz, dt, posLM, rotLM); \
    } while(0)
*/

#define mcPROP_DT(dt) \
do { \
    if (mcMagnet && dt > 0) PROP_MAGNET(dt);\
    x += vx*(dt); \
    y += vy*(dt); \
    z += vz*(dt); \
    t += (dt); \
    if (isnan(p) || isinf(p)) { ABSORB; }\
} while(0)

/* ADD: E. Farhi, Aug 6th, 2001 PROP_GRAV_DT propagation with acceleration */
#define PROP_GRAV_DT(dt, Ax, Ay, Az) \
do { \
    if(dt < 0 && mcallowbackprop == 0) { ABSORB; }\
    if (mcMagnet) /*printf("Spin precession gravity\n")*/; \
    x  += vx*(dt) + (Ax)*(dt)*(dt)/2; \
    y  += vy*(dt) + (Ay)*(dt)*(dt)/2; \
    z  += vz*(dt) + (Az)*(dt)*(dt)/2; \
    vx += (Ax)*(dt); \
    vy += (Ay)*(dt); \
    vz += (Az)*(dt); \
    t  += (dt); \
    DISALLOW_BACKPROP;\
} while(0)


#define PROP_DT(dt) \
do { \
    if(dt < 0 && mcallowbackprop == 0) { RESTORE=1; ABSORB; }; \
    if (mcgravitation) { Coords mcLocG; double mc_gx, mc_gy, mc_gz; \
    mcLocG = rot_apply(ROT_A_CURRENT_COMP, coords_set(0,-GRAVITY,0)); \
    coords_get(mcLocG, &mc_gx, &mc_gy, &mc_gz); \
    PROP_GRAV_DT(dt, mc_gx, mc_gy, mc_gz); } \
    else mcPROP_DT(dt); \
    DISALLOW_BACKPROP;\
} while(0)


#define PROP_Z0 \
do { \
    if (mcgravitation) { Coords mcLocG; int mc_ret; \
    double mc_dt, mc_gx, mc_gy, mc_gz; \
    mcLocG = rot_apply(ROT_A_CURRENT_COMP, coords_set(0,-GRAVITY,0)); \
    coords_get(mcLocG, &mc_gx, &mc_gy, &mc_gz); \
    mc_ret = solve_2nd_order(&mc_dt, NULL, -mc_gz/2, -vz, -z); \
    if (mc_ret) {PROP_GRAV_DT(mc_dt, mc_gx, mc_gy, mc_gz); z=0;}\
    else if (mcallowbackprop == 0 && mc_dt < 0) { ABSORB; }; } \
    else mcPROP_Z0; \
    DISALLOW_BACKPROP;\
} while(0)

#define mcPROP_Z0 \
do { \
    double mc_dt; \
    if(vz == 0) { ABSORB; }; \
    mc_dt = -z/vz; \
    if(mc_dt < 0 && mcallowbackprop == 0) { ABSORB; }; \
    mcPROP_DT(mc_dt); \
    z = 0; \
    DISALLOW_BACKPROP;\
} while(0)

#define PROP_X0 \
do { \
    if (mcgravitation) { Coords mcLocG; int mc_ret; \
    double mc_dt, mc_gx, mc_gy, mc_gz; \
    mcLocG = rot_apply(ROT_A_CURRENT_COMP, coords_set(0,-GRAVITY,0)); \
    coords_get(mcLocG, &mc_gx, &mc_gy, &mc_gz); \
    mc_ret = solve_2nd_order(&mc_dt, NULL, -mc_gx/2, -vx, -x); \
    if (mc_ret) {PROP_GRAV_DT(mc_dt, mc_gx, mc_gy, mc_gz); x=0;}\
    else if (mcallowbackprop == 0 && mc_dt < 0) { ABSORB; }; } \
    else mcPROP_X0; \
    DISALLOW_BACKPROP;\
} while(0)

#define mcPROP_X0 \
do { \
    double mc_dt; \
    if(vx == 0) { ABSORB; }; \
    mc_dt = -x/vx; \
    if(mc_dt < 0 && mcallowbackprop == 0) { ABSORB; }; \
    mcPROP_DT(mc_dt); \
    x = 0; \
    DISALLOW_BACKPROP;\
} while(0)

#define PROP_Y0 \
do { \
    if (mcgravitation) { Coords mcLocG; int mc_ret; \
    double mc_dt, mc_gx, mc_gy, mc_gz; \
    mcLocG = rot_apply(ROT_A_CURRENT_COMP, coords_set(0,-GRAVITY,0)); \
    coords_get(mcLocG, &mc_gx, &mc_gy, &mc_gz); \
    mc_ret = solve_2nd_order(&mc_dt, NULL, -mc_gy/2, -vy, -y); \
    if (mc_ret) {PROP_GRAV_DT(mc_dt, mc_gx, mc_gy, mc_gz); y=0;}\
    else if (mcallowbackprop == 0 && mc_dt < 0) { ABSORB; }; } \
    else mcPROP_Y0; \
    DISALLOW_BACKPROP;\
} while(0)


#define mcPROP_Y0 \
do { \
    double mc_dt; \
    if(vy == 0) { ABSORB; }; \
    mc_dt = -y/vy; \
    if(mc_dt < 0 && mcallowbackprop == 0) { ABSORB; }; \
    mcPROP_DT(mc_dt); \
    y = 0; \
    DISALLOW_BACKPROP; \
} while(0)


#ifdef DEBUG

#    define DEBUG_STATE() if(!mcdotrace); else \
        printf("STATE: %g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g\n", \
            x,y,z,vx,vy,vz,t,sx,sy,sz,p);
#    define DEBUG_SCATTER() if(!mcdotrace); else \
        printf("SCATTER: %g, %g, %g, %g, %g, %g, %g, %g, %g, %g, %g\n", \
            x,y,z,vx,vy,vz,t,sx,sy,sz,p);

#else

#    define DEBUG_STATE()
#    define DEBUG_SCATTER()

#endif


//
//  Implementations -> 


/*******************************************************************************
* mcsetstate: transfer parameters into global McStas variables
*******************************************************************************/
Neutron mcsetstate(double x, double y, double z, double vx, double vy, double vz,
        double t, double sx, double sy, double sz, double p, int mcgravitation, void *mcMagnet, int mcallowbackprop)
{
    Neutron mcneutron;

    mcneutron.x  = x;
    mcneutron.y  = y;
    mcneutron.z  = z;
    mcneutron.vx = vx;
    mcneutron.vy = vy;
    mcneutron.vz = vz;
    mcneutron.t  = t;
    mcneutron.sx = sx;
    mcneutron.sy = sy;
    mcneutron.sz = sz;
    mcneutron.p  = p;
    mcneutron.mcgravitation = mcgravitation;
    mcneutron.mcMagnet = mcMagnet;
    mcneutron.allow_backprop = mcallowbackprop;
    mcneutron._uid       = 0;
    mcneutron._index     = 1;
    mcneutron._absorbed  = 0;
    mcneutron._restore   = 0;
    mcneutron._scattered = 0;
    mcneutron.flag_nocoordschange = 0;

    /* init tmp-vars - FIXME are they used? */
    mcneutron._mctmp_a = mcneutron._mctmp_b =  mcneutron._mctmp_c = 0;
    // what about mcneutron._logic ?
    return(mcneutron);
} /* mcsetstate */

/*******************************************************************************
* mcgetstate: get neutron parameters from particle structure
*******************************************************************************/
Neutron mcgetstate(Neutron mcneutron, double *x, double *y, double *z,
            double *vx, double *vy, double *vz, double *t,
            double *sx, double *sy, double *sz, double *p)
{
    *x  =  mcneutron.x;
    *y  =  mcneutron.y;
    *z  =  mcneutron.z;
    *vx =  mcneutron.vx;
    *vy =  mcneutron.vy;
    *vz =  mcneutron.vz;
    *t  =  mcneutron.t;
    *sx =  mcneutron.sx;
    *sy =  mcneutron.sy;
    *sz =  mcneutron.sz;
    *p  =  mcneutron.p;

    return(mcneutron);
} /* mcgetstate */


/*******************************************************************************
* mccoordschanges: old style rotation routine rot -> (x y z) ,(vx vy vz),(sx,sy,sz)
*******************************************************************************/
void
mccoordschanges(Coords a, Rotation t, double *x, double *y, double *z,
            double *vx, double *vy, double *vz, double *sx, double *sy, double *sz)
{
    Coords b, c;

    b.x = *x;
    b.y = *y;
    b.z = *z;
    c = rot_apply(t, b);
    b = coords_add(c, a);
    *x = b.x;
    *y = b.y;
    *z = b.z;

    if ( (vz && vy  && vx) && (*vz != 0.0 || *vx != 0.0 || *vy != 0.0) )
        mccoordschange_polarisation(t, vx, vy, vz);

    if ( (sz && sy  && sx) && (*sz != 0.0 || *sx != 0.0 || *sy != 0.0) )
        mccoordschange_polarisation(t, sx, sy, sz);

}

/* intersection routines ==================================================== */

/*******************************************************************************
* inside_rectangle: Check if (x,y) is inside rectangle (xwidth, yheight)
* return 0 if outside and 1 if inside
*******************************************************************************/
int inside_rectangle(double x, double y, double xwidth, double yheight)
{
    if (x>-xwidth/2 && x<xwidth/2 && y>-yheight/2 && y<yheight/2)
        return 1;
    else
        return 0;
}

/*******************************************************************************
    * box_intersect: compute time intersection with a box
    * returns 0 when no intersection is found
    *      or 1 in case of intersection with resulting times dt_in and dt_out
    * This function written by Stine Nyborg, 1999.
    *******************************************************************************/
int box_intersect(double *dt_in, double *dt_out,
                double x, double y, double z,
                double vx, double vy, double vz,
                double dx, double dy, double dz)
{
    double x_in, y_in, z_in, tt, t[6], a, b;
    int i, count, s;

        /* Calculate intersection time for each of the six box surface planes
        *  If the box surface plane is not hit, the result is zero.*/

    if(vx != 0)
    {
        tt = -(dx/2 + x)/vx;
        y_in = y + tt*vy;
        z_in = z + tt*vz;
        if( y_in > -dy/2 && y_in < dy/2 && z_in > -dz/2 && z_in < dz/2)
        t[0] = tt;
        else
        t[0] = 0;

        tt = (dx/2 - x)/vx;
        y_in = y + tt*vy;
        z_in = z + tt*vz;
        if( y_in > -dy/2 && y_in < dy/2 && z_in > -dz/2 && z_in < dz/2)
        t[1] = tt;
        else
        t[1] = 0;
    }
    else
        t[0] = t[1] = 0;

    if(vy != 0)
    {
        tt = -(dy/2 + y)/vy;
        x_in = x + tt*vx;
        z_in = z + tt*vz;
        if( x_in > -dx/2 && x_in < dx/2 && z_in > -dz/2 && z_in < dz/2)
        t[2] = tt;
        else
        t[2] = 0;

        tt = (dy/2 - y)/vy;
        x_in = x + tt*vx;
        z_in = z + tt*vz;
        if( x_in > -dx/2 && x_in < dx/2 && z_in > -dz/2 && z_in < dz/2)
        t[3] = tt;
        else
        t[3] = 0;
    }
    else
        t[2] = t[3] = 0;

    if(vz != 0)
    {
        tt = -(dz/2 + z)/vz;
        x_in = x + tt*vx;
        y_in = y + tt*vy;
        if( x_in > -dx/2 && x_in < dx/2 && y_in > -dy/2 && y_in < dy/2)
        t[4] = tt;
        else
        t[4] = 0;

        tt = (dz/2 - z)/vz;
        x_in = x + tt*vx;
        y_in = y + tt*vy;
        if( x_in > -dx/2 && x_in < dx/2 && y_in > -dy/2 && y_in < dy/2)
        t[5] = tt;
        else
        t[5] = 0;
    }
    else
        t[4] = t[5] = 0;

    /* The intersection is evaluated and *dt_in and *dt_out are assigned */

    a = b = s = 0;
    count = 0;

    for( i = 0; i < 6; i = i + 1 )
        if( t[i] == 0 )
        s = s+1;
        else if( count == 0 )
        {
        a = t[i];
        count = 1;
        }
        else
        {
        b = t[i];
        count = 2;
        }

    if ( a == 0 && b == 0 )
        return 0;
    else if( a < b )
    {
        *dt_in = a;
        *dt_out = b;
        return 1;
    }
    else
    {
        *dt_in = b;
        *dt_out = a;
        return 1;
}

} /* box_intersect */

/*******************************************************************************
    * cylinder_intersect: compute intersection with a cylinder
    * returns 0 when no intersection is found
    *      or 2/4/8/16 bits depending on intersection,
    *     and resulting times t0 and t1
    * Written by: EM,NB,ABA 4.2.98
    *******************************************************************************/
int cylinder_intersect(double *t0, double *t1, double x, double y, double z,
                double vx, double vy, double vz, double r, double h)
{
    double D, t_in, t_out, y_in, y_out;
    int ret=1;

    D = (2*vx*x + 2*vz*z)*(2*vx*x + 2*vz*z) - 4*(vx*vx + vz*vz)*(x*x + z*z - r*r);

    if (D>=0)
    {
        if (vz*vz + vx*vx) {
        t_in  = (-(2*vz*z + 2*vx*x) - sqrt(D))/(2*(vz*vz + vx*vx));
        t_out = (-(2*vz*z + 2*vx*x) + sqrt(D))/(2*(vz*vz + vx*vx));
        } else if (vy) { /* trajectory parallel to cylinder axis */
        t_in = (-h/2-y)/vy;
        t_out = (h/2-y)/vy;
        if (t_in>t_out){
            double tmp=t_in;
            t_in=t_out;t_out=tmp;
        }
        } else return 0;
        y_in = vy*t_in + y;
        y_out =vy*t_out + y;

        if ( (y_in > h/2 && y_out > h/2) || (y_in < -h/2 && y_out < -h/2) )
        return 0;
        else
        {
        if (y_in > h/2)
            { t_in = ((h/2)-y)/vy; ret += 2; }
        else if (y_in < -h/2)
            { t_in = ((-h/2)-y)/vy; ret += 4; }
        if (y_out > h/2)
            { t_out = ((h/2)-y)/vy; ret += 8; }
        else if (y_out < -h/2)
            { t_out = ((-h/2)-y)/vy; ret += 16; }
        }
        *t0 = t_in;
        *t1 = t_out;
        return ret;
    }
    else
    {
        *t0 = *t1 = 0;
        return 0;
    }
} /* cylinder_intersect */


/*******************************************************************************
    * sphere_intersect: Calculate intersection between a line and a sphere.
    * returns 0 when no intersection is found
    *      or 1 in case of intersection with resulting times t0 and t1
    *******************************************************************************/
int sphere_intersect(double *t0, double *t1, double x, double y, double z,
                double vx, double vy, double vz, double r)
{
    double A, B, C, D, v;

    v = sqrt(vx*vx + vy*vy + vz*vz);
    A = v*v;
    B = 2*(x*vx + y*vy + z*vz);
    C = x*x + y*y + z*z - r*r;
    D = B*B - 4*A*C;
    if(D < 0)
        return 0;
    D = sqrt(D);
    *t0 = (-B - D) / (2*A);
    *t1 = (-B + D) / (2*A);
    return 1;
} /* sphere_intersect */

/*******************************************************************************
    * plane_intersect: Calculate intersection between a plane and a line.
    * returns 0 when no intersection is found (i.e. line is parallel to the plane)
    * returns 1 or -1 when intersection time is positive and negative respectively
    *******************************************************************************/
int plane_intersect(double *t, double x, double y, double z,
                double vx, double vy, double vz, double nx, double ny, double nz, double wx, double wy, double wz)
{
    double s;
    if (fabs(s=scalar_prod(nx,ny,nz,vx,vy,vz))<FLT_EPSILON) return 0;
    *t = - scalar_prod(nx,ny,nz,x-wx,y-wy,z-wz)/s;
    if (*t<0) return -1;
    else return 1;
} /* plane_intersect */


#endif

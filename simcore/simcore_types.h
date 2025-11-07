#ifndef __SIMCORE_TYPES_H__
#define __SIMCORE_TYPES_H__


#include <float.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <ctype.h>
#include <math.h>


struct NeutronSmall {
    double x;
    double y;
    double z;
    double vx;
    double vy;
    double vz = 1;

    double t;
    double p = 1;

    float flags;
};


struct NeutronBig {
    double x; /* position [m] */
    double y; /* position [m] */
    double z; /* position [m] */
    double vx; /* velocity [m/s] */
    double vy; /* velocity [m/s] */
    double vz = 1; /* velocity [m/s] */
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
    double t;     // time
    double p = 1;     // event weight
    long long _uid;  /* Unique event ID */
    long _index;     /* component index where to send this event */
    long _absorbed;  /* flag set to TRUE when this event is to be removed/ignored */
    long _scattered; /* flag set to TRUE when this event has interacted with the last component instance */
    long _restore;   /* set to true if neutron event must be restored */
    long flag_nocoordschange;   /* set to true if particle is jumping */
};


//#define Neutron NeutronSmall
#define Neutron NeutronBig


Neutron _particle_global_randnbuse_var;
Neutron* _particle = &_particle_global_randnbuse_var;


// defines & globals


typedef double MCNUM;
//typedef struct { MCNUM x, MCNUM y, MCNUM z; } Coords;
//typedef MCNUM Rotation[3][3];

struct Coords {
    MCNUM x;
    MCNUM y;
    MCNUM z;
};

typedef double Rotation[3][3];


//
//  Generic Component struct


// NOTE: This header is repeated on every single specific component type.
//      The commonality is represented here in order to be used by generic
//      functionality that configures the simcore engine.
//      E.g. To set the legacy position/rotation structs that are used by the
//      engine, but represented by 4x4 affine transforms in "high-level" app code.
struct ComponentSharedHeader {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;
};


enum CompMonitorType {
    MT_NOT,

    MT_0D,
    MT_1D,
    MT_2D,

    MT_CNT
};

struct Monitor {
    CompMonitorType mon_tpe;
    void *comp;

    Str comp_name;
    Str title;
    Str xlabel;
    Str ylabel;
    Str xvar;
    Str fname;

    f32 xmin;
    f32 xmax;
    f32 ymin;
    f32 ymax;

    s32 binm_x;
    s32 binn_y;

    double *N; // otherwise known as p0, ray hit count
    double *p; // otherwise known as p1, integrated probability per bin
    double *p2; // p squared, integrated per bin, has something to do with the error bar
};


// NOTE: The abstract / base type wrapper used by all high-level functionality (the specific 
//      component classes are only used under-the-hood, e.g. by the legacy simcore engine).
struct Component {
    Transform *transform;
    Matrix4f t_world2loc; // the inverse matrix of transform->t_world
    Matrix4f t_prev2loc; // transforms particles from previous local system into our local system; n_loc = ( t_current_world_inv * t_prev_world ) * n_prev_loc

    u32 type; // refers to CompType, a cogen'd type
    u32 cat; // refers to CompCategory, a cogen'd type
    Str type_name; // duplicated info from the shared header
    Str name;  // duplicated info from the shared header

    Wireframe display;
    Monitor monitor;
    bool interactable;
    bool interactable_this_frame;
    bool collided_this_frame;

    // pointer to the underlying component
    void *comp;

    ComponentSharedHeader *GetHeader() {
        return (ComponentSharedHeader*) comp;
    }
};


//

#define randstate_t uint64_t
#define MC_PATHSEP_S "/"
#define MC_PATHSEP_C '/'
#define MCCODE_STRING "tracetool_string"
#define MCCODE_NAME "tracetool_name"

int  defaultmain = 1;
int  traceenabled = 0;
char instrument_name[200] = "default_instr_name";
char instrument_source[200] = "default_instr_source";
char *instrument_exe = (char*) "default_instr_exe";
int  numipar = 0;
FILE *siminfo_file = NULL;


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


#endif

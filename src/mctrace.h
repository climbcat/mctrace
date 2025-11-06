#ifndef __MCTRACE__
#define __MCTRACE__


#include "../simcore/simcore_types.h"


#define MCT_COLOR_SELECTION_BOX         (( Color { 74, 78, 121, 128 } ))
#define MCT_COLOR_MONITOR               COLOR_RED
#define MCT_COLOR_TRAJECTORY            COLOR_GRAY_75
#define MCT_COLOR_OPTICS                COLOR_BLUE
#define MCT_COLOR_SOURCE_OR_SAMPLE      COLOR_RED
#define MCT_COLOR_DEFOCUSED             COLOR_GRAY_30


enum McTraceMode {
    MTM_UNDEF,

    MTM_SIM,
    MTM_TRACE,
    MTM_MONITORS,
    MTM_PLOT,

    MTM_CNT
};


struct McTraceApp {
    bool draw_rays;
    bool draw_plane;

    McTraceMode mode;
    Component *comp_selected = NULL;
};




inline
void ParticleTransform(Matrix4f t, Neutron *n) {
    Vector3f n_pos = { (f32) n->x, (f32) n->y, (f32) n->z };
    n_pos = TransformPoint(t, n_pos);
    n->x = n_pos.x;
    n->y = n_pos.y;
    n->z = n_pos.z;

    Vector3f n_vel = { (f32) n->vx, (f32) n->vy, (f32) n->vz };
    n_vel = TransformDirection(t, n_vel);
    n->vx = n_vel.x;
    n->vy = n_vel.y;
    n->vz = n_vel.z;

    // NOTE: figure out at what point the components start utilizing the spin // s
}

inline
Neutron ParticleImmutableTransform(Matrix4f t, Neutron n) {
    Neutron r = {};
    Vector3f n_pos = { (f32) n.x, (f32) n.y, (f32) n.z };
    n_pos = TransformPoint(t, n_pos);
    r.x = n_pos.x;
    r.y = n_pos.y;
    r.z = n_pos.z;

    Vector3f n_vel = { (f32) n.vx, (f32) n.vy, (f32) n.vz };
    n_vel = TransformDirection(t, n_vel);
    r.vx = n_vel.x;
    r.vy = n_vel.y;
    r.vz = n_vel.z;

    r.t = n.t;
    r.p = n.p;

    return r;
}

void ParticlePrintWorld(Matrix4f t_world, Neutron n) {
    n = ParticleImmutableTransform(t_world, n);
    printf("(%g %g %g, %g %g %g, %g, %g)\n", n.x, n.y, n.z, n.vx, n.vy, n.vz, n.t, n.p);
}

void ParticlePrint(Neutron n) {
    printf("(%g %g %g, %g %g %g, %g, %g)\n", n.x, n.y, n.z, n.vx, n.vy, n.vz, n.t, n.p);
}




#endif

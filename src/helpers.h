#ifndef __MCT_HELPERS__
#define __MCT_HELPERS__


Color ComponentCatToColor(u32 cat) {
    switch ((CompCategory) cat) {
        case CCAT_sources: return COLOR_RED;
        case CCAT_monitors: return COLOR_GREEN_50;
        case CCAT_contrib: return COLOR_GRAY_50;
        case CCAT_misc: return COLOR_BLACK;
        case CCAT_optics: return COLOR_BLUE;
        case CCAT_samples: return COLOR_RED;
    }

    return COLOR_BLACK;
}

struct InstrumentConfig {
    Instrument instr;
    Array<Component*> comps;
    SceneGraphHandle scenegraph;

    Matrix4f box_t_worold;
    Vector3f box_dims;
};

void UpdateLegacyTransforms(Array<Component*> comps) {
    Matrix4f t_world_prev = Matrix4f_Identity();
    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];

        Matrix4f t_world = comp->transform->t_world;
        comp->t_prev2loc = TransformGetInverse(t_world) * t_world_prev;
        t_world_prev = t_world;

        ComponentSharedHeader *hdr = comp->GetHeader();
        hdr-> position_absolute.x =  comp->transform->t_world.m[0][3];
        hdr-> position_absolute.y =  comp->transform->t_world.m[1][3];
        hdr-> position_absolute.z =  comp->transform->t_world.m[2][3];
        hdr-> position_relative.x =  comp->transform->t_loc.m[0][3];
        hdr-> position_relative.y =  comp->transform->t_loc.m[1][3];
        hdr-> position_relative.z =  comp->transform->t_loc.m[2][3];

        for (u32 i = 0; i < 3; i++) {
            for (u32 j = 0; j < 3; j++) {
                hdr->rotation_absolute[i][j] = comp->transform->t_world.m[i][j];
                hdr->rotation_relative[i][j] = comp->transform->t_loc.m[i][j];
            }
        }
    }
}

void DisplayComponents(MArena *a_dest, Array<Component*> comps) {
    for (s32 i = 0; i < comps.len; ++i) {
        Component *comp = comps.arr[i];

        printf("%.*s\n", comp->name.len, comp->name.str);
        PrintTransform(g_mcdis_t_world);

        McDisplayNext(cbui.ctx->a_pers, comp->transform->t_world);
        DisplayComponent(comp);        

        comp->display = {};
        comp->display.transform = Matrix4f_Identity();
        comp->display.color = ComponentCatToColor(comp->cat);
        comp->display.segments.arr = g_mcdis_anchors.lst;
        comp->display.segments.len = g_mcdis_anchors.len;
        comp->display.segments.max = g_mcdis_anchors.len;
        comp->display.CalculateAABox();
    }
}

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

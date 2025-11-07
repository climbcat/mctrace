#ifndef __MCT_COMPS_HELPERS__
#define __MCT_COMPS_HELPERS__


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


#endif

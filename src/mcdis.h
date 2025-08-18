#ifndef __MCDIS_H__
#define __MCDIS_H__


static List<Vector3f> g_mcdis_anchors;
static MArena *g_mcdis_a_dest;
static Matrix4f g_mcdis_t_world;
static bool g_mcdis_dbg;


void McDisplayNext(MArena *a_dest, Matrix4f t_world) {
    g_mcdis_a_dest = a_dest;
    g_mcdis_anchors = InitList<Vector3f>(a_dest, 0);

    // TODO: phase out
    g_mcdis_t_world = t_world;
}


void mcdis_multiline_hook25(int count, ...) {
    // The 2025 mcdis_ hook implementation

    va_list ap;

    Vector3f prev = {};
    Vector3f current = {};
    double x,y,z;

    va_start(ap, count);
    int iter = count;

    // TODO: re-write it as a for-loop
    while(iter--) {
        x = va_arg(ap, double);
        y = va_arg(ap, double);
        z = va_arg(ap, double);

        if (iter == count - 1) {
            // set the first anchor

            prev = TransformPoint(g_mcdis_t_world, { (f32) x, (f32) y, (f32) z });
        }
        else {
            // iterate "inner" anchor point
            current = TransformPoint(g_mcdis_t_world, { (f32) x, (f32) y, (f32) z});
            if (g_mcdis_dbg) { printf(" (%f %f %f) -> (%f %f %f) \n", prev.x, prev.y, prev.z, current.x, current.y, current.z); }

            ArenaAlloc(g_mcdis_a_dest, 2 * sizeof(Vector3f));
            g_mcdis_anchors.Add(prev);
            g_mcdis_anchors.Add(current);
            prev = current;
        }
    }

    va_end(ap);
}

void mcdis_line_hook25(double x1, double y1, double z1, double x2, double y2, double z2){
    // The 2025 mcdis_ hook implementation

    Vector3f a1 = TransformPoint(g_mcdis_t_world, { (f32) x1, (f32) y1, (f32) z1 });
    Vector3f a2 = TransformPoint(g_mcdis_t_world,{ (f32) x2, (f32) y2, (f32) z2 });

    if (g_mcdis_dbg) { printf("l : (%f %f %f) -> (%f %f %f) \n", a1.x, a1.y, a1.z, a2.x, a2.y, a2.z); }

    ArenaAlloc(g_mcdis_a_dest, 2 * sizeof(Vector3f));
    g_mcdis_anchors.Add(a1);
    g_mcdis_anchors.Add(a2);
}

// mcdis_Circle is implemented alongside the other line-generating mcdis_* functions, in simcore.h
void mcdis_Circle(double x, double y, double z, double r, double nx, double ny, double nz);

void mcdis_circle_hook25(char *plane, double x, double y, double z, double r) {
    if ( !strcmp(plane, "xy") ) {
        if (g_mcdis_dbg) { printf("mcdis_circle_hook25: xy\n"); }
        mcdis_Circle(x, y, z, r, 0, 0, 1);
    }
    else if ( !strcmp(plane, "xz") ) {
        if (g_mcdis_dbg) { printf("mcdis_circle_hook25: xz\n"); }
        mcdis_Circle(x, y, z, r, 0, 1, 0);
    }
    else if ( !strcmp(plane, "yz") ) {
        if (g_mcdis_dbg) { printf("mcdis_circle_hook25: yz\n"); }
        mcdis_Circle(x, y, z, r, 1, 0, 0);
    }
    else {
        printf("mcdis_circle_hook25: %s (WARN: unregistered branch)\n", plane);
    }
}


#endif

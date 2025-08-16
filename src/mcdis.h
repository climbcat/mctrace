#ifndef __MCDIS_H__
#define __MCDIS_H__


static Array<Vector3f> g_mcdis_anchors;
static Matrix4f g_mcdis_t_world;

void DisplayCaptureInit(MArena *a_dest, u32 max_lines_count = 2048) {
    assert(g_mcdis_anchors.arr == NULL);

    g_mcdis_anchors = InitArray<Vector3f>(a_dest, max_lines_count * 2);
}

void DisplaySetCurrentComponentWorldTransform(Matrix4f t_world) {
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

    while(iter--) {
        x = va_arg(ap, double);
        y = va_arg(ap, double);
        z = va_arg(ap, double);

        if (iter == count) {
            // set the first anchor

            prev = { (f32) x, (f32) y, (f32) z };
            prev = TransformPoint(g_mcdis_t_world, prev);
        }
        else {
            // iterate "inner" anchor point

            current = { (f32) x, (f32) y, (f32) z};
            current = TransformPoint(g_mcdis_t_world, current);

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

    g_mcdis_anchors.Add(a1);
    g_mcdis_anchors.Add(a2);
}


#endif

#ifndef __MCDIS_H__
#define __MCDIS_H__


void mcdis_multiline_reimpl_2025(int count, ...) {
    // TODO: re-implement

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

void mcdis_line_reimpl_2025(double x1, double y1, double z1, double x2, double y2, double z2){
    // TODO: re-implement

    printf("MCDISPLAY: multiline(2,%g,%g,%g,%g,%g,%g)\n", x1,y1,z1,x2,y2,z2);
}


#endif

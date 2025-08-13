#ifndef __Monochromator_2foc__
#define __Monochromator_2foc__

// share block


#ifndef DIV_CUTOFF
#define DIV_CUTOFF 2            /* ~ 10^-5 cutoff. */
#endif


struct Monochromator_2foc {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters
    char *reflect = (char*) 0;
    double zwidth = 0.01;
    double yheight = 0.01;
    double gap = 0.0005;
    double NH = 11;
    double NV = 11;
    double mosaich = 30.0;
    double mosaicv = 30.0;
    double r0 = 0.7;
    double Q = 1.8734;
    double RV = 0;
    double RH = 0;
    double DM = 0;
    double mosaic = 0;
    double width = 0;
    double height = 0;
    double verbose = 0;

    // declares
    double mos_y;
    double mos_z;
    double mono_Q;
    double SlabWidth;
    double SlabHeight;
    t_Table rTable;
};

Monochromator_2foc Create_Monochromator_2foc(s32 index, char *name) {
    Monochromator_2foc _comp = {};
    Monochromator_2foc *comp = &_comp;
    comp->type = (char*) "Monochromator_2foc";
    comp->name = name;
    comp->index = index;

    return _comp;
}

void Init_Monochromator_2foc(Monochromator_2foc *comp, Instrument *instrument) {

    #define reflect comp->reflect
    #define zwidth comp->zwidth
    #define yheight comp->yheight
    #define gap comp->gap
    #define NH comp->NH
    #define NV comp->NV
    #define mosaich comp->mosaich
    #define mosaicv comp->mosaicv
    #define r0 comp->r0
    #define Q comp->Q
    #define RV comp->RV
    #define RH comp->RH
    #define DM comp->DM
    #define mosaic comp->mosaic
    #define width comp->width
    #define height comp->height
    #define verbose comp->verbose

    #define mos_y comp->mos_y
    #define mos_z comp->mos_z
    #define mono_Q comp->mono_Q
    #define SlabWidth comp->SlabWidth
    #define SlabHeight comp->SlabHeight
    #define rTable comp->rTable
    ////////////////////////////////////////////////////////////////


    if (mosaic != 0) {
        mos_y = mosaic;
        mos_z = mos_y;
    }
    else {
        mos_y = mosaich;
        mos_z = mosaicv;
    }

    mono_Q = Q;
    if (DM != 0) mono_Q = 2*PI/DM;

    if (mono_Q == 0) { fprintf(stderr,"Monochromator_2foc: %s: Error scattering vector Q = 0\n", NAME_CURRENT_COMP); exit(-1); }
    if (r0 == 0) { fprintf(stderr,"Monochromator_2foc: %s: Error reflectivity r0 is null\n", NAME_CURRENT_COMP); exit(-1); }
    if (NH*NV == 0) { fprintf(stderr,"Monochromator_2foc: %s: no slabs ??? (NH or NV=0)\n", NAME_CURRENT_COMP); exit(-1); }

    if (verbose)
    {
        printf("Monochromator_2foc: component %s Q=%.3g Angs-1 (DM=%.4g Angs)\n", NAME_CURRENT_COMP, mono_Q, 2*PI/mono_Q);
        if (NH*NV == 1) printf("            flat.\n");
        else
        { if (NH > 1)
        { printf("            horizontal: %i blades", (int)NH);
            if (RH != 0) printf(" focusing with RH=%.3g [m]", RH);
            printf("\n");
        }
        if (NV > 1)
        { printf("            vertical:   %i blades", (int)NV);
            if (RV != 0) printf(" focusing with RV=%.3g [m]", RV);
            printf("\n");
        }
        }
    }

    if (reflect != NULL)
    {
        if (verbose)
            fprintf(stdout, "Monochromator_2foc: %s : Reflectivity data (k, R)\n", NAME_CURRENT_COMP);
        Table_Read(&rTable, reflect, 1); /* read 1st block data from file into rTable */
        Table_Rebin(&rTable);         /* rebin as evenly, increasing array */
        if (rTable.rows < 2)
            Table_Free(&rTable);
        Table_Info(rTable);
    }
    else {
        rTable.data = NULL;
    }

    if (width == 0) SlabWidth = zwidth;
    else SlabWidth = (width+gap)/NH - gap;
    if (height == 0) SlabHeight = yheight;
    else SlabHeight = (height+gap)/NV - gap;


    ////////////////////////////////////////////////////////////////
    #undef reflect
    #undef zwidth
    #undef yheight
    #undef gap
    #undef NH
    #undef NV
    #undef mosaich
    #undef mosaicv
    #undef r0
    #undef Q
    #undef RV
    #undef RH
    #undef DM
    #undef mosaic
    #undef width
    #undef height
    #undef verbose

    #undef mos_y
    #undef mos_z
    #undef mono_Q
    #undef SlabWidth
    #undef SlabHeight
    #undef rTable

}

void Trace_Monochromator_2foc(Monochromator_2foc *comp, Neutron *particle, Instrument *instrument) {
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

    #define reflect comp->reflect
    #define zwidth comp->zwidth
    #define yheight comp->yheight
    #define gap comp->gap
    #define NH comp->NH
    #define NV comp->NV
    #define mosaich comp->mosaich
    #define mosaicv comp->mosaicv
    #define r0 comp->r0
    #define Q comp->Q
    #define RV comp->RV
    #define RH comp->RH
    #define DM comp->DM
    #define mosaic comp->mosaic
    #define width comp->width
    #define height comp->height
    #define verbose comp->verbose

    #define mos_y comp->mos_y
    #define mos_z comp->mos_z
    #define mono_Q comp->mono_Q
    #define SlabWidth comp->SlabWidth
    #define SlabHeight comp->SlabHeight
    #define rTable comp->rTable
    ////////////////////////////////////////////////////////////////


    double dt;

    if(vx != 0.0 && (dt = -x/vx) >= 0.0)
    {
        double zmin,zmax, ymin,ymax, zp,yp, y1,z1,t1;

        zmax = ((NH*(SlabWidth+gap))-gap)/2;
        zmin = -1*zmax;
        ymax = ((NV*(SlabHeight+gap))-gap)/2;
        ymin = -1*ymax;
        y1 = y + vy*dt;             /* Propagate to crystal plane */
        z1 = z + vz*dt;
        t1 = t + dt;
        zp = fmod ( (z1-zmin),(SlabWidth+gap) );
        yp = fmod ( (y1-ymin),(SlabHeight+gap) );


        /* hit a slab or a gap ? */

        if (z1>zmin && z1<zmax && y1>ymin && y1<ymax && zp<SlabWidth && yp< SlabHeight)
        {
            double row,col, sna,snb,csa,csb,vxp,vyp,vzp;
            double v, theta0, theta, tmp3;
            double tilth,tiltv;         /* used to calculate tilt angle of slab */

            col = ceil ( (z1-zmin)/(SlabWidth+gap));  /* which slab hit ? */
            row = ceil ( (y1-ymin)/(SlabHeight+gap));
            if (RH != 0) tilth = asin((col-(NH+1)/2)*(SlabWidth+gap)/RH);
            else tilth=0;
            if (RV != 0) tiltv = -asin((row-(NV+1)/2)*(SlabHeight+gap)/RV);
            else tiltv=0;

            /* rotate with tilth and tiltv */

            sna = sin(tilth);
            snb = sin(tiltv);
            csa = cos(tilth);
            csb = cos(tiltv);
            vxp = vx*csa*csb+vy*snb-vz*sna*csb;
            vyp = -vx*csa*snb+vy*csb+vz*sna*snb;
            vzp = vx*sna+vz*csa;

            /* First: scattering in plane */
            /* theta0 = atan2(vx,vz);  neutron angle to slab Risoe version */

            v = sqrt(vxp*vxp+vyp*vyp+vzp*vzp);
            theta0 = asin(vxp/v);                /* correct neutron angle to slab */

            theta = asin(Q2V*mono_Q/(2.0*v));               /* Bragg's law */
            if (theta0 < 0)
                    theta = -theta;
            tmp3 = (theta-theta0)/(MIN2RAD*mos_y);
            if (tmp3 < DIV_CUTOFF)
            {
                double my_r0, k;
                double dphi,tmp1,tmp2,tmp4,vratio,phi,cs,sn;

                k = V2K*v;

                if (rTable.data != NULL)
                {
                    my_r0 = r0*Table_Value(rTable, k, 1); /* 2nd column */
                }
                else
                    my_r0 = r0;

                if (my_r0 >= 1){
                    if (verbose) fprintf(stdout, "Warning: Monochromator_2foc: %s: lowered reflectivity from %f to 0.99 (k=%f)\n", 
                        NAME_CURRENT_COMP, my_r0, k);
                    my_r0=0.99;
                }
                if (my_r0 < 0)
                {
                    if (verbose) fprintf(stdout, "Warning: Monochromator_2foc: %s: raised reflectivity from %f to 0 (k=%f)\n", 
                    NAME_CURRENT_COMP, my_r0, k);
                    my_r0=0;
                }
                x = 0.0;
                y = y1;
                z = z1;
                t = t1;
                
                /* reflectivity */
                t1 = fabs(my_r0)*exp(-tmp3*tmp3*4*log(2));
                if (t1 <= 0) ABSORB;
                if (t1 > 1)  t1 = 1;
                p *= t1; /* Use mosaics */
                
                tmp1 = 2*theta;
                cs = cos(tmp1);
                sn = sin(tmp1);
                tmp2 = cs*vxp - sn*vzp;
                vyp = vyp;
                /* vz = cs*vz + sn*vx; diese Zeile wurde durch die folgende ersetzt */
                tmp4 = vyp/vzp;  /* korrigiert den schr�en Einfall aufs Pl�tchen  */
                vzp = cs*(-vyp*sin(tmp4)+vzp*cos(tmp4)) + sn*vxp;
                vxp = tmp2;

                /* Second: scatering out of plane.
                Approximation is that Debye-Scherrer cone is a plane */

                phi = atan2(vyp,vzp);  /* out-of plane angle */
                dphi = (MIN2RAD*mos_z)/(2*sqrt(2*log(2)))*randnorm();  /* MC choice: */
                /* Vertical angle of the crystallite */
                vyp = vzp*tan(phi+2*dphi*sin(theta));
                vratio = v/sqrt(vxp*vxp+vyp*vyp+vzp*vzp);
                vzp = vzp*vratio;
                vyp = vyp*vratio;                             /* Renormalize v */
                vxp = vxp*vratio;

                /* rotate v coords back */
                vx = vxp*csb*csa-vyp*snb*csa+vzp*sna;
                vy = vxp*snb+vyp*csb;
                vz = -vxp*csb*sna+vyp*snb*sna+vzp*csa;
                /* v=sqrt(vx*vx+vy*vy+vz*vz);  */
                SCATTER;
            } /* end if Bragg ok */
        } /* End intersect the crystal (if z1) */
    } /* End neutron moving towards crystal (if vx)*/


    ////////////////////////////////////////////////////////////////
    #undef reflect
    #undef zwidth
    #undef yheight
    #undef gap
    #undef NH
    #undef NV
    #undef mosaich
    #undef mosaicv
    #undef r0
    #undef Q
    #undef RV
    #undef RH
    #undef DM
    #undef mosaic
    #undef width
    #undef height
    #undef verbose

    #undef mos_y
    #undef mos_z
    #undef mono_Q
    #undef SlabWidth
    #undef SlabHeight
    #undef rTable

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

void Save_Monochromator_2foc(Monochromator_2foc *comp) {

}

void Finally_Monochromator_2foc(Monochromator_2foc *comp) {

}

void Display_Monochromator_2foc(Monochromator_2foc *comp) {
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

    #define reflect comp->reflect
    #define zwidth comp->zwidth
    #define yheight comp->yheight
    #define gap comp->gap
    #define NH comp->NH
    #define NV comp->NV
    #define mosaich comp->mosaich
    #define mosaicv comp->mosaicv
    #define r0 comp->r0
    #define Q comp->Q
    #define RV comp->RV
    #define RH comp->RH
    #define DM comp->DM
    #define mosaic comp->mosaic
    #define width comp->width
    #define height comp->height
    #define verbose comp->verbose

    #define mos_y comp->mos_y
    #define mos_z comp->mos_z
    #define mono_Q comp->mono_Q
    #define SlabWidth comp->SlabWidth
    #define SlabHeight comp->SlabHeight
    #define rTable comp->rTable
    ////////////////////////////////////////////////////////////////


    int ih;
    for (ih = 0; ih < NH; ih++) {
        int iv;
        for(iv = 0; iv < NV; iv++)
        {
            double zmin,zmax,ymin,ymax;
            double xt, xt1, yt, yt1;

            zmin = (SlabWidth+gap)*(ih-NH/2.0)+gap/2;
            zmax = zmin+SlabWidth;
            ymin = (SlabHeight+gap)*(iv-NV/2.0)+gap/2;
            ymax = ymin+SlabHeight;

            if (RH) { 
                xt = zmin*zmin/RH;
                xt1 = zmax*zmax/RH; }
            else {
                xt = 0; xt1 = 0; 
            }

            if (RV) { 
                yt = ymin*ymin/RV;
                yt1 = ymax*ymax/RV;
            }
            else {
                yt = 0; yt1 = 0;
            }
            multiline(5, xt+yt, (double)ymin, (double)zmin,
                        xt+yt1, (double)ymax, (double)zmin,
                        xt1+yt1, (double)ymax, (double)zmax,
                        xt1+yt, (double)ymin, (double)zmax,
                        xt+yt, (double)ymin, (double)zmin);
        }
    }


    ////////////////////////////////////////////////////////////////
    #undef reflect
    #undef zwidth
    #undef yheight
    #undef gap
    #undef NH
    #undef NV
    #undef mosaich
    #undef mosaicv
    #undef r0
    #undef Q
    #undef RV
    #undef RH
    #undef DM
    #undef mosaic
    #undef width
    #undef height
    #undef verbose

    #undef mos_y
    #undef mos_z
    #undef mono_Q
    #undef SlabWidth
    #undef SlabHeight
    #undef rTable

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

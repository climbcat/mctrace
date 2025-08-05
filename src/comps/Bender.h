#ifndef __Bender__
#define __Bender__

// share block



struct Bender {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters
    double w;
    double h;
    double r;
    double Win = 0.04;
    double k = 1;
    double d = 0.001;
    double l = 0;
    double R0a = 0.99;
    double Qca = 0.021;
    double alphaa = 6.07;
    double ma = 2;
    double Wa = 0.003;
    double R0i = 0.99;
    double Qci = 0.021;
    double alphai = 6.07;
    double mi = 2;
    double Wi = 0.003;
    double R0s = 0.99;
    double Qcs = 0.021;
    double alphas = 6.07;
    double ms = 2;
    double Ws = 0.003;

    // declares
    double bk;
    double mWin;
};

Bender Create_Bender(s32 index, char *name) {
    Bender _comp = {};
    Bender *comp = &_comp;
    comp->type = (char*) "Bender";
    comp->name = name;
    comp->index = index;

    return _comp;
}

void Init_Bender(Bender *comp, Instrument *instrument) {

    #define w comp->w
    #define h comp->h
    #define r comp->r
    #define Win comp->Win
    #define k comp->k
    #define d comp->d
    #define l comp->l
    #define R0a comp->R0a
    #define Qca comp->Qca
    #define alphaa comp->alphaa
    #define ma comp->ma
    #define Wa comp->Wa
    #define R0i comp->R0i
    #define Qci comp->Qci
    #define alphai comp->alphai
    #define mi comp->mi
    #define Wi comp->Wi
    #define R0s comp->R0s
    #define Qcs comp->Qcs
    #define alphas comp->alphas
    #define ms comp->ms
    #define Ws comp->Ws

    #define bk comp->bk
    #define mWin comp->mWin
    ////////////////////////////////////////////////////////////////


    if (r <0) {
        fprintf(stderr,"Bender: error: %s: to bend in the other direction\n", NAME_CURRENT_COMP);
        fprintf(stderr,"        rotate comp on z-axis by 180 deg.\n"); exit(-1);
    }

    if (k*d > w) {
        fprintf(stderr,"Bender: error: %s has (k*d > w).\n", NAME_CURRENT_COMP);
        exit(-1);
    }
    if (w*h*r*Win*k == 0) {
        fprintf(stderr,"Bender: error: %s has one of w,h,r,Win,k null.\n", NAME_CURRENT_COMP);
        exit(-1);
    }
    /* width of one channel + thickness d of partition */
    mWin = Win;
    if (l!= 0 && r != 0)
        mWin = (double)l/(double)r;

    bk=(w+d)/k;
    if (mcgravitation)
        fprintf(stderr,"WARNING: Bender: %s: "
            "This component produces wrong results with gravitation !\n",
            NAME_CURRENT_COMP);


    ////////////////////////////////////////////////////////////////
    #undef w
    #undef h
    #undef r
    #undef Win
    #undef k
    #undef d
    #undef l
    #undef R0a
    #undef Qca
    #undef alphaa
    #undef ma
    #undef Wa
    #undef R0i
    #undef Qci
    #undef alphai
    #undef mi
    #undef Wi
    #undef R0s
    #undef Qcs
    #undef alphas
    #undef ms
    #undef Ws

    #undef bk
    #undef mWin

}

void Trace_Bender(Bender *comp, Neutron *particle, Instrument *instrument) {
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

    #define w comp->w
    #define h comp->h
    #define r comp->r
    #define Win comp->Win
    #define k comp->k
    #define d comp->d
    #define l comp->l
    #define R0a comp->R0a
    #define Qca comp->Qca
    #define alphaa comp->alphaa
    #define ma comp->ma
    #define Wa comp->Wa
    #define R0i comp->R0i
    #define Qci comp->Qci
    #define alphai comp->alphai
    #define mi comp->mi
    #define Wi comp->Wi
    #define R0s comp->R0s
    #define Qcs comp->Qcs
    #define alphas comp->alphas
    #define ms comp->ms
    #define Ws comp->Ws

    #define bk comp->bk
    #define mWin comp->mWin
    ////////////////////////////////////////////////////////////////


    int i,num,numa,numi;
    double dru,ab,dab,R,Q,Ta,vpl;
    double einmWin,ausmWin,zykmWin,aeumWin,innmWin,ref,innref,aeuref;
    double einzei,auszei,zykzei;

    /* does the neutron hit the bender at the entrance? */
    PROP_Z0;
    if ((fabs(x)<w/2) && (fabs(y)<h/2))
    {
        /*** reflections in the XZ-plane ***/

        /* distance between neutron and concave side of the channel at the entrance */
        dru=floor((w/2-x)/bk)*bk;
        ab=w/2.0-x-dru;

        /* radius of the channel */
        R=r-dru;

        /* does the neutron hit the partition at the entrance? */
        if (ab<bk-d)
        {
            double aeu[] = {R0a, Qca, alphaa, ma, Wa};
            /* velocity in the XZ-plane */
            vpl=sqrt(vx*vx+vz*vz);

            /* divergence of the neutron at the entrance */
            einmWin=atan(vx/vz);

            /* maximal distance between neutron and concave side of the channel */
            dab=R-cos(einmWin)*(R-ab);

            /* reflection angle at the concave side */
            aeumWin=acos((R-dab)/R);

            /* reflection coefficient at the concave side */
            Q=2.0*V2K*vpl*sin(aeumWin);
            StdReflecFunc(Q, aeu, &aeuref);

            /* does the neutron hit the convex side of the channel? */
            innmWin=0.0;
            innref=1.0;
            if (dab>bk-d)
            {
                double inn[] = {R0i, Qci, alphai, mi, Wi};
                /* reflection coefficient at the convex side */
                innmWin=acos((R-dab)/(R-bk+d));
                Q=2.0*V2K*vpl*sin(innmWin);
                StdReflecFunc(Q, inn, &innref);
            }

            /* divergence of the neutron at the exit */
            zykmWin=2.0*(aeumWin-innmWin);
            ausmWin=fmod(mWin+einmWin+aeumWin-innmWin
            *(1.0+SIGN(einmWin)),zykmWin)-zykmWin/2.0;
            ausmWin+=innmWin*SIGN(ausmWin);

            /* number of reflections at the concave side */
            numa=(mWin+einmWin+aeumWin-innmWin*(1.0+SIGN(einmWin)))/zykmWin;

            /* number of reflections at the convex side */
            numi=numa;
            if (ausmWin*einmWin<0)
            {
                if (ausmWin-einmWin>0)
                        numi++;
                else
                        numi--;
            }

            /* is the reflection coefficient too small? */
            if (((numa>0) && (aeuref<=0)) || ((numi>0) && (innref<=0)))
            ABSORB;

            /* calculation of the neutron probability weight p */
            for (i=1;i<=numa;i++)
                p*=aeuref;
            for (i=1;i<=numi;i++)
                p*=innref;

            /* time to cross the bender */
            Ta=(2*numa*(tan(aeumWin)-tan(innmWin))
                +tan(ausmWin)-tan(einmWin)
                -tan(innmWin)*(SIGN(ausmWin)-SIGN(einmWin)))
                *(R-dab)/vpl;
            t+=Ta;

            /* distance between neutron and concave side of channel at the exit */
            ab=R-(R-dab)/cos(ausmWin);

            /* calculation of the exit coordinates in the XZ-plane */
            x=w/2.0-ab-dru;
            z=r*mWin;
            vx=sin(ausmWin)*vpl;
            vz=cos(ausmWin)*vpl;

            /*** reflections at top and bottom side (Y axis) ***/

            if (vy!=0.0)
            {
                double s[] = {R0s, Qcs, alphas, ms, Ws};
                /* reflection coefficent at the top and bottom side */
                Q=2.0*V2K*fabs(vy);
                StdReflecFunc(Q, s, &ref);

                /* number of reflections at top and bottom */
                einzei=h/2.0/fabs(vy)+y/vy;
                zykzei=h/fabs(vy);
                num=(Ta+einzei)/zykzei;

                /* time between the last reflection and the exit */
                auszei=fmod(Ta+einzei,zykzei);

                /* is the reflection coefficient too small? */
                if ((num>0) && (ref<=0))
                    ABSORB;

                /* calculation of the probability weight p */
                for (i=1;i<=num;i++) {
                    p*=ref;
                    vy*=-1.0;
                }

                /* calculation of the exit coordinate */
                y=auszei*vy-vy*h/fabs(vy)/2.0;
            } /* if (vy!=0.0) */
            SCATTER;
        } /* if (dab>bk-d)  */

        else {
            ABSORB; /* hit separating walls */
        }
    }
    else /* if ((fabs(x)<w/2) && (fabs(y)<h/2))   */ {
        ABSORB; /* miss entry window */
    }


    ////////////////////////////////////////////////////////////////
    #undef w
    #undef h
    #undef r
    #undef Win
    #undef k
    #undef d
    #undef l
    #undef R0a
    #undef Qca
    #undef alphaa
    #undef ma
    #undef Wa
    #undef R0i
    #undef Qci
    #undef alphai
    #undef mi
    #undef Wi
    #undef R0s
    #undef Qcs
    #undef alphas
    #undef ms
    #undef Ws

    #undef bk
    #undef mWin

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

void Save_Bender(Bender *comp) {

}

void Finally_Bender(Bender *comp) {

}

void Display_Bender(Bender *comp) {
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

    #define w comp->w
    #define h comp->h
    #define r comp->r
    #define Win comp->Win
    #define k comp->k
    #define d comp->d
    #define l comp->l
    #define R0a comp->R0a
    #define Qca comp->Qca
    #define alphaa comp->alphaa
    #define ma comp->ma
    #define Wa comp->Wa
    #define R0i comp->R0i
    #define Qci comp->Qci
    #define alphai comp->alphai
    #define mi comp->mi
    #define Wi comp->Wi
    #define R0s comp->R0s
    #define Qcs comp->Qcs
    #define alphas comp->alphas
    #define ms comp->ms
    #define Ws comp->Ws

    #define bk comp->bk
    #define mWin comp->mWin
    ////////////////////////////////////////////////////////////////


    int i;
    double w1c, w2c, h1, h2, L, w1, w2;

    w1c = (w + d)/(double)k;
    w2c = w1c; h1 = h; h2 = h;
    L = r*mWin; w1 = w; w2 = w;

    
    for(i = 0; i < k; i++)
    {
        multiline(5,
            i*w1c - w1/2.0, -h1/2.0, 0.0,
            i*w2c - w2/2.0, -h2/2.0, (double)L,
            i*w2c - w2/2.0,  h2/2.0, (double)L,
            i*w1c - w1/2.0,  h1/2.0, 0.0,
            i*w1c - w1/2.0, -h1/2.0, 0.0);
        multiline(5,
            (i+1)*w1c - d - w1/2.0, -h1/2.0, 0.0,
            (i+1)*w2c - d - w2/2.0, -h2/2.0, (double)L,
            (i+1)*w2c - d - w2/2.0,  h2/2.0, (double)L,
            (i+1)*w1c - d - w1/2.0,  h1/2.0, 0.0,
            (i+1)*w1c - d - w1/2.0, -h1/2.0, 0.0);
    }
    line(-w1/2.0, -h1/2.0, 0.0, w1/2.0, -h1/2.0, 0.0);
    line(-w2/2.0, -h2/2.0, (double)L, w2/2.0, -h2/2.0, (double)L);


    ////////////////////////////////////////////////////////////////
    #undef w
    #undef h
    #undef r
    #undef Win
    #undef k
    #undef d
    #undef l
    #undef R0a
    #undef Qca
    #undef alphaa
    #undef ma
    #undef Wa
    #undef R0i
    #undef Qci
    #undef alphai
    #undef mi
    #undef Wi
    #undef R0s
    #undef Qcs
    #undef alphas
    #undef ms
    #undef Ws

    #undef bk
    #undef mWin

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

#ifndef __PSI_DMC__
#define __PSI_DMC__


struct PSI_DMC {

    // parameters
    double lambda = 2.5666;
    double R = 0.87;
    double R_curve = 0.87;
    char *filename = (char*) "data/Na2Ca3Al2F14.laz";
    double D_PHI = 6;
    double SHIFT = 0;
    double PACK = 0.7;
    double Dw = 0.8;
    int BARNS = 1;
    int SPLITS = 58;

    // declares
    double mono_q = 1.8734;
    double OMA;
    double RV;
    double y_mono = 0.025;
    double NV = 5;
    double d_phi_0;
    double TTM;
    double sample_radius = 0.008/2;
    double sample_height = 0.03;
    double can_radius = 0.0083/2;
    double can_height = 0.0303;
    double can_thick = 0.00015;
    double alpha;
    double Qc = 0.0217;
    double R0 = 0.995;
    double Mvalue = 1.9;
    double W = 1.0/250.0;
    double alpha_curve;
    double Qc_curve = 0.0217;
    double R0_curve = 0.995;
    double Mvalue_curve = 2.1;
    double W_curve = 1.0/250.0;
    double ldiff = 0.05;
    double angleGuideCurved;
};

void Init_PSI_DMC(PSI_DMC *spec) {

    #define lambda spec->lambda
    #define R spec->R
    #define R_curve spec->R_curve
    #define filename spec->filename
    #define D_PHI spec->D_PHI
    #define SHIFT spec->SHIFT
    #define PACK spec->PACK
    #define Dw spec->Dw
    #define BARNS spec->BARNS
    #define SPLITS spec->SPLITS

    #define mono_q spec->mono_q
    #define OMA spec->OMA
    #define RV spec->RV
    #define y_mono spec->y_mono
    #define NV spec->NV
    #define d_phi_0 spec->d_phi_0
    #define TTM spec->TTM
    #define sample_radius spec->sample_radius
    #define sample_height spec->sample_height
    #define can_radius spec->can_radius
    #define can_height spec->can_height
    #define can_thick spec->can_thick
    #define alpha spec->alpha
    #define Qc spec->Qc
    #define R0 spec->R0
    #define Mvalue spec->Mvalue
    #define W spec->W
    #define alpha_curve spec->alpha_curve
    #define Qc_curve spec->Qc_curve
    #define R0_curve spec->R0_curve
    #define Mvalue_curve spec->Mvalue_curve
    #define W_curve spec->W_curve
    #define ldiff spec->ldiff
    #define angleGuideCurved spec->angleGuideCurved
    ////////////////////////////////////////////////////////////////


  TTM = 2*asin(mono_q*lambda/(4*PI))*RAD2DEG;
  OMA = TTM/2;
  RV = fabs(2*2.82*sin(DEG2RAD*OMA));
  
  angleGuideCurved=-2.0*asin(0.4995 /2.0/3612)/PI*180;
  alpha=(R0-R)/Qc/(Mvalue-1);
  alpha_curve=(R0_curve-R_curve)/Qc_curve/(Mvalue_curve-1);
  


    ////////////////////////////////////////////////////////////////
    #undef lambda
    #undef R
    #undef R_curve
    #undef filename
    #undef D_PHI
    #undef SHIFT
    #undef PACK
    #undef Dw
    #undef BARNS
    #undef SPLITS

    #undef mono_q
    #undef OMA
    #undef RV
    #undef y_mono
    #undef NV
    #undef d_phi_0
    #undef TTM
    #undef sample_radius
    #undef sample_height
    #undef can_radius
    #undef can_height
    #undef can_thick
    #undef alpha
    #undef Qc
    #undef R0
    #undef Mvalue
    #undef W
    #undef alpha_curve
    #undef Qc_curve
    #undef R0_curve
    #undef Mvalue_curve
    #undef W_curve
    #undef ldiff
    #undef angleGuideCurved

}

void Config_PSI_DMC(MArena *a_dest, PSI_DMC *spec, Instrument *instr) {
    s32 index = 0;
    Component *source_arm = CreateComponent(a_dest, CT_Progress_bar, index++, "source_arm");
    Progress_bar *source_arm_comp = (Progress_bar*) source_arm->comp;
    Init_Progress_bar(source_arm_comp, instr);
    // ABSOLUTE
    // AT:  (0, 0, 0)

    Component *source = CreateComponent(a_dest, CT_Source_Maxwell_3, index++, "source");
    Source_Maxwell_3 *source_comp = (Source_Maxwell_3*) source->comp;
    source_comp->yheight = 0.156;
    source_comp->xwidth = 0.126;
    source_comp->Lmin = spec->lambda-spec->ldiff/2;
    source_comp->Lmax = spec->lambda+spec->ldiff/2;
    source_comp->dist = 1.5;
    source_comp->focus_xw = 0.02;
    source_comp->focus_yh = 0.12;
    source_comp->T1 = 296.16;
    source_comp->I1 = 8.5E11;
    source_comp->T2 = 40.68;
    source_comp->I2 = 5.2E11;
    Init_Source_Maxwell_3(source_comp, instr);
    // RELATIVE source_arm
    // AT:  (0, 0, 0)
    // ROT: (0, 0, 0)

    Component *PSDbefore_guides = CreateComponent(a_dest, CT_PSD_monitor, index++, "PSDbefore_guides");
    PSD_monitor *PSDbefore_guides_comp = (PSD_monitor*) PSDbefore_guides->comp;
    PSDbefore_guides_comp->nx = 128;
    PSDbefore_guides_comp->ny = 128;
    PSDbefore_guides_comp->filename = (char*) "PSDbefore_guides";
    PSDbefore_guides_comp->xwidth = 0.02;
    PSDbefore_guides_comp->yheight = 0.12;
    Init_PSD_monitor(PSDbefore_guides_comp, instr);
    // RELATIVE source_arm
    // AT:  (0, 0, 1.49999)

    Component *l_mon_source = CreateComponent(a_dest, CT_L_monitor, index++, "l_mon_source");
    L_monitor *l_mon_source_comp = (L_monitor*) l_mon_source->comp;
    l_mon_source_comp->nL = 101;
    l_mon_source_comp->filename = (char*) "lmonsource.dat";
    l_mon_source_comp->xwidth = 0.02;
    l_mon_source_comp->yheight = 0.12;
    l_mon_source_comp->Lmin = 0;
    l_mon_source_comp->Lmax = 20;
    Init_L_monitor(l_mon_source_comp, instr);
    // RELATIVE PREVIOUS
    // AT:  (0, 0, 1e-9)

    Component *guide1 = CreateComponent(a_dest, CT_Guide, index++, "guide1");
    Guide *guide1_comp = (Guide*) guide1->comp;
    guide1_comp->w1 = 0.02;
    guide1_comp->h1 = 0.12;
    guide1_comp->w2 = 0.02;
    guide1_comp->h2 = 0.12;
    guide1_comp->l = 4.66;
    guide1_comp->R0 = spec->R0;
    guide1_comp->Qc = spec->Qc;
    guide1_comp->alpha = spec->alpha;
    guide1_comp->m = 1.8;
    guide1_comp->W = spec->W;
    Init_Guide(guide1_comp, instr);
    // RELATIVE source_arm
    // AT:  (0, 0, 1.50)
    // ROT: (0, 0, 0)

    Component *PSDbefore_curve = CreateComponent(a_dest, CT_PSD_monitor, index++, "PSDbefore_curve");
    PSD_monitor *PSDbefore_curve_comp = (PSD_monitor*) PSDbefore_curve->comp;
    PSDbefore_curve_comp->nx = 128;
    PSDbefore_curve_comp->ny = 128;
    PSDbefore_curve_comp->filename = (char*) "PSDbefore_curve";
    PSDbefore_curve_comp->xwidth = 0.02;
    PSDbefore_curve_comp->yheight = 0.12;
    Init_PSD_monitor(PSDbefore_curve_comp, instr);
    // RELATIVE guide1
    // AT:  (0, 0, 4.664)

    Component *guide2 = CreateComponent(a_dest, CT_Bender, index++, "guide2");
    Bender *guide2_comp = (Bender*) guide2->comp;
    guide2_comp->w = 0.02;
    guide2_comp->h = 0.12;
    guide2_comp->r = 3612;
    guide2_comp->R0a = spec->R0_curve;
    guide2_comp->Qca = spec->Qc_curve;
    guide2_comp->alphaa = spec->alpha_curve;
    guide2_comp->ma = spec->Mvalue_curve;
    guide2_comp->Wa = spec->W_curve;
    guide2_comp->R0i = spec->R0_curve;
    guide2_comp->Qci = spec->Qc_curve;
    guide2_comp->alphai = spec->alpha_curve;
    guide2_comp->mi = 1;
    guide2_comp->Wi = spec->W_curve;
    guide2_comp->R0s = spec->R0_curve;
    guide2_comp->Qcs = spec->Qc_curve;
    guide2_comp->alphas = spec->alpha_curve;
    guide2_comp->ms = spec->Mvalue_curve;
    guide2_comp->Ws = spec->W_curve;
    guide2_comp->l = 20;
    Init_Bender(guide2_comp, instr);
    // RELATIVE guide1
    // AT:  (0, 0, 4.69)

    Component *PSDafter_curve = CreateComponent(a_dest, CT_PSD_monitor, index++, "PSDafter_curve");
    PSD_monitor *PSDafter_curve_comp = (PSD_monitor*) PSDafter_curve->comp;
    PSDafter_curve_comp->nx = 128;
    PSDafter_curve_comp->ny = 128;
    PSDafter_curve_comp->filename = (char*) "PSDafter_curve";
    PSDafter_curve_comp->xwidth = 0.02;
    PSDafter_curve_comp->yheight = 0.12;
    Init_PSD_monitor(PSDafter_curve_comp, instr);
    // RELATIVE guide2
    // AT:  (0, 0, 20.0001)

    Component *bunker = CreateComponent(a_dest, CT_Guide, index++, "bunker");
    Guide *bunker_comp = (Guide*) bunker->comp;
    bunker_comp->w1 = 0.02;
    bunker_comp->h1 = .12;
    bunker_comp->w2 = 0.02;
    bunker_comp->h2 = .12;
    bunker_comp->l = 3.43;
    bunker_comp->R0 = spec->R0;
    bunker_comp->Qc = spec->Qc;
    bunker_comp->alpha = spec->alpha;
    bunker_comp->m = 1.6;
    bunker_comp->W = spec->W;
    Init_Guide(bunker_comp, instr);
    // RELATIVE guide2
    // AT:  (0, 0, 20.1502)
    // ROT: (0, 0, 0)

    Component *guide3 = CreateComponent(a_dest, CT_Guide, index++, "guide3");
    Guide *guide3_comp = (Guide*) guide3->comp;
    guide3_comp->w1 = 0.02;
    guide3_comp->h1 = .12;
    guide3_comp->w2 = 0.02;
    guide3_comp->h2 = .12;
    guide3_comp->l = 12.275;
    guide3_comp->R0 = spec->R0;
    guide3_comp->Qc = spec->Qc;
    guide3_comp->alpha = spec->alpha;
    guide3_comp->m = 1.6;
    guide3_comp->W = spec->W;
    Init_Guide(guide3_comp, instr);
    // RELATIVE bunker
    // AT:  (0, 0, 3.56)
    // ROT: (0, 0, 0)

    Component *guide4 = CreateComponent(a_dest, CT_Guide, index++, "guide4");
    Guide *guide4_comp = (Guide*) guide4->comp;
    guide4_comp->w1 = 0.02;
    guide4_comp->h1 = .12;
    guide4_comp->w2 = 0.02;
    guide4_comp->h2 = .12;
    guide4_comp->l = 5.66;
    guide4_comp->R0 = spec->R0;
    guide4_comp->Qc = spec->Qc;
    guide4_comp->alpha = spec->alpha;
    guide4_comp->m = 1.6;
    guide4_comp->W = spec->W;
    Init_Guide(guide4_comp, instr);
    // RELATIVE bunker
    // AT:  (0, 0, 15.8555)
    // ROT: (0, 0, 0)

    Component *window1 = CreateComponent(a_dest, CT_Al_window, index++, "window1");
    Al_window *window1_comp = (Al_window*) window1->comp;
    window1_comp->thickness = 0.002;
    Init_Al_window(window1_comp, instr);
    // RELATIVE PREVIOUS
    // AT:  (0, 0, 5.66+1e-9)

    Component *ydist_fluxpos = CreateComponent(a_dest, CT_PSDlin_monitor, index++, "ydist_fluxpos");
    PSDlin_monitor *ydist_fluxpos_comp = (PSDlin_monitor*) ydist_fluxpos->comp;
    ydist_fluxpos_comp->nbins = 11;
    ydist_fluxpos_comp->filename = (char*) "ydist_fluxpos.dat";
    ydist_fluxpos_comp->xwidth = 0.120;
    ydist_fluxpos_comp->yheight = 0.02;
    Init_PSDlin_monitor(ydist_fluxpos_comp, instr);
    // RELATIVE guide4
    // AT:  (0, 0, 5.66+1e-8+0.01)
    // ROT: (0, 0, 90)

    Component *PSD_fluxpos = CreateComponent(a_dest, CT_PSD_monitor, index++, "PSD_fluxpos");
    PSD_monitor *PSD_fluxpos_comp = (PSD_monitor*) PSD_fluxpos->comp;
    PSD_fluxpos_comp->nx = 100;
    PSD_fluxpos_comp->ny = 100;
    PSD_fluxpos_comp->filename = (char*) "xdist_fluxposy.dat";
    PSD_fluxpos_comp->xwidth = 0.02;
    PSD_fluxpos_comp->yheight = 0.12;
    Init_PSD_monitor(PSD_fluxpos_comp, instr);
    // RELATIVE guide4
    // AT:  (0, 0, 5.66+1e-7+0.01)

    Component *xdist_flux_pos = CreateComponent(a_dest, CT_PSDlin_monitor, index++, "xdist_flux_pos");
    PSDlin_monitor *xdist_flux_pos_comp = (PSDlin_monitor*) xdist_flux_pos->comp;
    xdist_flux_pos_comp->nbins = 11;
    xdist_flux_pos_comp->filename = (char*) "xdist_fluxpos.dat";
    xdist_flux_pos_comp->xwidth = 0.020;
    xdist_flux_pos_comp->yheight = 0.12;
    Init_PSDlin_monitor(xdist_flux_pos_comp, instr);
    // RELATIVE PREVIOUS
    // AT:  (0, 0, 1e-9)

    Component *PSD_fluxposB = CreateComponent(a_dest, CT_PSD_monitor, index++, "PSD_fluxposB");
    PSD_monitor *PSD_fluxposB_comp = (PSD_monitor*) PSD_fluxposB->comp;
    PSD_fluxposB_comp->nx = 100;
    PSD_fluxposB_comp->ny = 100;
    PSD_fluxposB_comp->filename = (char*) "PSD_fluxposB.dat";
    PSD_fluxposB_comp->xwidth = 0.02;
    PSD_fluxposB_comp->yheight = 0.12;
    Init_PSD_monitor(PSD_fluxposB_comp, instr);
    // RELATIVE guide4
    // AT:  (0, 0, 6.24-1e-7-0.01)

    Component *window2 = CreateComponent(a_dest, CT_Al_window, index++, "window2");
    Al_window *window2_comp = (Al_window*) window2->comp;
    window2_comp->thickness = 0.002;
    Init_Al_window(window2_comp, instr);
    // RELATIVE PREVIOUS
    // AT:  (0, 0, 1e-9)

    Component *in_slit = CreateComponent(a_dest, CT_Slit, index++, "in_slit");
    Slit *in_slit_comp = (Slit*) in_slit->comp;
    in_slit_comp->xmin = -0.01;
    in_slit_comp->xmax = 0.01 ;
    in_slit_comp->ymin = -0.06;
    in_slit_comp->ymax = 0.06;
    Init_Slit(in_slit_comp, instr);
    // RELATIVE PREVIOUS
    // AT:  (0, 0, 0.0021)

    Component *lambda_in = CreateComponent(a_dest, CT_L_monitor, index++, "lambda_in");
    L_monitor *lambda_in_comp = (L_monitor*) lambda_in->comp;
    lambda_in_comp->xmin = -0.011;
    lambda_in_comp->xmax = 0.011;
    lambda_in_comp->ymin = -0.061;
    lambda_in_comp->ymax = 0.061;
    lambda_in_comp->Lmin = 0;
    lambda_in_comp->Lmax = 2*spec->lambda;
    lambda_in_comp->nL = 128;
    lambda_in_comp->filename = (char*) "L_in.dat";
    Init_L_monitor(lambda_in_comp, instr);
    // RELATIVE in_slit
    // AT:  (0, 0, 0.001)

    Component *sma = CreateComponent(a_dest, CT_Arm, index++, "sma");
    Arm *sma_comp = (Arm*) sma->comp;
    Init_Arm(sma_comp, instr);
    // RELATIVE in_slit
    // AT:  (0, 0, 0.65)
    // ROT: (0, OMA, 0)

    Component *foc_mono = CreateComponent(a_dest, CT_Monochromator_2foc, index++, "foc_mono");
    Monochromator_2foc *foc_mono_comp = (Monochromator_2foc*) foc_mono->comp;
    foc_mono_comp->zwidth = 0.05;
    foc_mono_comp->yheight = 0.025;
    foc_mono_comp->gap = 0.0005;
    foc_mono_comp->NH = 1;
    foc_mono_comp->NV = 5;
    foc_mono_comp->mosaich = 38;
    foc_mono_comp->mosaicv = 38;
    foc_mono_comp->r0 = 0.7;
    foc_mono_comp->Q = spec->mono_q;
    foc_mono_comp->RV = spec->RV;
    foc_mono_comp->RH = 0;
    Init_Monochromator_2foc(foc_mono_comp, instr);
    // RELATIVE sma
    // AT:  (0, 0, 0)

    Component *msa = CreateComponent(a_dest, CT_Arm, index++, "msa");
    Arm *msa_comp = (Arm*) msa->comp;
    Init_Arm(msa_comp, instr);
    // RELATIVE sma
    // AT:  (0, 0, 0)
    // ROT: (0, TTM, 0)

    Component *out1_slit = CreateComponent(a_dest, CT_Slit, index++, "out1_slit");
    Slit *out1_slit_comp = (Slit*) out1_slit->comp;
    out1_slit_comp->xmin = -0.01;
    out1_slit_comp->xmax = 0.01;
    out1_slit_comp->ymin = -0.06;
    out1_slit_comp->ymax = 0.06;
    Init_Slit(out1_slit_comp, instr);
    // RELATIVE msa
    // AT:  (0, 0, 0.2)
    // ROT: (0, 0, 0)

    Component *Amoin_slit = CreateComponent(a_dest, CT_Slit, index++, "Amoin_slit");
    Slit *Amoin_slit_comp = (Slit*) Amoin_slit->comp;
    Amoin_slit_comp->xmin = -0.01;
    Amoin_slit_comp->xmax = 0.01;
    Amoin_slit_comp->ymin = -0.06;
    Amoin_slit_comp->ymax = 0.06;
    Init_Slit(Amoin_slit_comp, instr);
    // RELATIVE msa
    // AT:  (0, 0, 0.325)
    // ROT: (0, 0, 0)

    Component *Bmoin_slit = CreateComponent(a_dest, CT_Slit, index++, "Bmoin_slit");
    Slit *Bmoin_slit_comp = (Slit*) Bmoin_slit->comp;
    Bmoin_slit_comp->xmin = -0.01;
    Bmoin_slit_comp->xmax = 0.01;
    Bmoin_slit_comp->ymin = -0.06;
    Bmoin_slit_comp->ymax = 0.06;
    Init_Slit(Bmoin_slit_comp, instr);
    // RELATIVE msa
    // AT:  (0, 0, 0.525)
    // ROT: (0, 0, 0)

    Component *out2_slit = CreateComponent(a_dest, CT_Slit, index++, "out2_slit");
    Slit *out2_slit_comp = (Slit*) out2_slit->comp;
    out2_slit_comp->xmin = -0.01;
    out2_slit_comp->xmax = 0.01;
    out2_slit_comp->ymin = -0.06;
    out2_slit_comp->ymax = 0.06;
    Init_Slit(out2_slit_comp, instr);
    // RELATIVE msa
    // AT:  (0, 0, 0.65)
    // ROT: (0, 0, 0)

    Component *PSD_sample = CreateComponent(a_dest, CT_PSD_monitor, index++, "PSD_sample");
    PSD_monitor *PSD_sample_comp = (PSD_monitor*) PSD_sample->comp;
    PSD_sample_comp->xmin = -0.05;
    PSD_sample_comp->xmax = 0.05;
    PSD_sample_comp->ymin = -0.07;
    PSD_sample_comp->ymax = 0.07;
    PSD_sample_comp->nx = 80;
    PSD_sample_comp->ny = 80;
    PSD_sample_comp->filename = (char*) "PSD_sample.dat";
    Init_PSD_monitor(PSD_sample_comp, instr);
    // RELATIVE msa
    // AT:  (0, 0, 2.77)

    Component *lambda_sample = CreateComponent(a_dest, CT_L_monitor, index++, "lambda_sample");
    L_monitor *lambda_sample_comp = (L_monitor*) lambda_sample->comp;
    lambda_sample_comp->xmin = -spec->sample_radius;
    lambda_sample_comp->xmax = spec->sample_radius;
    lambda_sample_comp->ymin = -spec->sample_height/2;
    lambda_sample_comp->ymax = spec->sample_height/2;
    lambda_sample_comp->Lmin = spec->lambda-0.2;
    lambda_sample_comp->Lmax = spec->lambda+0.2;
    lambda_sample_comp->nL = 128;
    lambda_sample_comp->filename = (char*) "L_sample.dat";
    Init_L_monitor(lambda_sample_comp, instr);
    // RELATIVE msa
    // AT:  (0, 0, 2.81)

    Component *sa_arm = CreateComponent(a_dest, CT_Arm, index++, "sa_arm");
    Arm *sa_arm_comp = (Arm*) sa_arm->comp;
    Init_Arm(sa_arm_comp, instr);
    // RELATIVE msa
    // AT:  (0, 0, 2.82)
    // ROT: (0, 0, 0)

    Component *sample = CreateComponent(a_dest, CT_PowderN, index++, "sample");
    PowderN *sample_comp = (PowderN*) sample->comp;
    sample_comp->d_phi = spec->D_PHI;
    sample_comp->radius = spec->sample_radius;
    sample_comp->yheight = spec->sample_height;
    sample_comp->DW = spec->Dw;
    sample_comp->pack = spec->PACK;
    sample_comp->reflections = spec->filename;
    sample_comp->barns = spec->BARNS;
    sample_comp->p_transmit = 0;
    sample_comp->p_inc = 0;
    Init_PowderN(sample_comp, instr);
    // RELATIVE sa_arm
    // AT:  (0, 0, 0)

    Component *STOP = CreateComponent(a_dest, CT_Beamstop, index++, "STOP");
    Beamstop *STOP_comp = (Beamstop*) STOP->comp;
    STOP_comp->radius = 0.3;
    Init_Beamstop(STOP_comp, instr);
    // RELATIVE sa_arm
    // AT:  (0, 0, 1.4)
    // ROT: (0, 0, 0)

    Component *Detector = CreateComponent(a_dest, CT_Monitor_nD, index++, "Detector");
    Monitor_nD *Detector_comp = (Monitor_nD*) Detector->comp;
    Detector_comp->xwidth = 3.0;
    Detector_comp->yheight = 0.09;
    Detector_comp->filename = (char*) "detector.dat";
    Detector_comp->min = 19.9+spec->SHIFT;
    Detector_comp->max = 99.9+spec->SHIFT;
    Detector_comp->bins = 400;
    Detector_comp->options = (char*) "banana, theta";
    Init_Monitor_nD(Detector_comp, instr);
    // RELATIVE sa_arm
    // AT:  (0, 0, 0)
    // ROT: (0, 0, 180)

}

#endif // PSI_DMC

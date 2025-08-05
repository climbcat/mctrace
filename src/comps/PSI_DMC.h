#ifndef __PSI_DMC__
#define __PSI_DMC__


#include "meta_comps.h"


struct PSI_DMC {

    // parameters
    double lambda = 2.5666;
    double R = 0.87;
    double R_curve = 0.87;
    char *filename = (char*) "Na2Ca3Al2F14.laz";
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

void Config_PSI_DMC(PSI_DMC *spec, Instrument *instr) {

    Progress_bar source_arm = Create_Progress_bar(0, (char*) "source_arm");
    Init_Progress_bar(&source_arm, instr);

    Source_Maxwell_3 source = Create_Source_Maxwell_3(1, (char*) "source");
    source.yheight = 0.156;
    source.xwidth = 0.126;
    source.Lmin = spec->lambda-spec->ldiff/2;
    source.Lmax = spec->lambda+spec->ldiff/2;
    source.dist = 1.5;
    source.focus_xw = 0.02;
    source.focus_yh = 0.12;
    source.T1 = 296.16;
    source.I1 = 8.5E11;
    source.T2 = 40.68;
    source.I2 = 5.2E11;
    Init_Source_Maxwell_3(&source, instr);

    PSD_monitor PSDbefore_guides = Create_PSD_monitor(2, (char*) "PSDbefore_guides");
    PSDbefore_guides.nx = 128;
    PSDbefore_guides.ny = 128;
    PSDbefore_guides.filename = (char*) "PSDbefore_guides";
    PSDbefore_guides.xwidth = 0.02;
    PSDbefore_guides.yheight = 0.12;
    Init_PSD_monitor(&PSDbefore_guides, instr);

    L_monitor l_mon_source = Create_L_monitor(3, (char*) "l_mon_source");
    l_mon_source.nL = 101;
    l_mon_source.filename = (char*) "lmonsource.dat";
    l_mon_source.xwidth = 0.02;
    l_mon_source.yheight = 0.12;
    l_mon_source.Lmin = 0;
    l_mon_source.Lmax = 20;
    Init_L_monitor(&l_mon_source, instr);

    Guide guide1 = Create_Guide(4, (char*) "guide1");
    guide1.w1 = 0.02;
    guide1.h1 = 0.12;
    guide1.w2 = 0.02;
    guide1.h2 = 0.12;
    guide1.l = 4.66;
    guide1.R0 = spec->R0;
    guide1.Qc = spec->Qc;
    guide1.alpha = spec->alpha;
    guide1.m = 1.8;
    guide1.W = spec->W;
    Init_Guide(&guide1, instr);

    PSD_monitor PSDbefore_curve = Create_PSD_monitor(5, (char*) "PSDbefore_curve");
    PSDbefore_curve.nx = 128;
    PSDbefore_curve.ny = 128;
    PSDbefore_curve.filename = (char*) "PSDbefore_curve";
    PSDbefore_curve.xwidth = 0.02;
    PSDbefore_curve.yheight = 0.12;
    Init_PSD_monitor(&PSDbefore_curve, instr);

    Bender guide2 = Create_Bender(6, (char*) "guide2");
    guide2.w = 0.02;
    guide2.h = 0.12;
    guide2.r = 3612;
    guide2.R0a = spec->R0_curve;
    guide2.Qca = spec->Qc_curve;
    guide2.alphaa = spec->alpha_curve;
    guide2.ma = spec->Mvalue_curve;
    guide2.Wa = spec->W_curve;
    guide2.R0i = spec->R0_curve;
    guide2.Qci = spec->Qc_curve;
    guide2.alphai = spec->alpha_curve;
    guide2.mi = 1;
    guide2.Wi = spec->W_curve;
    guide2.R0s = spec->R0_curve;
    guide2.Qcs = spec->Qc_curve;
    guide2.alphas = spec->alpha_curve;
    guide2.ms = spec->Mvalue_curve;
    guide2.Ws = spec->W_curve;
    guide2.l = 20;
    Init_Bender(&guide2, instr);

    PSD_monitor PSDafter_curve = Create_PSD_monitor(7, (char*) "PSDafter_curve");
    PSDafter_curve.nx = 128;
    PSDafter_curve.ny = 128;
    PSDafter_curve.filename = (char*) "PSDafter_curve";
    PSDafter_curve.xwidth = 0.02;
    PSDafter_curve.yheight = 0.12;
    Init_PSD_monitor(&PSDafter_curve, instr);

    Guide bunker = Create_Guide(8, (char*) "bunker");
    bunker.w1 = 0.02;
    bunker.h1 = .12;
    bunker.w2 = 0.02;
    bunker.h2 = .12;
    bunker.l = 3.43;
    bunker.R0 = spec->R0;
    bunker.Qc = spec->Qc;
    bunker.alpha = spec->alpha;
    bunker.m = 1.6;
    bunker.W = spec->W;
    Init_Guide(&bunker, instr);

    Guide guide3 = Create_Guide(9, (char*) "guide3");
    guide3.w1 = 0.02;
    guide3.h1 = .12;
    guide3.w2 = 0.02;
    guide3.h2 = .12;
    guide3.l = 12.275;
    guide3.R0 = spec->R0;
    guide3.Qc = spec->Qc;
    guide3.alpha = spec->alpha;
    guide3.m = 1.6;
    guide3.W = spec->W;
    Init_Guide(&guide3, instr);

    Guide guide4 = Create_Guide(10, (char*) "guide4");
    guide4.w1 = 0.02;
    guide4.h1 = .12;
    guide4.w2 = 0.02;
    guide4.h2 = .12;
    guide4.l = 5.66;
    guide4.R0 = spec->R0;
    guide4.Qc = spec->Qc;
    guide4.alpha = spec->alpha;
    guide4.m = 1.6;
    guide4.W = spec->W;
    Init_Guide(&guide4, instr);

    Al_window window1 = Create_Al_window(11, (char*) "window1");
    window1.thickness = 0.002;
    Init_Al_window(&window1, instr);

    PSDlin_monitor ydist_fluxpos = Create_PSDlin_monitor(12, (char*) "ydist_fluxpos");
    ydist_fluxpos.nbins = 11;
    ydist_fluxpos.filename = (char*) "ydist_fluxpos.dat";
    ydist_fluxpos.xwidth = 0.120;
    ydist_fluxpos.yheight = 0.02;
    Init_PSDlin_monitor(&ydist_fluxpos, instr);

    PSD_monitor PSD_fluxpos = Create_PSD_monitor(13, (char*) "PSD_fluxpos");
    PSD_fluxpos.nx = 100;
    PSD_fluxpos.ny = 100;
    PSD_fluxpos.filename = (char*) "xdist_fluxposy.dat";
    PSD_fluxpos.xwidth = 0.02;
    PSD_fluxpos.yheight = 0.12;
    Init_PSD_monitor(&PSD_fluxpos, instr);

    PSDlin_monitor xdist_flux_pos = Create_PSDlin_monitor(14, (char*) "xdist_flux_pos");
    xdist_flux_pos.nbins = 11;
    xdist_flux_pos.filename = (char*) "xdist_fluxpos.dat";
    xdist_flux_pos.xwidth = 0.020;
    xdist_flux_pos.yheight = 0.12;
    Init_PSDlin_monitor(&xdist_flux_pos, instr);

    PSD_monitor PSD_fluxposB = Create_PSD_monitor(15, (char*) "PSD_fluxposB");
    PSD_fluxposB.nx = 100;
    PSD_fluxposB.ny = 100;
    PSD_fluxposB.filename = (char*) "PSD_fluxposB.dat";
    PSD_fluxposB.xwidth = 0.02;
    PSD_fluxposB.yheight = 0.12;
    Init_PSD_monitor(&PSD_fluxposB, instr);

    Al_window window2 = Create_Al_window(16, (char*) "window2");
    window2.thickness = 0.002;
    Init_Al_window(&window2, instr);

    Slit in_slit = Create_Slit(17, (char*) "in_slit");
    in_slit.xmin = -0.01;
    in_slit.xmax = 0.01 ;
    in_slit.ymin = -0.06;
    in_slit.ymax = 0.06;
    Init_Slit(&in_slit, instr);

    L_monitor lambda_in = Create_L_monitor(18, (char*) "lambda_in");
    lambda_in.xmin = -0.011;
    lambda_in.xmax = 0.011;
    lambda_in.ymin = -0.061;
    lambda_in.ymax = 0.061;
    lambda_in.Lmin = 0;
    lambda_in.Lmax = 2*spec->lambda;
    lambda_in.nL = 128;
    lambda_in.filename = (char*) "L_in.dat";
    Init_L_monitor(&lambda_in, instr);

    Arm sma = Create_Arm(19, (char*) "sma");
    Init_Arm(&sma, instr);

    Monochromator_2foc foc_mono = Create_Monochromator_2foc(20, (char*) "foc_mono");
    foc_mono.zwidth = 0.05;
    foc_mono.yheight = 0.025;
    foc_mono.gap = 0.0005;
    foc_mono.NH = 1;
    foc_mono.NV = 5;
    foc_mono.mosaich = 38;
    foc_mono.mosaicv = 38;
    foc_mono.r0 = 0.7;
    foc_mono.Q = spec->mono_q;
    foc_mono.RV = spec->RV;
    foc_mono.RH = 0;
    Init_Monochromator_2foc(&foc_mono, instr);

    Arm msa = Create_Arm(21, (char*) "msa");
    Init_Arm(&msa, instr);

    Slit out1_slit = Create_Slit(22, (char*) "out1_slit");
    out1_slit.xmin = -0.01;
    out1_slit.xmax = 0.01;
    out1_slit.ymin = -0.06;
    out1_slit.ymax = 0.06;
    Init_Slit(&out1_slit, instr);

    Slit Amoin_slit = Create_Slit(23, (char*) "Amoin_slit");
    Amoin_slit.xmin = -0.01;
    Amoin_slit.xmax = 0.01;
    Amoin_slit.ymin = -0.06;
    Amoin_slit.ymax = 0.06;
    Init_Slit(&Amoin_slit, instr);

    Slit Bmoin_slit = Create_Slit(24, (char*) "Bmoin_slit");
    Bmoin_slit.xmin = -0.01;
    Bmoin_slit.xmax = 0.01;
    Bmoin_slit.ymin = -0.06;
    Bmoin_slit.ymax = 0.06;
    Init_Slit(&Bmoin_slit, instr);

    Slit out2_slit = Create_Slit(25, (char*) "out2_slit");
    out2_slit.xmin = -0.01;
    out2_slit.xmax = 0.01;
    out2_slit.ymin = -0.06;
    out2_slit.ymax = 0.06;
    Init_Slit(&out2_slit, instr);

    PSD_monitor PSD_sample = Create_PSD_monitor(26, (char*) "PSD_sample");
    PSD_sample.xmin = -0.05;
    PSD_sample.xmax = 0.05;
    PSD_sample.ymin = -0.07;
    PSD_sample.ymax = 0.07;
    PSD_sample.nx = 80;
    PSD_sample.ny = 80;
    PSD_sample.filename = (char*) "PSD_sample.dat";
    Init_PSD_monitor(&PSD_sample, instr);

    L_monitor lambda_sample = Create_L_monitor(27, (char*) "lambda_sample");
    lambda_sample.xmin = -spec->sample_radius;
    lambda_sample.xmax = spec->sample_radius;
    lambda_sample.ymin = -spec->sample_height/2;
    lambda_sample.ymax = spec->sample_height/2;
    lambda_sample.Lmin = spec->lambda-0.2;
    lambda_sample.Lmax = spec->lambda+0.2;
    lambda_sample.nL = 128;
    lambda_sample.filename = (char*) "L_sample.dat";
    Init_L_monitor(&lambda_sample, instr);

    Arm sa_arm = Create_Arm(28, (char*) "sa_arm");
    Init_Arm(&sa_arm, instr);

    PowderN sample = Create_PowderN(29, (char*) "sample");
    sample.d_phi = spec->D_PHI;
    sample.radius = spec->sample_radius;
    sample.yheight = spec->sample_height;
    sample.DW = spec->Dw;
    sample.pack = spec->PACK;
    sample.reflections = spec->filename;
    sample.barns = spec->BARNS;
    sample.p_transmit = 0;
    sample.p_inc = 0;
    Init_PowderN(&sample, instr);

    Beamstop STOP = Create_Beamstop(30, (char*) "STOP");
    STOP.radius = 0.3;
    Init_Beamstop(&STOP, instr);

    Monitor_nD Detector = Create_Monitor_nD(31, (char*) "Detector");
    Detector.xwidth = 3.0;
    Detector.yheight = 0.09;
    Detector.filename = (char*) "detector.dat";
    Detector.min = 19.9+spec->SHIFT;
    Detector.max = 99.9+spec->SHIFT;
    Detector.bins = 400;
    Detector.options = (char*) "banana, theta";
    Init_Monitor_nD(&Detector, instr);
}

#endif // PSI_DMC

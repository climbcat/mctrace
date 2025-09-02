#ifndef __COMPS_META___
#define __COMPS_META___


#include "comps/Slit.h"
#include "comps/L_monitor.h"
#include "comps/Bender.h"
#include "comps/Progress_bar.h"
#include "comps/PSD_monitor.h"
#include "comps/Arm.h"
#include "comps/Al_window.h"
#include "comps/PSDlin_monitor.h"
#include "comps/Guide.h"
#include "comps/Source_Maxwell_3.h"
#include "comps/Beamstop.h"
#include "comps/PowderN.h"
#include "comps/Monitor_nD.h"
#include "comps/Monochromator_2foc.h"


enum CompType {
    CT_UNDEF,

    CT_Slit,
    CT_L_monitor,
    CT_Bender,
    CT_Progress_bar,
    CT_PSD_monitor,
    CT_Arm,
    CT_Al_window,
    CT_PSDlin_monitor,
    CT_Guide,
    CT_Source_Maxwell_3,
    CT_Beamstop,
    CT_PowderN,
    CT_Monitor_nD,
    CT_Monochromator_2foc,

    CT_CNT
};


enum CompCategory {
    CCAT_UNDEF,

    CCAT_sources,
    CCAT_monitors,
    CCAT_contrib,
    CCAT_misc,
    CCAT_optics,
    CCAT_samples,
    
    CCAT_CNT
};


Component *CreateComponent(MArena *a_dest, CompType type, s32 index, const char *name) {
    Component *comp = (Component*) ArenaAlloc(a_dest, sizeof(Component));
    comp->type = type;

    switch (type) {
        case CT_Slit: {
            Slit comp_spec = Create_Slit(index, (char*) name);
            comp->comp = ArenaPush(a_dest, &comp_spec, sizeof(Slit));
            comp->type_name = StrL(comp_spec.type);
            comp->name = StrL(comp_spec.name);
            comp->cat = CCAT_optics;
        } break;

        case CT_L_monitor: {
            L_monitor comp_spec = Create_L_monitor(index, (char*) name);
            comp->comp = ArenaPush(a_dest, &comp_spec, sizeof(L_monitor));
            comp->type_name = StrL(comp_spec.type);
            comp->name = StrL(comp_spec.name);
            comp->cat = CCAT_monitors;
        } break;

        case CT_Bender: {
            Bender comp_spec = Create_Bender(index, (char*) name);
            comp->comp = ArenaPush(a_dest, &comp_spec, sizeof(Bender));
            comp->type_name = StrL(comp_spec.type);
            comp->name = StrL(comp_spec.name);
            comp->cat = CCAT_optics;
        } break;

        case CT_Progress_bar: {
            Progress_bar comp_spec = Create_Progress_bar(index, (char*) name);
            comp->comp = ArenaPush(a_dest, &comp_spec, sizeof(Progress_bar));
            comp->type_name = StrL(comp_spec.type);
            comp->name = StrL(comp_spec.name);
            comp->cat = CCAT_misc;
        } break;

        case CT_PSD_monitor: {
            PSD_monitor comp_spec = Create_PSD_monitor(index, (char*) name);
            comp->comp = ArenaPush(a_dest, &comp_spec, sizeof(PSD_monitor));
            comp->type_name = StrL(comp_spec.type);
            comp->name = StrL(comp_spec.name);
            comp->cat = CCAT_monitors;
        } break;

        case CT_Arm: {
            Arm comp_spec = Create_Arm(index, (char*) name);
            comp->comp = ArenaPush(a_dest, &comp_spec, sizeof(Arm));
            comp->type_name = StrL(comp_spec.type);
            comp->name = StrL(comp_spec.name);
            comp->cat = CCAT_optics;
        } break;

        case CT_Al_window: {
            Al_window comp_spec = Create_Al_window(index, (char*) name);
            comp->comp = ArenaPush(a_dest, &comp_spec, sizeof(Al_window));
            comp->type_name = StrL(comp_spec.type);
            comp->name = StrL(comp_spec.name);
            comp->cat = CCAT_contrib;
        } break;

        case CT_PSDlin_monitor: {
            PSDlin_monitor comp_spec = Create_PSDlin_monitor(index, (char*) name);
            comp->comp = ArenaPush(a_dest, &comp_spec, sizeof(PSDlin_monitor));
            comp->type_name = StrL(comp_spec.type);
            comp->name = StrL(comp_spec.name);
            comp->cat = CCAT_monitors;
        } break;

        case CT_Guide: {
            Guide comp_spec = Create_Guide(index, (char*) name);
            comp->comp = ArenaPush(a_dest, &comp_spec, sizeof(Guide));
            comp->type_name = StrL(comp_spec.type);
            comp->name = StrL(comp_spec.name);
            comp->cat = CCAT_optics;
        } break;

        case CT_Source_Maxwell_3: {
            Source_Maxwell_3 comp_spec = Create_Source_Maxwell_3(index, (char*) name);
            comp->comp = ArenaPush(a_dest, &comp_spec, sizeof(Source_Maxwell_3));
            comp->type_name = StrL(comp_spec.type);
            comp->name = StrL(comp_spec.name);
            comp->cat = CCAT_sources;
        } break;

        case CT_Beamstop: {
            Beamstop comp_spec = Create_Beamstop(index, (char*) name);
            comp->comp = ArenaPush(a_dest, &comp_spec, sizeof(Beamstop));
            comp->type_name = StrL(comp_spec.type);
            comp->name = StrL(comp_spec.name);
            comp->cat = CCAT_optics;
        } break;

        case CT_PowderN: {
            PowderN comp_spec = Create_PowderN(index, (char*) name);
            comp->comp = ArenaPush(a_dest, &comp_spec, sizeof(PowderN));
            comp->type_name = StrL(comp_spec.type);
            comp->name = StrL(comp_spec.name);
            comp->cat = CCAT_samples;
        } break;

        case CT_Monitor_nD: {
            Monitor_nD comp_spec = Create_Monitor_nD(index, (char*) name);
            comp->comp = ArenaPush(a_dest, &comp_spec, sizeof(Monitor_nD));
            comp->type_name = StrL(comp_spec.type);
            comp->name = StrL(comp_spec.name);
            comp->cat = CCAT_monitors;
        } break;

        case CT_Monochromator_2foc: {
            Monochromator_2foc comp_spec = Create_Monochromator_2foc(index, (char*) name);
            comp->comp = ArenaPush(a_dest, &comp_spec, sizeof(Monochromator_2foc));
            comp->type_name = StrL(comp_spec.type);
            comp->name = StrL(comp_spec.name);
            comp->cat = CCAT_contrib;
        } break;

        default: { } break;
    }

    return comp;
}


void InitComponent(Component *comp, Instrument *instr = NULL) {
    switch (comp->type) {
        case CT_Slit: { Init_Slit((Slit*) comp->comp, instr); } break;
        case CT_L_monitor: { Init_L_monitor((L_monitor*) comp->comp, instr); } break;
        case CT_Bender: { Init_Bender((Bender*) comp->comp, instr); } break;
        case CT_Progress_bar: { Init_Progress_bar((Progress_bar*) comp->comp, instr); } break;
        case CT_PSD_monitor: { Init_PSD_monitor((PSD_monitor*) comp->comp, instr); } break;
        case CT_Arm: { Init_Arm((Arm*) comp->comp, instr); } break;
        case CT_Al_window: { Init_Al_window((Al_window*) comp->comp, instr); } break;
        case CT_PSDlin_monitor: { Init_PSDlin_monitor((PSDlin_monitor*) comp->comp, instr); } break;
        case CT_Guide: { Init_Guide((Guide*) comp->comp, instr); } break;
        case CT_Source_Maxwell_3: { Init_Source_Maxwell_3((Source_Maxwell_3*) comp->comp, instr); } break;
        case CT_Beamstop: { Init_Beamstop((Beamstop*) comp->comp, instr); } break;
        case CT_PowderN: { Init_PowderN((PowderN*) comp->comp, instr); } break;
        case CT_Monitor_nD: { Init_Monitor_nD((Monitor_nD*) comp->comp, instr); } break;
        case CT_Monochromator_2foc: { Init_Monochromator_2foc((Monochromator_2foc*) comp->comp, instr); } break;

        default: { } break;
    }
}


void DisplayComponent(Component *comp) {
    switch (comp->type) {
        case CT_Slit: { Display_Slit((Slit*) comp->comp); } break;
        case CT_L_monitor: { Display_L_monitor((L_monitor*) comp->comp); } break;
        case CT_Bender: { Display_Bender((Bender*) comp->comp); } break;
        case CT_Progress_bar: { Display_Progress_bar((Progress_bar*) comp->comp); } break;
        case CT_PSD_monitor: { Display_PSD_monitor((PSD_monitor*) comp->comp); } break;
        case CT_Arm: { Display_Arm((Arm*) comp->comp); } break;
        case CT_Al_window: { Display_Al_window((Al_window*) comp->comp); } break;
        case CT_PSDlin_monitor: { Display_PSDlin_monitor((PSDlin_monitor*) comp->comp); } break;
        case CT_Guide: { Display_Guide((Guide*) comp->comp); } break;
        case CT_Source_Maxwell_3: { Display_Source_Maxwell_3((Source_Maxwell_3*) comp->comp); } break;
        case CT_Beamstop: { Display_Beamstop((Beamstop*) comp->comp); } break;
        case CT_PowderN: { Display_PowderN((PowderN*) comp->comp); } break;
        case CT_Monitor_nD: { Display_Monitor_nD((Monitor_nD*) comp->comp); } break;
        case CT_Monochromator_2foc: { Display_Monochromator_2foc((Monochromator_2foc*) comp->comp); } break;

        default: { } break;
    }
}


void TraceComponent(Component *comp, Neutron *particle, Instrument *instr = NULL) {
    switch (comp->type) {
        case CT_Slit: { Trace_Slit((Slit*) comp->comp, particle, instr); } break;
        case CT_L_monitor: { Trace_L_monitor((L_monitor*) comp->comp, particle, instr); } break;
        case CT_Bender: { Trace_Bender((Bender*) comp->comp, particle, instr); } break;
        case CT_Progress_bar: { Trace_Progress_bar((Progress_bar*) comp->comp, particle, instr); } break;
        case CT_PSD_monitor: { Trace_PSD_monitor((PSD_monitor*) comp->comp, particle, instr); } break;
        case CT_Arm: { Trace_Arm((Arm*) comp->comp, particle, instr); } break;
        case CT_Al_window: { Trace_Al_window((Al_window*) comp->comp, particle, instr); } break;
        case CT_PSDlin_monitor: { Trace_PSDlin_monitor((PSDlin_monitor*) comp->comp, particle, instr); } break;
        case CT_Guide: { Trace_Guide((Guide*) comp->comp, particle, instr); } break;
        case CT_Source_Maxwell_3: { Trace_Source_Maxwell_3((Source_Maxwell_3*) comp->comp, particle, instr); } break;
        case CT_Beamstop: { Trace_Beamstop((Beamstop*) comp->comp, particle, instr); } break;
        case CT_PowderN: { Trace_PowderN((PowderN*) comp->comp, particle, instr); } break;
        case CT_Monitor_nD: { Trace_Monitor_nD((Monitor_nD*) comp->comp, particle, instr); } break;
        case CT_Monochromator_2foc: { Trace_Monochromator_2foc((Monochromator_2foc*) comp->comp, particle, instr); } break;

        default: { } break;
    }
}


void SaveComponent(Component *comp) {
    switch (comp->type) {
        case CT_Slit: { Save_Slit((Slit*) comp->comp); } break;
        case CT_L_monitor: { Save_L_monitor((L_monitor*) comp->comp); } break;
        case CT_Bender: { Save_Bender((Bender*) comp->comp); } break;
        case CT_Progress_bar: { Save_Progress_bar((Progress_bar*) comp->comp); } break;
        case CT_PSD_monitor: { Save_PSD_monitor((PSD_monitor*) comp->comp); } break;
        case CT_Arm: { Save_Arm((Arm*) comp->comp); } break;
        case CT_Al_window: { Save_Al_window((Al_window*) comp->comp); } break;
        case CT_PSDlin_monitor: { Save_PSDlin_monitor((PSDlin_monitor*) comp->comp); } break;
        case CT_Guide: { Save_Guide((Guide*) comp->comp); } break;
        case CT_Source_Maxwell_3: { Save_Source_Maxwell_3((Source_Maxwell_3*) comp->comp); } break;
        case CT_Beamstop: { Save_Beamstop((Beamstop*) comp->comp); } break;
        case CT_PowderN: { Save_PowderN((PowderN*) comp->comp); } break;
        case CT_Monitor_nD: { Save_Monitor_nD((Monitor_nD*) comp->comp); } break;
        case CT_Monochromator_2foc: { Save_Monochromator_2foc((Monochromator_2foc*) comp->comp); } break;

        default: { } break;
    }
}


void FinallyComponent(Component *comp) {
    switch (comp->type) {
        case CT_Slit: { Finally_Slit((Slit*) comp->comp); } break;
        case CT_L_monitor: { Finally_L_monitor((L_monitor*) comp->comp); } break;
        case CT_Bender: { Finally_Bender((Bender*) comp->comp); } break;
        case CT_Progress_bar: { Finally_Progress_bar((Progress_bar*) comp->comp); } break;
        case CT_PSD_monitor: { Finally_PSD_monitor((PSD_monitor*) comp->comp); } break;
        case CT_Arm: { Finally_Arm((Arm*) comp->comp); } break;
        case CT_Al_window: { Finally_Al_window((Al_window*) comp->comp); } break;
        case CT_PSDlin_monitor: { Finally_PSDlin_monitor((PSDlin_monitor*) comp->comp); } break;
        case CT_Guide: { Finally_Guide((Guide*) comp->comp); } break;
        case CT_Source_Maxwell_3: { Finally_Source_Maxwell_3((Source_Maxwell_3*) comp->comp); } break;
        case CT_Beamstop: { Finally_Beamstop((Beamstop*) comp->comp); } break;
        case CT_PowderN: { Finally_PowderN((PowderN*) comp->comp); } break;
        case CT_Monitor_nD: { Finally_Monitor_nD((Monitor_nD*) comp->comp); } break;
        case CT_Monochromator_2foc: { Finally_Monochromator_2foc((Monochromator_2foc*) comp->comp); } break;

        default: { } break;
    }
}


#endif // __META_COMPS__

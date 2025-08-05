#ifndef __Progress_bar__
#define __Progress_bar__

// share block



struct Progress_bar {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters
    char *profile = (char*) "NULL";
    double percent = 10;
    double flag_save = 0;
    double minutes = 0;

    // declares
    double IntermediateCnts;
    time_t StartTime;
    time_t EndTime;
    time_t CurrentTime;
    char infostring[64];
};

Progress_bar Create_Progress_bar(s32 index, char *name) {
    Progress_bar _comp = {};
    Progress_bar *comp = &_comp;
    comp->type = (char*) "Progress_bar";
    comp->name = name;
    comp->index = index;

    return _comp;
}

void Init_Progress_bar(Progress_bar *comp, Instrument *instrument) {

    #define profile comp->profile
    #define percent comp->percent
    #define flag_save comp->flag_save
    #define minutes comp->minutes

    #define IntermediateCnts comp->IntermediateCnts
    #define StartTime comp->StartTime
    #define EndTime comp->EndTime
    #define CurrentTime comp->CurrentTime
    #define infostring comp->infostring
    ////////////////////////////////////////////////////////////////


    IntermediateCnts=0;
    StartTime=0;
    EndTime=0;
    CurrentTime=0;

    fprintf(stdout, "[%s] Initialize\n", instrument_name);
    if (percent*mcget_ncount()/100 < 1e5) {
        percent=1e5*100.0/mcget_ncount();
    }
    sprintf(infostring, "(single process) ");


    ////////////////////////////////////////////////////////////////
    #undef profile
    #undef percent
    #undef flag_save
    #undef minutes

    #undef IntermediateCnts
    #undef StartTime
    #undef EndTime
    #undef CurrentTime
    #undef infostring

}

void Trace_Progress_bar(Progress_bar *comp, Neutron *particle, Instrument *instrument) {
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

    #define profile comp->profile
    #define percent comp->percent
    #define flag_save comp->flag_save
    #define minutes comp->minutes

    #define IntermediateCnts comp->IntermediateCnts
    #define StartTime comp->StartTime
    #define EndTime comp->EndTime
    #define CurrentTime comp->CurrentTime
    #define infostring comp->infostring
    ////////////////////////////////////////////////////////////////


    double ncount;
    ncount = mcget_run_num();
    if (!StartTime) {
        time(&StartTime); /* compute starting time */
        IntermediateCnts = 1e3;
    }
    time_t NowTime;
    time(&NowTime);

    /* compute initial estimate of computation duration */
    if (!EndTime && ncount >= IntermediateCnts) {
        CurrentTime = NowTime;
        if (difftime(NowTime,StartTime) > 10 && ncount) {
            /* wait 10 sec before writing ETA */
            EndTime = StartTime + (time_t)(difftime(NowTime,StartTime) *(double)mcget_ncount()/ncount);
            IntermediateCnts = 0;
            fprintf(stdout, "\nTrace ETA ");
            fprintf(stdout, "%s", infostring);
            
            if (difftime(EndTime,StartTime) < 60.0)
                fprintf(stdout, "%g [s] ", difftime(EndTime,StartTime));
            else if (difftime(EndTime,StartTime) > 3600.0)
                fprintf(stdout, "%g [h] ", difftime(EndTime,StartTime)/3600.0);
            else
                fprintf(stdout, "%g [min] ", difftime(EndTime,StartTime)/60.0);
            fprintf(stdout, "\n");
        } else
            IntermediateCnts += 1e3;
        fflush(stdout);
    }

    /* display percentage when percent or minutes have reached step */
    if (EndTime && mcget_ncount() &&
        (    (minutes && difftime(NowTime,CurrentTime) > minutes*60)
        || (percent && !minutes && ncount >= IntermediateCnts))   )
    {
        fprintf(stdout, "%llu %%\n", (unsigned long long)(ncount*100.0/mcget_ncount())); fflush(stdout);
        CurrentTime = NowTime;

        IntermediateCnts = ncount + percent*mcget_ncount()/100;
        /* check that next intermediate ncount check is a multiple of the desired percentage */
        IntermediateCnts = floor(IntermediateCnts*100/percent/mcget_ncount())*percent*mcget_ncount()/100;
        /* raise flag to indicate that we did something */
        SCATTER;
        if (flag_save)
            save(NULL);
    }


    ////////////////////////////////////////////////////////////////
    #undef profile
    #undef percent
    #undef flag_save
    #undef minutes

    #undef IntermediateCnts
    #undef StartTime
    #undef EndTime
    #undef CurrentTime
    #undef infostring

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

void Save_Progress_bar(Progress_bar *comp) {

    #define profile comp->profile
    #define percent comp->percent
    #define flag_save comp->flag_save
    #define minutes comp->minutes

    #define IntermediateCnts comp->IntermediateCnts
    #define StartTime comp->StartTime
    #define EndTime comp->EndTime
    #define CurrentTime comp->CurrentTime
    #define infostring comp->infostring
    ////////////////////////////////////////////////////////////////


    fprintf(stdout, "\nSave [%s]\n", instrument_name);
    if (profile && strlen(profile) && strcmp(profile,"NULL") && strcmp(profile,"0")) {
        char filename[256];
        if (!strlen(profile) || !strcmp(profile,"NULL") || !strcmp(profile,"0")) strcpy(filename, instrument_name);
        else strcpy(filename, profile);
        DETECTOR_OUT_1D(
            "Intensity profiler",
            "Component index [1]",
            "Intensity",
            "prof", 1, mcNUMCOMP, mcNUMCOMP-1,
            
            // jg-250618: disabled
            //&(instrument->counter_N[1]),&(instrument->counter_P[1]),&(instrument->counter_P2[1]),
            NULL, NULL, NULL,
            filename);
    }


    ////////////////////////////////////////////////////////////////
    #undef profile
    #undef percent
    #undef flag_save
    #undef minutes

    #undef IntermediateCnts
    #undef StartTime
    #undef EndTime
    #undef CurrentTime
    #undef infostring
}

void Finally_Progress_bar(Progress_bar *comp) {

    #define profile comp->profile
    #define percent comp->percent
    #define flag_save comp->flag_save
    #define minutes comp->minutes

    #define IntermediateCnts comp->IntermediateCnts
    #define StartTime comp->StartTime
    #define EndTime comp->EndTime
    #define CurrentTime comp->CurrentTime
    #define infostring comp->infostring
    ////////////////////////////////////////////////////////////////


    time_t NowTime;
    time(&NowTime);
    fprintf(stdout, "\nFinally [%s: %s]. Time: ", instrument_name, dirname ? dirname : ".");
    if (difftime(NowTime,StartTime) < 60.0)
        fprintf(stdout, "%g [s] ", difftime(NowTime,StartTime));
    else if (difftime(NowTime,StartTime) > 3600.0)
        fprintf(stdout, "%g [h] ", difftime(NowTime,StartTime)/3600.0);
    else
        fprintf(stdout, "%g [min] ", difftime(NowTime,StartTime)/60.0);
    fprintf(stdout, "\n");


    ////////////////////////////////////////////////////////////////
    #undef profile
    #undef percent
    #undef flag_save
    #undef minutes

    #undef IntermediateCnts
    #undef StartTime
    #undef EndTime
    #undef CurrentTime
    #undef infostring
}

void Display_Progress_bar(Progress_bar *comp) {
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

    #define profile comp->profile
    #define percent comp->percent
    #define flag_save comp->flag_save
    #define minutes comp->minutes

    #define IntermediateCnts comp->IntermediateCnts
    #define StartTime comp->StartTime
    #define EndTime comp->EndTime
    #define CurrentTime comp->CurrentTime
    #define infostring comp->infostring
    ////////////////////////////////////////////////////////////////





    ////////////////////////////////////////////////////////////////
    #undef profile
    #undef percent
    #undef flag_save
    #undef minutes

    #undef IntermediateCnts
    #undef StartTime
    #undef EndTime
    #undef CurrentTime
    #undef infostring

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

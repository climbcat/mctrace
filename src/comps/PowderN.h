#ifndef __PowderN__
#define __PowderN__

// share block



/* Declare structures and functions only once in each instrument. */
#ifndef POWDERN_DECL
#define POWDERN_DECL

struct line_data
{
    double F2;                  /* Value of structure factor */
    double q;                   /* Qvector */
    int j;                      /* Multiplicity */
    double DWfactor;            /* Debye-Waller factor */
    double w;                   /* Intrinsic line width */
    double Epsilon;             /* Strain=delta_d_d/d shift in ppm */
};

struct line_info_struct
{
    struct line_data *list;     /* Reflection array */
    int    count;                  /* Number of reflections */
    double Dd;
    double DWfactor;
    double V_0;
    double rho;
    double at_weight;
    double at_nb;
    double sigma_a;
    double sigma_i;
    char   compname[256];
    double flag_barns;
    int    shape; /* 0 cylinder, 1 box, 2 sphere, 3 OFF file */
    int    column_order[9]; /* column signification */
    int    flag_warning;
    double dq;    /* wavevector transfer [Angs-1] */
    double Epsilon; /* global strain in ppm */
    double XsectionFactor;
    double my_s_v2_sum;
    double my_a_v;
    double my_inc;
    double lfree; // store mean free path for the last event;
    double *w_v,*q_v, *my_s_v2;
    double radius_i,xwidth_i,yheight_i,zdepth_i;
    double v; /* last velocity (cached) */
    double Nq;
    int    nb_reuses, nb_refl, nb_refl_count;
    double v_min, v_max;
    double xs_Nq[CHAR_BUF_LENGTH];
    double xs_sum[CHAR_BUF_LENGTH];
    double neutron_passed;
    long   xs_compute, xs_reuse, xs_calls;
};

// PN_list_compare *****************************************************************

int PN_list_compare (void const *a, void const *b)
{
    struct line_data const *pa = (struct line_data const *) a;
    struct line_data const *pb = (struct line_data const *) b;
    double s = pa->q - pb->q;

    if (!s) return 0;
    else    return (s < 0 ? -1 : 1);
} /* PN_list_compare */
  
#ifndef CIF2HKL
#define CIF2HKL

// hkl_filename = cif2hkl(file, options)
//   used to convert CIF/CFL/INS file into F2(hkl)
//   the CIF2HKL env var can point to a cif2hkl executable
//   else the McCode binary is attempted, then the system.
char *cif2hkl(char *infile, char *options) {
    char cmd[1024];
    int  ret = 0;
    int  found = 0;
    char *OUTFILE;
    char *inpath;
    
    // get filename extension
    char *ext = strrchr(infile, '.');
    if(!ext || ext == infile) return infile;
    else ext++;
    
    // return input when no extension or not a CIF/FullProf/ShelX file
    if ( strcasecmp(ext, "cif") 
    && strcasecmp(ext, "pcr")
    && strcasecmp(ext, "cfl")
    && strcasecmp(ext, "shx")
    && strcasecmp(ext, "ins")
    && strcasecmp(ext, "res")) return infile;
    
    OUTFILE = (char*) malloc(1024);
    if (!OUTFILE)
        return infile;
    inpath = (char*) malloc(1024);
    if (!inpath)
        return infile;

    // get input file path from read-table:Open_File
    FILE *f_infile = Open_File(infile, "r", inpath);
    if (!f_infile) return infile;
    fclose(f_infile);
    
    strncpy(OUTFILE, tmpnam(NULL), 1024); // create an output temporary file name
    
    // try in order the CIF2HKL env var, then the system cif2hkl, then the McCode one
    if (!found && getenv("CIF2HKL")) {
        snprintf(cmd,  1024, "%s -o %s %s %s",
            getenv("CIF2HKL"),
            OUTFILE, options, inpath);
        ret = system(cmd);
        if (ret != -1 && ret != 127) found = 1;
    }
    if (!found) {
        // try with cif2hkl command from the system PATH
        snprintf(cmd,  1024, "%s -o %s %s %s",
            "cif2hkl", OUTFILE, options, infile);
        ret = system(cmd);
        if (ret != -1 && ret != 127) found = 1;
    }
    if (!found) {
        // As a last resort, attempt with cif2hkl from $MCSTAS/bin
        snprintf(cmd,  1024, "mcstas/%s/%s -o %s %s %s",
            "bin", "cif2hkl",
            OUTFILE, options, inpath);
        ret = system(cmd);
    }
    // ret = -1:  child process could not be created
    // ret = 127: shell could not be executed in the child process     
    if (ret == -1 || ret == 127) {
        free(OUTFILE);
        return(NULL);
    }

    // test if the result file has been created
    FILE *file = fopen(OUTFILE,"r");
    if (!file) {
        free(OUTFILE);
        return(NULL);
    }
    printf("%s: INFO: Converting %s into F2(HKL) list %s\n", 
        __FILE__, inpath, OUTFILE);
    printf ("%s\n",cmd);
    fflush(NULL);
    fclose(file);
    return(OUTFILE);
} // cif2hkl
#endif

int read_line_data(char *SC_file, struct line_info_struct *info)
{
    struct line_data *list = NULL;
    int    size = 0;
    t_Table sTable; /* sample data table structure from SC_file */
    int    i=0;
    int    mult_count  =0;
    char   flag=0;
    double q_count=0, j_count=0, F2_count=0;
    char **parsing;
    int    list_count=0;
    char  *filename = NULL;

    if (!SC_file || !strlen(SC_file) || !strcmp(SC_file, "NULL")) {
        printf("PowderN: %s: Using incoherent elastic scattering only.\n",
            info->compname);
        info->count = 0;
        return(0);
    }
    filename = cif2hkl(SC_file, (char*) "--mode NUC");
    if (filename != SC_file)
        info->flag_barns=1; // cif2hkl returns barns
    long retval = Table_Read(&sTable, filename, 1); /* read 1st block data from SC_file into sTable*/
    if (retval<0) {
        fprintf(stderr,"PowderN: Could not open file %s - exiting!\n",SC_file);
        exit(-1);
    }

    /* parsing of header */
    parsing = Table_ParseHeader(sTable.header,
        "Vc","V_0",
        "sigma_abs","sigma_a ",
        "sigma_inc","sigma_i ",
        "column_j",
        "column_d",
        "column_F2",
        "column_DW",
        "column_Dd",
        "column_inv2d", "column_1/2d", "column_sintheta/lambda",
        "column_q", /* 14 */
        "DW", "Debye_Waller",
        "delta_d_d/d",
        "column_F ",
        "V_rho",
        "density",
        "weight",
        "nb_atoms","multiplicity", /* 23 */
        "column_ppm","column_strain",
        NULL);

    if (parsing) {
        if (parsing[0] && !info->V_0)     info->V_0    =atof(parsing[0]);
        if (parsing[1] && !info->V_0)     info->V_0    =atof(parsing[1]);
        if (parsing[2] && !info->sigma_a) info->sigma_a=atof(parsing[2]);
        if (parsing[3] && !info->sigma_a) info->sigma_a=atof(parsing[3]);
        if (parsing[4] && !info->sigma_i) info->sigma_i=atof(parsing[4]);
        if (parsing[5] && !info->sigma_i) info->sigma_i=atof(parsing[5]);
        if (parsing[6])                   info->column_order[0]=atoi(parsing[6]);
        if (parsing[7])                   info->column_order[1]=atoi(parsing[7]);
        if (parsing[8])                   info->column_order[2]=atoi(parsing[8]);
        if (parsing[9])                   info->column_order[3]=atoi(parsing[9]);
        if (parsing[10])                  info->column_order[4]=atoi(parsing[10]);
        if (parsing[11])                  info->column_order[5]=atoi(parsing[11]);
        if (parsing[12])                  info->column_order[5]=atoi(parsing[12]);
        if (parsing[13])                  info->column_order[5]=atoi(parsing[13]);
        if (parsing[14])                  info->column_order[6]=atoi(parsing[14]);
        if (parsing[15] && info->DWfactor<=0)    info->DWfactor=atof(parsing[15]);
        if (parsing[16] && info->DWfactor<=0)    info->DWfactor=atof(parsing[16]);
        if (parsing[17] && info->Dd <0)          info->Dd      =atof(parsing[17]);
        if (parsing[18])                  info->column_order[7]=atoi(parsing[18]);
        if (parsing[19] && !info->V_0)    info->V_0    =1/atof(parsing[19]);
        if (parsing[20] && !info->rho)    info->rho    =atof(parsing[20]);
        if (parsing[21] && !info->at_weight)     info->at_weight    =atof(parsing[21]);
        if (parsing[22] && info->at_nb <= 1)  info->at_nb    =atof(parsing[22]);
        if (parsing[23] && info->at_nb <= 1)  info->at_nb    =atof(parsing[23]);
        if (parsing[24])                  info->column_order[8]=atoi(parsing[24]);
        if (parsing[25])                  info->column_order[8]=atoi(parsing[25]);
        for (i=0; i<=25; i++) if (parsing[i]) free(parsing[i]);
        free(parsing);
    }

    if (!sTable.rows)
        exit(fprintf(stderr, "PowderN: %s: Error: The number of rows in %s "
        "should be at least %d\n", info->compname, SC_file, 1));
    else
        size = sTable.rows;

    Table_Info(sTable);
    printf("PowderN: %s: Reading %d rows from %s\n",
          info->compname, size, SC_file);

    if (filename == SC_file) { // only when not from cif2hkl
      if (info->column_order[0] == 4 && info->flag_barns !=0)
        printf("PowderN: %s: Powder file probably of type Crystallographica/Fullprof (lau)\n"
            "WARNING: but F2 unit is set to barns=1 (barns). Intensity might be 100 times too high.\n",
            info->compname);

      if (info->column_order[0] == 17 && info->flag_barns == 0)
        printf("PowderN: %s: Powder file probably of type Lazy Pulver (laz)\n"
            "WARNING: but F2 unit is set to barns=0 (fm^2). Intensity might be 100 times too low.\n",
            info->compname);
    }
    /* allocate line_data array */
    list = (struct line_data*)malloc(size*sizeof(struct line_data));

    for (i=0; i<size; i++)
    {
        /*      printf("Reading in line %i\n",i);*/
        double j=0, d=0, w=0, q=0, DWfactor=0, F2=0, Epsilon=0;
        int index;

        if (info->Dd >= 0)      w         = info->Dd;
        if (info->DWfactor > 0) DWfactor  = info->DWfactor;
        if (info->Epsilon)      Epsilon   = info->Epsilon*1e-6;

        /* get data from table using columns {j d F2 DW Dd inv2d q F} */
        /* column indexes start at 1, thus need to substract 1 */
        if (info->column_order[0] >0)
            j = Table_Index(sTable, i, info->column_order[0]-1);
        if (info->column_order[1] >0)
            d = Table_Index(sTable, i, info->column_order[1]-1);
        if (info->column_order[2] >0)
            F2 = Table_Index(sTable, i, info->column_order[2]-1);
        if (info->column_order[3] >0)
            DWfactor = Table_Index(sTable, i, info->column_order[3]-1);
        if (info->column_order[4] >0)
            w = Table_Index(sTable, i, info->column_order[4]-1);
        if (info->column_order[5] >0 && !(info->column_order[1] >0)) { // Only use if d not read already
            d = Table_Index(sTable, i, info->column_order[5]-1);
            d = (d > 0? 1/d/2 : 0);
        }
        if (info->column_order[6] >0 && !(info->column_order[1] >0)) { // Only use if d not read already 
            q = Table_Index(sTable, i, info->column_order[6]-1);
            d = (q > 0 ? 2*PI/q : 0);
        }
        if (info->column_order[7] >0  && !F2) { 
            F2 = Table_Index(sTable, i, info->column_order[7]-1); F2 *= F2;
        }
        if (info->column_order[8] >0  && !Epsilon) { 
            Epsilon = Table_Index(sTable, i, info->column_order[8]-1)*1e-6;
        }

        /* assign and check values */
        j        = (j > 0 ? j : 0);
        q        = (d > 0 ? 2*PI/d : 0); /* this is q */
        if (Epsilon && fabs(Epsilon) < 1e6) {
            q     -= Epsilon*q; /* dq/q = -delta_d_d/d = -Epsilon */
        }
        DWfactor = (DWfactor > 0 ? DWfactor : 1);
        w        = (w>0 ? w : 0); /* this is q and d relative spreading */
        F2       = (F2 >= 0 ? F2 : 0);
        if (j == 0 || q == 0) {
            printf("PowderN: %s: line %i has invalid definition\n"
                "         (mult=0 or q=0 or d=0)\n", info->compname, i);
            continue;
        }
        list[list_count].j = j;
        list[list_count].q = q;
        list[list_count].DWfactor = DWfactor;
        list[list_count].w = w;
        list[list_count].F2= F2;
        list[list_count].Epsilon = Epsilon;

        if (!q_count)      q_count  = q;
        if (!j_count)      j_count  = j;
        if (!F2_count)     F2_count = F2;
        if (fabs(q_count-q) < 0.0001*fabs(q)
        && fabs(F2_count-F2) < 0.0001*fabs(F2) && j_count == j) {
        mult_count++; flag=0; }
        else flag=1;
        if (i == size-1)
            flag=1;

        if (flag) {
            if (i == size-1) list_count++;

            if ((mult_count && list_count>0)
                && (mult_count == list[list_count-1].j
                    || ((list_count < size) && (i == size - 1)
                        && (mult_count == list[list_count].j))) )
            {
                printf("PowderN: %s: Set multiplicity to 1 for lines [%i:%i]\n"
                        "         (d-spacing %g is duplicated %i times)\n",
                    info->compname, list_count-mult_count, list_count-1, list[list_count-1].q, mult_count);
                for (index=list_count-mult_count; index<list_count; list[index++].j = 1);
                mult_count = 1;
                q_count   = q;
                j_count   = j;
                F2_count  = F2;
            }
            if (i == size-1) list_count--;
            flag=0;
        }
        list_count++;
    } /* end for */

    Table_Free(&sTable);

    /* sort the list with increasing q */
    qsort(list, list_count, sizeof(struct line_data),  PN_list_compare);

    printf("PowderN: %s: Read %i reflections from file '%s'\n",
        info->compname, list_count, SC_file);
    
    // remove temporary F2(hkl) file when giving CFL/CIF/ShelX file
    if (filename != SC_file)
        unlink(filename); 

    info->list  = list;
    info->count = list_count;

    return(list_count);
} /* read_line_data */


/* computes the number of possible reflections (return value), and the total xsection 'sum' */
/* this routine looks for a pre-computed value in the Nq and sum cache tables               */
/* when found, the earch starts from the corresponding lower element in the table           */
#pragma acc routine seq
int calc_xsect(double v, double *qv, double *my_sv2, int count, double *sum,
    struct line_info_struct *line_info) {
    int    Nq = 0, line=0, line0=0;
    *sum=0;

    /* check if a line_info element has been recorded already - not on OpenACC */
    #ifndef OPENACC
    if (v >= line_info->v_min && v <= line_info->v_max && line_info->neutron_passed >= CHAR_BUF_LENGTH) {
        line = (int)floor(v - line_info->v_min)*CHAR_BUF_LENGTH/(line_info->v_max - line_info->v_min);
        Nq    = line_info->xs_Nq[line];
        *sum  = line_info->xs_sum[line];
        if (!Nq && *sum == 0) {
            /* not yet set: we compute the sum up to the corresponding speed in the table cache */
            double line_v = line_info->v_min + line*(line_info->v_max - line_info->v_min)/CHAR_BUF_LENGTH;
            for(line0=0; line0<count; line0++) {
                if (qv[line0] <= 2*line_v) { /* q < 2*kf: restrict structural range */
                *sum += my_sv2[line0];
                if (Nq < line0+1) Nq=line0+1; /* determine maximum line index which can scatter */
                } else break;
            }
            line_info->xs_Nq[line] = Nq;
            line_info->xs_sum[line]= *sum;
            line_info->xs_compute++;
        } else line_info->xs_reuse++;
        line0 = Nq;
    }

    line_info->xs_calls++;
    #endif

    for(line=line0; line<count; line++) {
        if (qv[line] <= 2*v) { /* q < 2*kf: restrict structural range */
            *sum += my_sv2[line];
            if (Nq < line+1) Nq=line+1; /* determine maximum line index which can scatter */
        } else break;
    }

    return(Nq);
} /* calc_xsect */

#endif /* !POWDERN_DECL */



struct PowderN {
    int index;
    char *name;
    char *type;
    Coords position_absolute;
    Coords position_relative;
    Rotation rotation_absolute;
    Rotation rotation_relative;

    // parameters
    char *reflections = (char*) "NULL";
    char *geometry = (char*) "NULL";
    double format[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    double radius = 0;
    double yheight = 0;
    double xwidth = 0;
    double zdepth = 0;
    double thickness = 0;
    double pack = 1;
    double Vc = 0;
    double sigma_abs = 0;
    double sigma_inc = 0;
    double delta_d_d = 0;
    double p_inc = 0.1;
    double p_transmit = 0.1;
    double DW = 0;
    double nb_atoms = 1;
    double d_omega = 0;
    double d_phi = 0;
    double tth_sign = 0;
    double p_interact = 0.8;
    double concentric = 0;
    double density = 0;
    double weight = 0;
    double barns = 1;
    double Strain = 0;
    double focus_flip = 0;
    int target_index = 0;

    // declares
    struct line_info_struct line_info;
    double *columns;
    off_struct offdata;
    double tgt_x;
    double tgt_y;
    double tgt_z;
};

PowderN Create_PowderN(s32 index, char *name) {
    PowderN _comp = {};
    PowderN *comp = &_comp;
    comp->type = (char*) "PowderN";
    comp->name = name;
    comp->index = index;

    return _comp;
}

void Init_PowderN(PowderN *comp, Instrument *instrument) {

    #define reflections comp->reflections
    #define geometry comp->geometry
    #define format comp->format
    #define radius comp->radius
    #define yheight comp->yheight
    #define xwidth comp->xwidth
    #define zdepth comp->zdepth
    #define thickness comp->thickness
    #define pack comp->pack
    #define Vc comp->Vc
    #define sigma_abs comp->sigma_abs
    #define sigma_inc comp->sigma_inc
    #define delta_d_d comp->delta_d_d
    #define p_inc comp->p_inc
    #define p_transmit comp->p_transmit
    #define DW comp->DW
    #define nb_atoms comp->nb_atoms
    #define d_omega comp->d_omega
    #define d_phi comp->d_phi
    #define tth_sign comp->tth_sign
    #define p_interact comp->p_interact
    #define concentric comp->concentric
    #define density comp->density
    #define weight comp->weight
    #define barns comp->barns
    #define Strain comp->Strain
    #define focus_flip comp->focus_flip
    #define target_index comp->target_index

    #define line_info comp->line_info
    #define columns comp->columns
    #define offdata comp->offdata
    #define tgt_x comp->tgt_x
    #define tgt_y comp->tgt_y
    #define tgt_z comp->tgt_z
    ////////////////////////////////////////////////////////////////


    /* We ought to clean up the columns variable as format is now a proper vector/array */
    columns = format;

    int i=0;
    struct line_data *L;
    line_info.Dd       = delta_d_d;
    line_info.DWfactor = DW;
    line_info.V_0      = Vc;
    line_info.rho      = density;
    line_info.at_weight= weight;
    line_info.at_nb    = nb_atoms;
    line_info.sigma_a  = sigma_abs;
    line_info.sigma_i  = sigma_inc;
    line_info.flag_barns=barns;
    line_info.shape    = 0;
    line_info.flag_warning=0;
    line_info.Epsilon  = Strain;
    line_info.radius_i =line_info.xwidth_i=line_info.yheight_i=line_info.zdepth_i=0;
    line_info.v  = 0;
    line_info.Nq = 0;
    line_info.v_min = FLT_MAX; line_info.v_max = 0;
    line_info.neutron_passed=0;
    line_info.nb_reuses = line_info.nb_refl = line_info.nb_refl_count = 0;
    line_info.xs_compute= line_info.xs_reuse= line_info.xs_calls =0;
    for (i=0; i< 9; i++) {
        line_info.column_order[i] = (int)columns[i];
    }
    strncpy(line_info.compname, NAME_CURRENT_COMP, 256);

    line_info.shape=-1; /* -1:no shape, 0:cyl, 1:box, 2:sphere, 3:any-shape  */
    if (geometry && strlen(geometry) && strcmp(geometry, "NULL") && strcmp(geometry, "0")) {
        #ifndef USE_OFF
        fprintf(stderr,"Error: You are attempting to use an OFF geometry without -DUSE_OFF. You will need to recompile with that define set!\n");
        exit(-1);
        #else
        if (off_init(geometry, xwidth, yheight, zdepth, 0, &offdata)) {
            line_info.shape=3; thickness=0; concentric=0;
        }
        #endif
    }
    else
        if (xwidth && yheight && zdepth)  line_info.shape=1; /* box */
    else if (radius > 0 && yheight)        line_info.shape=0; /* cylinder */
    else if (radius > 0 && !yheight)       line_info.shape=2; /* sphere */

    if (line_info.shape < 0)
        exit(fprintf(stderr,"PowderN: %s: sample has invalid dimensions.\n"
                            "ERROR    Please check parameter values (xwidth, yheight, zdepth, radius).\n", NAME_CURRENT_COMP));
    if (thickness) {
        if (radius && (radius < fabs(thickness))) {
            printf("PowderN: %s: hollow sample thickness is larger than its volume (sphere/cylinder).\n"
                            "WARNING  Please check parameter values. Using bulk sample (thickness=0).\n", NAME_CURRENT_COMP);
            thickness=0;
        }
        else if (!radius && (xwidth < 2*fabs(thickness) || yheight < 2*fabs(thickness) || zdepth < 2*fabs(thickness))) {
            printf("PowderN: %s: hollow sample thickness is larger than its volume (box).\n"
                            "WARNING  Please check parameter values.\n", NAME_CURRENT_COMP);
        }
    }

    if (concentric && thickness==0) {
        printf("PowderN: %s:Can not use concentric mode\n"
            "WARNING     on non hollow shape. Ignoring.\n",
            NAME_CURRENT_COMP);
        concentric=0;
    }

    if (thickness>0) {
        if (radius>thickness) {
            line_info.radius_i=radius-thickness;
        } else {
            if (xwidth>2*thickness)  line_info.xwidth_i =xwidth -2*thickness;
            if (yheight>2*thickness) line_info.yheight_i=yheight-2*thickness;
            if (zdepth>2*thickness)  line_info.zdepth_i =zdepth -2*thickness;
        }
    } else if (thickness<0) {
        thickness = fabs(thickness);
        if (radius) {
            line_info.radius_i=radius;
            radius=line_info.radius_i+thickness;
        } else {
            line_info.xwidth_i =xwidth;
            line_info.yheight_i=yheight;
            line_info.zdepth_i =zdepth;
            xwidth   =xwidth +2*thickness;
            yheight  =yheight+2*thickness;
            zdepth   =zdepth +2*thickness;
        }
    }

    if (!line_info.yheight_i) {
        line_info.yheight_i = yheight;
    }

    if (!p_interact){
        fprintf(stderr,"WARNING(%s): p_interact=0, adjusting to 0.01, to avoid algorithm instability\n",NAME_CURRENT_COMP);
        p_interact=1e-2;
    }
    if (!p_inc){
        fprintf(stderr,"WARNING(%s): p_inc=0, adjusting to 0.01, to avoid algorithm instability\n",NAME_CURRENT_COMP);
        p_inc     =1e-2;
    }
    if (!p_transmit){
        fprintf(stderr,"WARNING(%s): p_transmit=0, adjusting to 0.01, to avoid algorithm instability\n",NAME_CURRENT_COMP);
        p_transmit=1e-2;
    }
    double p_sum=p_interact+p_inc+p_transmit;
    p_interact = p_interact / p_sum;
    p_inc      = p_inc      / p_sum;
    p_transmit = p_transmit / p_sum;

    if (concentric) {
        printf("PowderN: %s: Concentric mode - remember to include the 'opposite' copy of this component !\n"
            "WARNING  The equivalent, 'opposite' comp should have concentric=0\n", NAME_CURRENT_COMP);
        if (p_transmit < 0.1) {
            printf("PowderN: %s: Concentric mode and p_transmit<0.1 !\n"
                "WARNING  Consider increasing p_transmit as few particles will reach the inner hollow.\n", NAME_CURRENT_COMP);
        }
    }

    if (reflections && strlen(reflections) && strcmp(reflections, "NULL") && strcmp(reflections, "0")) {
        i = read_line_data(reflections, &line_info);
        if (i == 0)
            exit(fprintf(stderr,"PowderN: %s: reflection file %s is not valid.\n"
                            "ERROR    Please check file format (laz or lau).\n", NAME_CURRENT_COMP, reflections));
    }

    /* compute the scattering unit density from material weight and density */
    /* the weight of the scattering element is the chemical formula molecular weight
    * times the nb of chemical formulae in the scattering element (nb_atoms) */
    if (!line_info.V_0 && line_info.at_nb > 0 && line_info.at_weight > 0 && line_info.rho > 0) {
        /* molar volume [cm^3/mol] = weight [g/mol] / density [g/cm^3] */
        /* atom density per Angs^3 = [mol/cm^3] * N_Avogadro *(1e-8)^3 */
        line_info.V_0 = line_info.at_nb / (line_info.rho/line_info.at_weight/1e24*6.02214199e23);
    }

    /* the scattering unit cross sections are the chemical formula onces
    * times the nb of chemical formulae in the scattering element */
    if (line_info.at_nb > 0) {
        line_info.sigma_a *= line_info.at_nb; line_info.sigma_i *= line_info.at_nb;
    }

    if (line_info.sigma_a<0) line_info.sigma_a=0;
    if (line_info.sigma_i<0) line_info.sigma_i=0;

    if (line_info.V_0 <= 0) {
        printf("PowderN: %s: density/unit cell volume is NULL (Vc). Unactivating component.\n", NAME_CURRENT_COMP);
    }

    if (line_info.V_0 > 0 && p_inc && !line_info.sigma_i) {
        printf("PowderN: %s: WARNING: You have requested statistics for incoherent scattering but not defined sigma_inc!\n", NAME_CURRENT_COMP);
    }

    if (line_info.flag_barns) { /* Factor 100 to convert from barns to fm^2 */
        line_info.XsectionFactor = 100;
    } else {
        line_info.XsectionFactor = 1;
    }

    if (line_info.V_0 > 0 && i) {
        L = line_info.list;

        line_info.q_v = (double*) malloc(line_info.count*sizeof(double));
        line_info.w_v = (double*) malloc(line_info.count*sizeof(double));
        line_info.my_s_v2 = (double*) malloc(line_info.count*sizeof(double));

        if (!line_info.q_v || !line_info.w_v || !line_info.my_s_v2) {
            exit(fprintf(stderr,"PowderN: %s: ERROR allocating memory (init)\n", NAME_CURRENT_COMP));
        }
        for(i=0; i<line_info.count; i++)
        {
            line_info.my_s_v2[i] = 4*PI*PI*PI*pack*(L[i].DWfactor ? L[i].DWfactor : 1)
                        /(line_info.V_0*line_info.V_0*V2K*V2K)
                        *(L[i].j * L[i].F2 / L[i].q)*line_info.XsectionFactor;
            /* Is not yet divided by v^2 */
            /* Squires [3.103] */
            line_info.q_v[i] = L[i].q*K2V;
            line_info.w_v[i] = L[i].w;
        }
    }
    if (line_info.V_0 > 0) {
        /* Is not yet divided by v */
        line_info.my_a_v = pack*line_info.sigma_a/line_info.V_0*2200*100;   // Factor 100 to convert from barns to fm^2
        line_info.my_inc = pack*line_info.sigma_i/line_info.V_0*100;   // Factor 100 to convert from barns to fm^2
        printf("PowderN: %s: Vc=%g [Angs] sigma_abs=%g [barn] sigma_inc=%g [barn] reflections=%s\n",
            NAME_CURRENT_COMP, line_info.V_0, line_info.sigma_a, line_info.sigma_i, reflections && strlen(reflections) ? reflections : "NULL");
    }
    
    /* update JS, 1/7/2017
        Get target coordinates relative to the local reference frame.
    */
    if (target_index) {
		Coords ToTarget;
		ToTarget = coords_sub(POS_A_COMP_INDEX(INDEX_CURRENT_COMP+target_index),POS_A_CURRENT_COMP);
		ToTarget = rot_apply(ROT_A_CURRENT_COMP, ToTarget);
		coords_get(ToTarget, &tgt_x, &tgt_y, &tgt_z);
		NORM(tgt_x, tgt_y, tgt_z);
		printf("PowderN: Target direction = (%g %g %g)\n",tgt_x, tgt_y, tgt_z);
	} else {
		tgt_x=0.0;
		tgt_y=0.0;
		tgt_z=1.0;
    }


    ////////////////////////////////////////////////////////////////
    #undef reflections
    #undef geometry
    #undef format
    #undef radius
    #undef yheight
    #undef xwidth
    #undef zdepth
    #undef thickness
    #undef pack
    #undef Vc
    #undef sigma_abs
    #undef sigma_inc
    #undef delta_d_d
    #undef p_inc
    #undef p_transmit
    #undef DW
    #undef nb_atoms
    #undef d_omega
    #undef d_phi
    #undef tth_sign
    #undef p_interact
    #undef concentric
    #undef density
    #undef weight
    #undef barns
    #undef Strain
    #undef focus_flip
    #undef target_index

    #undef line_info
    #undef columns
    #undef offdata
    #undef tgt_x
    #undef tgt_y
    #undef tgt_z

}

void Trace_PowderN(PowderN *comp, Neutron *particle, Instrument *instrument) {
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

    #define reflections comp->reflections
    #define geometry comp->geometry
    #define format comp->format
    #define radius comp->radius
    #define yheight comp->yheight
    #define xwidth comp->xwidth
    #define zdepth comp->zdepth
    #define thickness comp->thickness
    #define pack comp->pack
    #define Vc comp->Vc
    #define sigma_abs comp->sigma_abs
    #define sigma_inc comp->sigma_inc
    #define delta_d_d comp->delta_d_d
    #define p_inc comp->p_inc
    #define p_transmit comp->p_transmit
    #define DW comp->DW
    #define nb_atoms comp->nb_atoms
    #define d_omega comp->d_omega
    #define d_phi comp->d_phi
    #define tth_sign comp->tth_sign
    #define p_interact comp->p_interact
    #define concentric comp->concentric
    #define density comp->density
    #define weight comp->weight
    #define barns comp->barns
    #define Strain comp->Strain
    #define focus_flip comp->focus_flip
    #define target_index comp->target_index

    #define line_info comp->line_info
    #define columns comp->columns
    #define offdata comp->offdata
    #define tgt_x comp->tgt_x
    #define tgt_y comp->tgt_y
    #define tgt_z comp->tgt_z
    ////////////////////////////////////////////////////////////////


    double t0, t1, t2, t3, v, v1,l_full, l, l_1, dt, alpha0, alpha, theta, my_s, my_s_n, sg;
    double solid_angle;
    double neutrontype = 0;
    double ntype = 0;
    double arg, tmp_vx, tmp_vy, tmp_vz, vout_x, vout_y, vout_z, nx, ny, nz, pmul=1;
    int    line;
    char   intersect=0;
    char   intersecti=0;
    
    // Variables calculated within thread for thread purpose only 
    char type = '\0';
    int itype = 0;
    double d_phi_thread = d_phi;
    // These ones are injected back to struct at the end of TRACE in non-OpenACC case
    int nb_reuses = line_info.nb_reuses;
    int nb_refl = line_info.nb_refl;
    int nb_refl_count = line_info.nb_refl_count;
    double vcache = line_info.v; 
    double Nq = line_info.Nq;
    double v_min = line_info.v_min;
    double v_max = line_info.v_max;
    double lfree = line_info.lfree;
    long   xs_compute = line_info.xs_compute;
    long   xs_reuse   = line_info.xs_reuse;
    long   xs_calls   = line_info.xs_calls;
    double dq = line_info.dq;

    #ifdef OPENACC
    #ifdef USE_OFF
    off_struct thread_offdata = offdata;
    #endif
    #else
    #define thread_offdata offdata
    #endif
  
    if (line_info.V_0 > 0 && (line_info.count || line_info.my_inc)) {
        if (line_info.shape == 1) {
            intersect  = box_intersect(&t0, &t3, x, y, z, vx, vy, vz, xwidth, yheight, zdepth);
            intersecti = box_intersect(&t1, &t2, x, y, z, vx, vy, vz, line_info.xwidth_i, line_info.yheight_i, line_info.zdepth_i);
        } else if (line_info.shape == 0) {
            intersect  = cylinder_intersect(&t0, &t3, x, y, z, vx, vy, vz, radius, yheight);
            intersecti = cylinder_intersect(&t1, &t2, x, y, z, vx, vy, vz, line_info.radius_i, line_info.yheight_i);
        } else if (line_info.shape == 2) {
            intersect  = sphere_intersect  (&t0, &t3, x,y,z, vx,vy,vz, radius);
            intersecti = sphere_intersect  (&t1, &t2, x,y,z, vx,vy,vz, line_info.radius_i);
        }
        #ifdef USE_OFF
        else if (line_info.shape == 3) {
            intersect  = off_intersect  (&t0, &t3, NULL, NULL, x,y,z, vx,vy,vz, 0, 0, 0, thread_offdata);
            intersecti = 0;
        }
        #endif
    } 

  if(intersect && t3 >0) {

    if (concentric) {
        /* Set up for concentric case */
        /* 'Remove' the backside of this comp */
        if (!intersecti) {
            t1 = (t3 + t0) /2;
        }
        t2 = t1;
        t3 = t1;
        dt = -1.0*rand01(); /* In case of scattering we will scatter on 'forward' part of sample */
    }
    else {
        if (!intersecti) {
            t1 = (t3 + t0) /2;
            t2 = t1;
        }
        dt = randpm1(); /* Possibility to scatter at all points in line of sight */
    }

    /* Neutron enters at t=t0. */
    if(t0 < 0) t0=0; /* already in sample */
    if(t1 < 0) t1=0; /* already in inner hollow */
    if(t2 < 0) t2=0; /* already past inner hollow */
    v = sqrt(vx*vx + vy*vy + vz*vz);
    l_full = v * (t3 - t2 + t1 - t0);

    if (line_info.neutron_passed < CHAR_BUF_LENGTH) {
        if (v < v_min) v_min = v;
        if (v > v_max) v_max = v;
        line_info.neutron_passed++;
    }

    /* Calculate total scattering cross section at relevant velocity - but not on GPU*/
    #ifndef OPENACC
    if ( fabs(v - vcache) < 1e-6) {
        nb_reuses++;
    }
    else {
    #endif
        Nq = calc_xsect(v, line_info.q_v, line_info.my_s_v2, line_info.count, &line_info.my_s_v2_sum, &line_info);
        vcache = v;
        nb_refl += Nq;
        nb_refl_count++;
    #ifndef OPENACC
    }
    #endif

        if (t3 < 0) {
        t3=0; /* Already past sample?! */
        if (line_info.flag_warning < 10)
        printf("PowderN: %s: Warning: Neutron has already passed us? (Skipped).\n"
                "         In concentric geometry, this may be caused by a missing concentric=0 option in 2nd enclosing instance.\n", NAME_CURRENT_COMP);
        line_info.flag_warning++;
    }
    else {
        if (dt<0) { /* Calculate scattering point position */
                dt = fabs(dt)*(t1 - t0); /* 'Forward' part */
        } else {
                dt = dt * (t3 - t2) + (t2-t0) ; /* Possibly also 'backside' part */
        }

        my_s = line_info.my_s_v2_sum/(v*v)+line_info.my_inc;
        /* Total attenuation from scattering */
        lfree=0;
        ntype = rand01();
        /* How to handle this one? Transmit (1) / Incoherent (2) / Coherent (3) ? */
        if (ntype < p_transmit) {
                neutrontype = 1;
                l = l_full; /* Passing through, full length */
                PROP_DT(t3);
        } else if (ntype >= p_transmit && ntype < (p_transmit + p_inc)) {
                neutrontype = 2;
                l = v*dt;       /* Penetration in sample */
                PROP_DT(dt+t0); /* Point of scattering */
                SCATTER;
        } else if (ntype >= p_transmit + p_inc) {
                neutrontype = 3;
                l = v*dt;       /* Penetration in sample */
                PROP_DT(dt+t0); /* Point of scattering */
                SCATTER;
        } else {
                exit(fprintf(stderr,"PowderN %s: DEAD - this shouldn't happen!\n", NAME_CURRENT_COMP));
      }

      if (neutrontype == 3) { /* Make coherent scattering event */
            if (line_info.count > 0) {
            /* choose line */
                if (Nq > 1) line=floor(Nq*rand01());  /* Select between Nq powder lines */
                else line = 0;
                if (line_info.w_v[line])
                    arg = line_info.q_v[line]*(1+line_info.w_v[line]*randnorm())/(2.0*v);
                else
                    arg = line_info.q_v[line]/(2.0*v);
                my_s_n = line_info.my_s_v2[line]/(v*v);
                if(fabs(arg) > 1)
                    ABSORB;                   /* No bragg scattering possible*/
                if (tth_sign == 0) {
                    sg = randpm1();
                    if (sg > 0) sg = 1; else sg=-1;
                }
                else  {
                sg = tth_sign/fabs(tth_sign);
                }
                theta = asin(arg);          /* Bragg scattering law */
                /* Choose point on Debye-Scherrer cone */
                if (d_phi_thread)
                { /* relate height of detector to the height on DS cone */
                    arg = sin(d_phi_thread*DEG2RAD/2)/sin(2*theta);
                    /* If full Debye-Scherrer cone is within d_phi, don't focus */
                    if (arg < -1 || arg > 1) d_phi_thread = 0;
                    /* Otherwise, determine alpha to rotate from scattering plane
                    into d_phi focusing area*/
                    else alpha = 2*asin(arg);
                }
                if (d_phi_thread) {
                    /* Focusing */
                    alpha = fabs(alpha);
                    alpha0 = 0.5*randpm1()*alpha;
                    if(focus_flip){
                        alpha0+=M_PI_2;
                    }
                }
                else
                    alpha0 = PI*randpm1();

                /* now find a nearly vertical rotation axis:
                * Either
                *  (v along Z) x (X axis) -> nearly Y axis
                * Or
                *  (v along X) x (Z axis) -> nearly Y axis
                */
            
                /* update JS, 1/7/2017
                    If a target is defined, try to define vertical axis as a normal to the plane 
                    defined by the incident neutron velocity and target position. 
                    Check that v is not ~ parallel to the target direction.
                */

                double vnorm=0.0;
                if (target_index) {
                    vec_prod(tmp_vx, tmp_vy, tmp_vz, vx,vy,vz, tgt_x, tgt_y, tgt_z);
                    vnorm = sqrt(tmp_vx*tmp_vx+tmp_vy*tmp_vy+tmp_vz*tmp_vz)/v;
                }
                // no target or direction is nearly parallel to v:
                if (vnorm<0.01) {
                    if (fabs(vx/v) < fabs(vz/v)) {
                        nx = 1; ny = 0; nz = 0;
                    } else {
                        nx = 0; ny = 0; nz = 1;
                    }
                    vec_prod(tmp_vx,tmp_vy,tmp_vz, vx,vy,vz, nx,ny,nz);
                }

                /* v_out = rotate 'v' by 2*theta around tmp_v: Bragg angle */
                rotate(vout_x,vout_y,vout_z, vx,vy,vz, 2*sg*theta, tmp_vx,tmp_vy,tmp_vz);
                
                /* tmp_v = rotate v_out by alpha0 around 'v' (Debye-Scherrer cone) */
                rotate(tmp_vx,tmp_vy,tmp_vz, vout_x,vout_y,vout_z, alpha0, vx, vy, vz);
                vx = tmp_vx;
                vy = tmp_vy;
                vz = tmp_vz;
                
                /* Since now scattered and new direction given, calculate path to exit */
                if (line_info.shape == 1) {
                    intersect  = box_intersect(&t0, &t3, x, y, z, vx, vy, vz, xwidth, yheight, zdepth);
                    intersecti = box_intersect(&t1, &t2, x, y, z, vx, vy, vz, line_info.xwidth_i, line_info.yheight_i, line_info.zdepth_i);
                } else if (line_info.shape == 0) {
                    intersect  = cylinder_intersect(&t0, &t3, x, y, z, vx, vy, vz, radius, yheight);
                    intersecti = cylinder_intersect(&t1, &t2, x, y, z, vx, vy, vz, line_info.radius_i, line_info.yheight_i);
                } else if (line_info.shape == 2) {
                    intersect  = sphere_intersect  (&t0, &t3, x,y,z, vx,vy,vz, radius);
                    intersecti = sphere_intersect  (&t1, &t2, x,y,z, vx,vy,vz, line_info.radius_i);
                }
                #ifdef USE_OFF
                else if (line_info.shape == 3) {
                    intersect  = off_intersect  (&t0, &t3, NULL, NULL, x,y,z, vx,vy,vz, 0, 0, 0, thread_offdata);
                    intersecti = 0;
                }
                #endif

                if (!intersect) {
                    /* Strange error: did not hit cylinder */
                    if (line_info.flag_warning < 10)
                    printf("PowderN: %s: WARNING: Did not hit sample from inside (coh). ABSORB.\n", NAME_CURRENT_COMP);
                    line_info.flag_warning++;
                    ABSORB;
                }

                if (!intersecti) {
                    t1 = (t3 + t0) /2;
                    t2 = t1;
                }

                if (concentric && intersecti) {
                    /* In case of concentricity, 'remove' backward wall of sample */
                    t2 = t1;
                    t3 = t1;
                }

                if(t0 < 0) t0=0; /* already in sample */
                if(t1 < 0) t1=0; /* already in inner hollow */
                if(t2 < 0) t2=0; /* already past inner hollow */


                l_1 = v*(t3 - t2 + t1 - t0); /* Length to exit */

                pmul  *= Nq*l_full*my_s_n*exp(-(line_info.my_a_v/v+my_s)*(l+l_1))
                                        /(1-(p_inc+p_transmit));
                /* Correction in case of d_phi focusing - BUT only when d_phi != 0 */
                if (d_phi_thread) {
                pmul *= alpha/PI;
                if (tth_sign) pmul *=0.5;
                }


                type = 'c';
                itype = 1;
                dq = line_info.q_v[line]*V2K;
                lfree=1/(line_info.my_a_v/v+my_s);
            } /* else transmit <-- No powder lines in file */

        }  /* Coherent scattering event */

        else if (neutrontype == 2) {  /* Make incoherent scattering event */
            if (d_omega && d_phi_thread) {
                randvec_target_rect_angular(&vx, &vy, &vz, &solid_angle,
                tgt_x, tgt_y, tgt_z, d_omega*DEG2RAD, d_phi_thread*DEG2RAD, ROT_A_CURRENT_COMP);
            }
            else if (d_phi_thread) {
                randvec_target_rect_angular(&vx, &vy, &vz, &solid_angle,
                                        tgt_x, tgt_y, tgt_z,
                                        2*PI, d_phi_thread*DEG2RAD, ROT_A_CURRENT_COMP);
            }
            else {
            randvec_target_circle(&vx, &vy, &vz,
                                    &solid_angle, 0, 0, 1, 0);
            }
            v1 = sqrt(vx*vx+vy*vy+vz*vz);
            vx *= v/v1;
            vy *= v/v1;
            vz *= v/v1;

            /* Since now scattered and new direction given, calculate path to exit */
            if (line_info.shape == 1) {
                intersect  = box_intersect(&t0, &t3, x, y, z, vx, vy, vz, xwidth, yheight, zdepth);
                intersecti = box_intersect(&t1, &t2, x, y, z, vx, vy, vz, line_info.xwidth_i, line_info.yheight_i, line_info.zdepth_i);
            } else if (line_info.shape == 0) {
                intersect  = cylinder_intersect(&t0, &t3, x, y, z, vx, vy, vz, radius, yheight);
                intersecti = cylinder_intersect(&t1, &t2, x, y, z, vx, vy, vz, line_info.radius_i, line_info.yheight_i);
            } else if (line_info.shape == 2) {
                intersect  = sphere_intersect  (&t0, &t3, x,y,z, vx,vy,vz, radius);
                intersecti = sphere_intersect  (&t1, &t2, x,y,z, vx,vy,vz, line_info.radius_i);
            }
            #ifdef USE_OFF
            else if (line_info.shape == 3) {
                intersect  = off_intersect  (&t0, &t3, NULL, NULL, x,y,z, vx,vy,vz, 0, 0, 0, thread_offdata);
                intersecti = 0;
            }
            #endif

            if (!intersect) {
                /* Strange error: did not hit cylinder */
                if (line_info.flag_warning < 10)
                    printf("PowderN: %s: WARNING: Did not hit sample from inside (inc). ABSORB.\n", NAME_CURRENT_COMP);
                line_info.flag_warning++;
                ABSORB;
            }

            if (!intersecti) {
                t1 = (t3 + t0) /2;
                t2 = t1;
            }

            if (concentric && intersecti) {
                /* In case of concentricity, 'remove' backward wall of sample */
                t2 = t1;
                t3 = t1;
            }

            if(t0 < 0) t0=0; /* already in sample */
            if(t1 < 0) t1=0; /* already in inner hollow */
            if(t2 < 0) t2=0; /* already past inner hollow */


            l_1 = v*(t3 - t2 + t1 - t0); /* Length to exit */

            pmul *= l_full*line_info.my_inc*exp(-(line_info.my_a_v/v+my_s)*(l+l_1))/(p_inc);
            pmul *= solid_angle/(4*PI);
            lfree=1/(line_info.my_a_v/v+my_s);
            type = 'i';
            itype = 2;

        }  /* Incoherent scattering event */

        else if (neutrontype == 1) {
                /* Make transmitted (absorption-corrected) event */
                /* No coordinate changes here, simply change neutron weight */
                pmul *= exp(-(line_info.my_a_v/v+my_s)*(l))/(p_transmit);
                lfree=1/(line_info.my_a_v/v+my_s);
                type = 't';
                itype = 3;
            }
        p *= pmul;
        } /* Neutron leaving since it has passed already */
    } /* else transmit non interacting neutrons */

    // Inject these back to global struct in non-OpenACC case
    #ifndef OPENACC
    line_info.nb_reuses=nb_reuses;
    line_info.nb_refl=nb_refl;
    line_info.nb_refl_count=nb_refl_count;
    line_info.v=vcache; 
    line_info.Nq=Nq;
    line_info.v_min=v_min;
    line_info.v_max=v_max;
    line_info.lfree=lfree;
    line_info.xs_compute=xs_compute;
    line_info.xs_reuse=xs_reuse;
    line_info.xs_calls=xs_calls;
    line_info.dq=dq;
    #endif


    ////////////////////////////////////////////////////////////////
    #undef reflections
    #undef geometry
    #undef format
    #undef radius
    #undef yheight
    #undef xwidth
    #undef zdepth
    #undef thickness
    #undef pack
    #undef Vc
    #undef sigma_abs
    #undef sigma_inc
    #undef delta_d_d
    #undef p_inc
    #undef p_transmit
    #undef DW
    #undef nb_atoms
    #undef d_omega
    #undef d_phi
    #undef tth_sign
    #undef p_interact
    #undef concentric
    #undef density
    #undef weight
    #undef barns
    #undef Strain
    #undef focus_flip
    #undef target_index

    #undef line_info
    #undef columns
    #undef offdata
    #undef tgt_x
    #undef tgt_y
    #undef tgt_z

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

void Save_PowderN(PowderN *comp) {

}

void Finally_PowderN(PowderN *comp) {

    #define reflections comp->reflections
    #define geometry comp->geometry
    #define format comp->format
    #define radius comp->radius
    #define yheight comp->yheight
    #define xwidth comp->xwidth
    #define zdepth comp->zdepth
    #define thickness comp->thickness
    #define pack comp->pack
    #define Vc comp->Vc
    #define sigma_abs comp->sigma_abs
    #define sigma_inc comp->sigma_inc
    #define delta_d_d comp->delta_d_d
    #define p_inc comp->p_inc
    #define p_transmit comp->p_transmit
    #define DW comp->DW
    #define nb_atoms comp->nb_atoms
    #define d_omega comp->d_omega
    #define d_phi comp->d_phi
    #define tth_sign comp->tth_sign
    #define p_interact comp->p_interact
    #define concentric comp->concentric
    #define density comp->density
    #define weight comp->weight
    #define barns comp->barns
    #define Strain comp->Strain
    #define focus_flip comp->focus_flip
    #define target_index comp->target_index

    #define line_info comp->line_info
    #define columns comp->columns
    #define offdata comp->offdata
    #define tgt_x comp->tgt_x
    #define tgt_y comp->tgt_y
    #define tgt_z comp->tgt_z
    ////////////////////////////////////////////////////////////////


    free(line_info.list);
    free(line_info.q_v);
    free(line_info.w_v);
    free(line_info.my_s_v2);

    if (line_info.flag_warning)
        printf("PowderN: %s: Error messages were repeated %i times with absorbed neutrons.\n",
        NAME_CURRENT_COMP, line_info.flag_warning);

    /* in case this instance is used in a SPLIT, we can recommend the optimal iteration value */
    if (line_info.nb_refl_count) {
        double split_iterations = (double)line_info.nb_reuses/line_info.nb_refl_count + 1;
        double split_optimal    = (double)line_info.nb_refl/line_info.nb_refl_count;
        if (split_optimal > split_iterations + 5)
            printf("PowderN: %s: Info: you may highly improve the computation efficiency by using\n"
                "    SPLIT %i COMPONENT %s=PowderN(...)\n"
                "  in the instrument description %s.\n",
                NAME_CURRENT_COMP, (int)split_optimal, NAME_CURRENT_COMP, instrument_source);
    }


    ////////////////////////////////////////////////////////////////
    #undef reflections
    #undef geometry
    #undef format
    #undef radius
    #undef yheight
    #undef xwidth
    #undef zdepth
    #undef thickness
    #undef pack
    #undef Vc
    #undef sigma_abs
    #undef sigma_inc
    #undef delta_d_d
    #undef p_inc
    #undef p_transmit
    #undef DW
    #undef nb_atoms
    #undef d_omega
    #undef d_phi
    #undef tth_sign
    #undef p_interact
    #undef concentric
    #undef density
    #undef weight
    #undef barns
    #undef Strain
    #undef focus_flip
    #undef target_index

    #undef line_info
    #undef columns
    #undef offdata
    #undef tgt_x
    #undef tgt_y
    #undef tgt_z
}

void Display_PowderN(PowderN *comp) {
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

    #define reflections comp->reflections
    #define geometry comp->geometry
    #define format comp->format
    #define radius comp->radius
    #define yheight comp->yheight
    #define xwidth comp->xwidth
    #define zdepth comp->zdepth
    #define thickness comp->thickness
    #define pack comp->pack
    #define Vc comp->Vc
    #define sigma_abs comp->sigma_abs
    #define sigma_inc comp->sigma_inc
    #define delta_d_d comp->delta_d_d
    #define p_inc comp->p_inc
    #define p_transmit comp->p_transmit
    #define DW comp->DW
    #define nb_atoms comp->nb_atoms
    #define d_omega comp->d_omega
    #define d_phi comp->d_phi
    #define tth_sign comp->tth_sign
    #define p_interact comp->p_interact
    #define concentric comp->concentric
    #define density comp->density
    #define weight comp->weight
    #define barns comp->barns
    #define Strain comp->Strain
    #define focus_flip comp->focus_flip
    #define target_index comp->target_index

    #define line_info comp->line_info
    #define columns comp->columns
    #define offdata comp->offdata
    #define tgt_x comp->tgt_x
    #define tgt_y comp->tgt_y
    #define tgt_z comp->tgt_z
    ////////////////////////////////////////////////////////////////


    if (line_info.V_0)
    {

        if (line_info.shape == 0) { /* cyl */
            circle((char*) "xz", 0,  yheight/2.0, 0, radius);
            circle((char*)"xz", 0, -yheight/2.0, 0, radius);
            line(-radius, -yheight/2.0, 0, -radius, +yheight/2.0, 0);
            line(+radius, -yheight/2.0, 0, +radius, +yheight/2.0, 0);
            line(0, -yheight/2.0, -radius, 0, +yheight/2.0, -radius);
            line(0, -yheight/2.0, +radius, 0, +yheight/2.0, +radius);
            if (thickness) {
                double radius_i=radius-thickness;
                circle((char*)"xz", 0,  yheight/2.0, 0, radius_i);
                circle((char*)"xz", 0, -yheight/2.0, 0, radius_i);
                line(-radius_i, -yheight/2.0, 0, -radius_i, +yheight/2.0, 0);
                line(+radius_i, -yheight/2.0, 0, +radius_i, +yheight/2.0, 0);
                line(0, -yheight/2.0, -radius_i, 0, +yheight/2.0, -radius_i);
                line(0, -yheight/2.0, +radius_i, 0, +yheight/2.0, +radius_i);
            }
        }
        else if (line_info.shape == 1) {  /* box */
            double xmin = -0.5*xwidth;
            double xmax =  0.5*xwidth;
            double ymin = -0.5*yheight;
            double ymax =  0.5*yheight;
            double zmin = -0.5*zdepth;
            double zmax =  0.5*zdepth;
            multiline(5, xmin, ymin, zmin,
                        xmax, ymin, zmin,
                        xmax, ymax, zmin,
                        xmin, ymax, zmin,
                        xmin, ymin, zmin);
            multiline(5, xmin, ymin, zmax,
                        xmax, ymin, zmax,
                        xmax, ymax, zmax,
                        xmin, ymax, zmax,
                        xmin, ymin, zmax);
            line(xmin, ymin, zmin, xmin, ymin, zmax);
            line(xmax, ymin, zmin, xmax, ymin, zmax);
            line(xmin, ymax, zmin, xmin, ymax, zmax);
            line(xmax, ymax, zmin, xmax, ymax, zmax);
            if (line_info.zdepth_i) {
                xmin = -0.5*line_info.xwidth_i;
                xmax =  0.5*line_info.xwidth_i;
                ymin = -0.5*line_info.yheight_i;
                ymax =  0.5*line_info.yheight_i;
                zmin = -0.5*line_info.zdepth_i;
                zmax =  0.5*line_info.zdepth_i;
                multiline(5, xmin, ymin, zmin,
                        xmax, ymin, zmin,
                        xmax, ymax, zmin,
                        xmin, ymax, zmin,
                        xmin, ymin, zmin);
                multiline(5, xmin, ymin, zmax,
                        xmax, ymin, zmax,
                        xmax, ymax, zmax,
                        xmin, ymax, zmax,
                        xmin, ymin, zmax);
                line(xmin, ymin, zmin, xmin, ymin, zmax);
                line(xmax, ymin, zmin, xmax, ymin, zmax);
                line(xmin, ymax, zmin, xmin, ymax, zmax);
                line(xmax, ymax, zmin, xmax, ymax, zmax);
            }
        }
        if (line_info.shape == 2) { /* sphere */
            if (line_info.radius_i) {
                circle((char*)"xy",0,0,0,line_info.radius_i);
                circle((char*)"xz",0,0,0,line_info.radius_i);
                circle((char*)"yz",0,0,0,line_info.radius_i);
            }
            circle((char*)"xy",0,0,0,radius);
            circle((char*)"xz",0,0,0,radius);
            circle((char*)"yz",0,0,0,radius);
        }
        else if (line_info.shape == 3) {	/* OFF file */
            off_display(offdata);
        }
    }


    ////////////////////////////////////////////////////////////////
    #undef reflections
    #undef geometry
    #undef format
    #undef radius
    #undef yheight
    #undef xwidth
    #undef zdepth
    #undef thickness
    #undef pack
    #undef Vc
    #undef sigma_abs
    #undef sigma_inc
    #undef delta_d_d
    #undef p_inc
    #undef p_transmit
    #undef DW
    #undef nb_atoms
    #undef d_omega
    #undef d_phi
    #undef tth_sign
    #undef p_interact
    #undef concentric
    #undef density
    #undef weight
    #undef barns
    #undef Strain
    #undef focus_flip
    #undef target_index

    #undef line_info
    #undef columns
    #undef offdata
    #undef tgt_x
    #undef tgt_y
    #undef tgt_z

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

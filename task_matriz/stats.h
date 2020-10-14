/*
        Grupo: 8
        Nome: Abner Eduardo Silveira Santos             NUSP: 10692012
        Nome: Gyovana Mayara Moriyama                   NUSP: 10734387
        Nome: Henrique Matarazo Camillo                 NUSP: 10294943
        Nome: Vitor Augusto de Oliveira                 NUSP: 9360815
*/

#ifndef _STATS_H_
#define _STATS_H_

typedef struct {
    double *avgs; 
    double *harms; 
    double *medians; 
    double *modes; 
    double *variances; 
    double *std_deviations; 
    double *var_coefs; 
    int size;
} StatsMetrics;

StatsMetrics create_stats_metrics(int ncols);
void deallocate_StatsMetrics(StatsMetrics metrics);
double average(double *vec, int c);
double harmonic(double *vec, int size);
int compare(const void *a, const void *b);
double median(double *vec, int size);
double variance(double *vec, int size, double avg);
int mode(double *vec, int size);
void print_all_metrics(StatsMetrics metrics);

#endif
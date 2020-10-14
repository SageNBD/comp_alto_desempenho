/*
        Grupo: 8
        Nome: Abner Eduardo Silveira Santos             NUSP: 10692012
        Nome: Gyovana Mayara Moriyama                   NUSP: 10734387
        Nome: Henrique Matarazo Camillo                 NUSP: 10294943
        Nome: Vitor Augusto de Oliveira                 NUSP: 9360815
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "stats.h"

/*
 * StatsMetrics create_stats_metrics(int ncols)
 * 
 * Allocate memory for each metric result vector
 * 
 * @param   int             ncols       size of array
 * 
 * @return  StatsMetrics    metrics     Struct containing all metrics
 */
StatsMetrics create_stats_metrics(int ncols) 
{
    StatsMetrics metrics;

    metrics.size = ncols;
    metrics.avgs = (double *) malloc(sizeof(double) * ncols); 
    metrics.harms = (double *) malloc(sizeof(double) * ncols); 
    metrics.medians = (double *) malloc(sizeof(double) * ncols); 
    metrics.modes = (double *) malloc(sizeof(double) * ncols); 
    metrics.variances = (double *) malloc(sizeof(double) * ncols); 
    metrics.std_deviations = (double *) malloc(sizeof(double) * ncols); 
    metrics.var_coefs = (double *) malloc(sizeof(double) * ncols); 

    return metrics;
}

/*
 * void deallocate_StatsMetrics(StatsMetrics metrics)
 * 
 * Dealllocates memory for each metric result vector
 * 
 * @param  StatsMetrics    metrics     Struct containing all metrics
 * 
 * @return None
 */
void deallocate_StatsMetrics(StatsMetrics metrics) 
{
    free(metrics.avgs);
    free(metrics.harms);
    free(metrics.medians);
    free(metrics.modes);
    free(metrics.variances);
    free(metrics.std_deviations);
    free(metrics.var_coefs);
}

/*
 * double average(int *vec, int c)
 * 
 * Gets arithmetic mean of a vector of size 'c'
 * 
 * @param   int*    vec     vector of integers
 * @param   int     size    number of elements
 * 
 * @return  double  avg     average of each element
 */
double average(double *vec, int size) 
{
    double sum = 0;
    for (int i = 0; i < size; ++i)
        sum += vec[i];
    return sum / size;
}

/*
 * double harmonic(int *vec, int size)
 * 
 * Gets harmonic mean of elements in a vector
 * 
 * @param   int*    vec     vector of integers
 * @param   int     size    number of elements
 * 
 * @return  double  hmean   harmonic mean of elements
 */
double harmonic(double *vec, int size) 
{
    double sum = 0;
    for (int i = 0; i < size; ++i)
    {
        if (vec[i] == 0) return 0;
        sum += 1 /  vec[i];
    }
    return  size / sum;
}

/* 
 * int compare(const void *a, const void *b)
 * 
 * Utility method for qsort
 */
int compare(const void *a, const void *b)
{
    return (*(double *)a - *(double *)b );
}

/*
 * double median(int *vec, int size)
 * 
 * Sorts the array, gets center element to get the median
 * 
 * @param   int*    vec     vector of integers
 * @param   int     size    number of elements
 * 
 * @return  double  median  central element of sorted array
 */
double median(double *vec, int size) 
{
    qsort(vec, size, sizeof(double), compare);
    return size % 2 != 0 ? vec[size / 2] : (vec[size / 2] + vec[(size/2)-1])/2; // Handles even arrays
}

/*
 * double variance(int *vec, int size, double avg)
 * 
 * Gets the variance, given the pre-calculated average
 * 
 * @param   int*    vec     vector of integers
 * @param   int     size    number of elements
 * @param   double  avg     average of array
 * 
 * @return  double  variance  variance of given array
 */
double variance(double *vec, int size, double avg)
{
    double sum = 0;
    for(int i = 0; i < size; i++)
        sum += pow(vec[i] - avg, 2);

    return sum / (size - 1);
}

/*
 * double mode(int *vec, int size)
 * 
 * Gets mode, treats input as integers
 * 
 * @param   int*    vec     vector of integers
 * @param   int     size    number of elements
 * 
 * @return  int     mode    most frequent value in array
 */
int mode(double *vec, int size) 
{
    int num_elements = 0;
    for (int i = 0; i < size; ++i) 
        if ((int)vec[i] > num_elements)
            num_elements = (int)vec[i];

    // int *freq = (int *) calloc(num_elements, sizeof(int)); // Array to indicate the frequency of a value
    // int *pos = (int *) calloc(num_elements, sizeof(int)); // Array to indicate the very first time a value was found
    
    int freq[num_elements];
    int pos[num_elements];

    memset(freq, 0, sizeof(freq));
    memset(pos, 0, sizeof(pos));

    int max = 0, current_mode;
    for (int i = 0; i < size; ++i) 
    {
        if (pos[(int)vec[i] - 1] == 0)
            pos[(int)vec[i] - 1] = i + 1;

        // If two values are competing for being the mode, get the one that appeared first in the array
        if (++freq[(int)vec[i] - 1] == max && pos[(int)vec[i] - 1] < pos[current_mode])
        {
            max = freq[(int)vec[i] - 1];
            current_mode = vec[i];
        }
        else if (freq[(int)vec[i] - 1] > max) 
        {                
            max = freq[(int)vec[i] - 1];
            current_mode = (int)vec[i];
        }
    }
    
    // free(freq);
    // free(pos);

    return (max != 1 ? current_mode : -1); // If no element is ever repeated, return -1
}

void print_metric(double *metric, int size)
{
    for (int i = 0; i < size; i++)
        printf("%.1lf ", round(metric[i] * 10) / 10);
    printf("\n");
}

/*
 * void print_all_metrics(StatsMetrics metrics)
 * 
 * Prints every single metric 
 * 
 * @param  StatsMetrics    metrics     Struct containing all metrics
 * 
 * @return None
 */
void print_all_metrics(StatsMetrics metrics)
{
    print_metric(metrics.avgs, metrics.size);
    print_metric(metrics.harms, metrics.size);
    print_metric(metrics.medians, metrics.size);
    print_metric(metrics.modes, metrics.size);
    print_metric(metrics.variances, metrics.size);
    print_metric(metrics.std_deviations, metrics.size);
    print_metric(metrics.var_coefs, metrics.size);
}
/*
        Grupo: 8
        Nome: Abner Eduardo Silveira Santos             NUSP: 10692012
        Nome: Gyovana Mayara Moriyama                   NUSP: 10734387
        Nome: Henrique Matarazo Camillo                 NUSP: 10294943
        Nome: Vitor Augusto de Oliveira                 NUSP: 9360815
*/

#ifndef _MATRIX_H_
#define _MATRIX_H_

double **alloc_matrix(int r, int c);
double **copy_matrix(double **mat, int r, int c);
void destroy_matrix(int r, void **mat);

#endif

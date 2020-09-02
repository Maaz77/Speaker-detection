
//  feature matching.c
//  Final_Speaker_detection
//
//  Created by MAAZ on 8/31/20.
//  Copyright © 2020 MAAZ. All rights reserved.


#include "feature matching.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#define e 2.7182818284590452353602874713527

#define N 9

void printmatrix(int n, int m ,float   a[n][m] ){
    for(int i =0 ; i < n ; i ++) {
        for (int j =0 ; j < m ; j++) {
            printf("%f, ",a[i][j]);
        }
        printf("\n\n");
    }
}

void printvector(int n, float a[n]){
    printf("array : ");
    for (int i = 0 ; i < n ; i++) printf("%f\n",a[i]);
    printf("\n");
}

// Function to get cofactor of A[p][q] in temp[][]. n is current
// dimension of A[][]
void getCofactor(float A[N][N], float temp[N][N], int p, int q, int n)
{
    int i = 0, j = 0;
    
    // Looping for each element of the matrix
    for (int row = 0; row < n; row++)
    {
        for (int col = 0; col < n; col++)
        {
            //  Copying into temporary matrix only those element
            //  which are not in given row and column
            if (row != p && col != q)
            {
                temp[i][j++] = A[row][col];
                
                // Row is filled, so increase row index and
                // reset col index
                if (j == n - 1)
                {
                    j = 0;
                    i++;
                }
            }
        }
    }
}

/* Recursive function for finding determinant of matrix.
 n is current dimension of A[][]. */
float determinant(float A[N][N], int n)
{
    int D = 0; // Initialize result
    
    //  Base case : if matrix contains single element
    if (n == 1)
        return A[0][0];
    
    float temp[N][N]; // To store cofactors
    
    int sign = 1;  // To store sign multiplier
    
    // Iterate for each element of first row
    for (int f = 0; f < n; f++)
    {
        // Getting Cofactor of A[0][f]
        getCofactor(A, temp, 0, f, n);
        D += sign * A[0][f] * determinant(temp, n - 1);
        
        // terms are to be added with alternate sign
        sign = -sign;
    }
    
    return D;
}

// Function to get adjoint of A[N][N] in adj[N][N].
void adjoint(float A[N][N],float adj[N][N])
{
    if (N == 1)
    {
        adj[0][0] = 1;
        return;
    }
    
    // temp is used to store cofactors of A[][]
    int sign = 1;
    float temp[N][N];
    
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
        {
            // Get cofactor of A[i][j]
            getCofactor(A, temp, i, j, N);
            
            // sign of adj[j][i] positive if sum of row
            // and column indexes is even.
            sign = ((i+j)%2==0)? 1: -1;
            
            // Interchanging rows and columns to get the
            // transpose of the cofactor matrix
            adj[j][i] = (sign)*(determinant(temp, N-1));
        }
    }
}

// Function to calculate and store inverse, returns false if
// matrix is singular
int inverse(float A[N][N], float inverse[N][N])
{
    // Find determinant of A[][]
    //printf("adjoint");
    int det = determinant(A, N);
    if (det == 0)
    {
        printf("Singular matrix, can't find its inverse\n");
        return 0;
    }
    
    // Find adjoint
    float adj[N][N];
    //printf("adjoint");
    adjoint(A, adj);
    
    // Find Inverse using formula "inverse(A) = adj(A)/det(A)"
    for (int i=0; i<N; i++)
        for (int j=0; j<N; j++)
            inverse[i][j] = adj[i][j]/det;
    
    return 1;
}



void matrix_mul(float matrix[N][N], float *vector,float * rslt){
    float sum=0;
    for (int i = 0 ; i <N ; i++){
        sum = 0;
        for(int j =0 ; j < N ; j ++){
            sum+=vector[j] * matrix[i][j];
        }
        rslt[i] = sum;
    }
}

float vector_mul(float * a, float * b  ){
    float rslt = 0;
    for(int i =0 ; i < N ; i ++) rslt += a[i] * b[i];
    return rslt;
    
}



void minus(int len,float arr1[len], float arr2[len]){
    int i;
    for(i=0;i<len;i++){
        arr1[i]-=arr2[i];
    }
}

void sum_matrix(int len,float arr1[len][len], float arr2[len][len]){
    int i;
    int j;
    for (i=0;i<len;i++){
        for(j=0;j<len;j++){
            arr1[i][j]+=arr2[i][j];
        }
    }
}




void avg_trnsp(int coeff,float arr[coeff],float final[coeff][coeff]){
  
    for (int i=0;i<coeff;i++){
        for(int j=0;j<coeff;j++){
            final[i][j]=arr[i]*arr[j];
        }
    }
    
}







float bayesian_classifier(int frame1,int frame2, int coeff,float source[frame1][coeff],float sample [frame2][coeff]){
    printf("bayesian classifier started :");
//
    //printmatrix(frame1,coeff,source);
//    printf("\n\n\n\n\nsample : \n");
//    printmatrix(frame2,coeff,sample);
    
    int i;
    int j;
    float * average = (float * ) malloc(sizeof(float)*coeff);
    // float **trnsp_mult= (float **) malloc(sizeof(float*) * coeff * coeff);
    float trnsp_mult[13][13];
    
    //float ** Sum = (float **) malloc(sizeof(float *) * coeff * coeff);
    
    float Sum[coeff][coeff];
    float Sum_inv[coeff][coeff];
    //float temp_vertical[coeff][1];
    //float temp_horizontal[1][coeff];
    float temp_horizontale1[coeff];
    float rslt;
    for(i=0;i<coeff;i++){
        for(j=0;j<coeff;j++){
            Sum[i][j]=0.0;
        }
    }
    
    float sum;
    for (i=0;i<coeff;i++){
        sum=0;
        for (j=0;j<frame1;j++){
            
            sum+=source[j][i];
        }
        average[i]=sum/frame1;
    }
//    printf("\naverage\n");
//    printvector(frame1,average);
    for (i=0;i<frame1;i++){
        minus(coeff, source[i], average);
    }
//    printf("\n\nsource\n\n:");
//    printmatrix(frame1,coeff,source);
    for(i=0;i<frame1;i++){
        //printvector(coeff,source[i]);
        avg_trnsp(coeff, source[i], trnsp_mult);
        //printf("\n\n\n\n trnsp_mult:\n\n");
        //printmatrix(coeff, coeff, trnsp_mult);
        sum_matrix(coeff, Sum, trnsp_mult);
    }
    for (i=0;i<coeff;i++){
        for (j=0;j<coeff;j++){
            Sum[i][j]/=frame1-1;
            
        }
    }
//    printf("\n\n\nSUM \n\n\n");
//    printmatrix(9, 9, Sum);
    inverse(Sum, Sum_inv);
    
    sum=0;
    
    
    for(i=0;i<frame2;i++){
        
        minus(coeff, sample[i], average);
        
        
        matrix_mul(Sum_inv, sample[i], temp_horizontale1);
        rslt = vector_mul(sample[i], temp_horizontale1);
        
        //printf("\nrslt : %f\n",rslt);
        sum=sum+exp(-0.5*rslt);
    }
    //printf("\n%f sum \n",sum);
    return sum;
    
    
}




/* Cheung Chi Wang 1155158048*/
/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M==32){         // can store 8 rows of the matrix to cache without eviction
        int a, b, c, d, e, f, g, h;
        int m, n, i;
        for (n = 0; n < N; n += 8) {            // 8x8 blocking
            for (m = 0; m < M; m += 8) {
                for (i = n; i < n + 8; i++) {
                    a = A[i][m];
                    b = A[i][m+1];
                    c = A[i][m+2];
                    d = A[i][m+3];
                    e = A[i][m+4];
                    f = A[i][m+5];
                    g = A[i][m+6];
                    h = A[i][m+7];

                    B[m][i] = a;
                    B[m+1][i] = b;
                    B[m+2][i] = c;
                    B[m+3][i] = d;
                    B[m+4][i] = e;
                    B[m+5][i] = f;
                    B[m+6][i] = g;
                    B[m+7][i] = h;
                }
            }
        }
        /*if (is_transpose(M, N, A, B)!=1)
            printf("Not transposed correctly!");*/
    }

    else if (M==64) {         // can store 4 rows of matrix to cache without eviction
        int m,n,i;
        int a,b,c,d,e,f,g,h;
        for(n=0;n<N;n+=8){      // using 4x4 blocking inside 8x8 blocking
            for(m=0;m<M;m+=8){
                for(i=m;i<m+4;i++){
                    a=A[i][n];      // get 8 elements from ith row of A
                    b=A[i][n+1];
                    c=A[i][n+2];
                    d=A[i][n+3];
                    e=A[i][n+4];
                    f=A[i][n+5];
                    g=A[i][n+6];
                    h=A[i][n+7];

                    B[n][i]=a;      // store the elements to two 4x4 block
                    B[n+1][i]=b;
                    B[n+2][i]=c;
                    B[n+3][i]=d;

                    B[n][i+4]=e;
                    B[n+1][i+4]=f;
                    B[n+2][i+4]=g;
                    B[n+3][i+4]=h;
                }
                for(i=n;i<n+4;i++){
                    a=B[i][m+4];      // store the right top 4x4 block
                    b=B[i][m+5];
                    c=B[i][m+6];
                    d=B[i][m+7];

                    e=A[m+4][i];      // store more element from A
                    f=A[m+5][i];
                    g=A[m+6][i];
                    h=A[m+7][i];

                    B[i][m+4]=e;    // put the elements to right top 4x4 block
                    B[i][m+5]=f;
                    B[i][m+6]=g;
                    B[i][m+7]=h;

                    B[i+4][m]=a;    //put the right top 4x4 block to left bottom
                    B[i+4][m+1]=b;
                    B[i+4][m+2]=c;
                    B[i+4][m+3]=d;
                }
                for(i=n+4;i<n+8;i++){
                    a=A[m+4][i];    // assign the last 4x4 block of elements from A to B
                    b=A[m+5][i];
                    c=A[m+6][i];
                    d=A[m+7][i];

                    B[i][m+4]=a;
                    B[i][m+5]=b;
                    B[i][m+6]=c;
                    B[i][m+7]=d;
                }
            }
        }
        /*if (is_transpose(M, N, A, B)!=1)
          printf("Not transposed correctly!");*/
    }
    
    else if (M==61) {
        int n, m, i, j;
        for (n = 0; n < N; n += 17) {   // use 17x17 blocking
            for (m = 0; m < M; m += 17) {
                for (i = n; i < N && i < n + 17; i++) {
                    for (j = m; j < M && j < m + 17; j++) {
                        {
                            B[j][i] = A[i][j];
                        }
                    }
                }
            }
        }
        /*if (is_transpose(M, N, A, B)!=1)
            printf("Not transposed correctly!");*/
    }
}



/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

char trans_desc_1[]= "8x8 blocking trial";
void trans1(int M, int N, int A[N][M], int B[M][N]){
    int i, j, col, row;
    for (i = 0; i < N; i += 8)
        for (j = 0; j < M; j += 8)
            for (row = i; col < i + 8; col++)
                for (col = j; row < j + 8; row++)
                {
                    B[col][row] = A[row][col];
                }
}


/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
    //registerTransFunction(trans1, trans_desc_1);


}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}


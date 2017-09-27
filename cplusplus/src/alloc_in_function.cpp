#include <stdio.h>
#include <math.h>
#include <malloc.h>

//using namespace std;

/*
F(k) = \sum _0 ^{N-1} f(n) e^{-i \frac{2 \pi} {N} n k}
     = \sum _0 ^{N-1} f(n) \cos(\frac{2 \pi}{N}nk) - i \sum _0 ^{N-1} f(n) \sin(\frac{2 \pi}{N} nk)

     k=0,1,2,...,N-1
*/

// f: input sequence, N: length of sequence, M: the magnitude, A: the angle.
int dft_1D(float* f, int N, float **M, float **A)
{
    const float pi = 3.1415926;
    float alpha = 2.0 * pi / (float)N;

    *M = (float*) malloc(N*sizeof(float));
    *A = (float*) malloc(N*sizeof(float));

    float re, im;
    int k,n;
    for(k=0; k<N; k++)
    {
        for(n=0; n<N; n++)
        {
            re += f[n] * cos(alpha * n * k);
            im += -f[n] * sin(alpha * n * k);
        }
        (*M)[k] = sqrt(re*re+im*im);
        (*A)[k] = atan2(im, re);
    }
    return 1;
}

int main()
{
    int N = 1000;
    float *f = (float*) malloc(N*sizeof(float));
    int i;
    for(i=0; i<N; i++)
    {
        if(i<200)
            f[i] = 1.0;
        else
            f[i] = 0.0;
    }

    float *M,*A;
    dft_1D(f, N, &M, &A);

    FILE * file_out;
    file_out = fopen("out.csv", "w");
    for(i=0; i<N; i++)
    {
        fprintf(file_out, "%f,", M[i]);
        fprintf(file_out, "%f\n", A[i]);
    }
    fclose(file_out);

    printf("Done!\n");

    return 0;
}

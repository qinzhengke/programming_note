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

    float re=0, im=0;
    int k,n;
    for(k=0; k<N; k++)
    {

        for(re=0,im=0,n=0; n<N; n++)
        {
            re += f[n] * cos(alpha * (float)n * (float)k);
            im += -f[n] * sin(alpha * (float)n * (float)k);
        }
        (*M)[k] = sqrt(re*re+im*im);
        (*A)[k] = atan2(im, re);
    }
    return 1;
}

// Input--> N: lenght, M: magnitude, phi: angle, f: frequency
// Output--> x:
int gen_sin(int N, float M, float phi, float f, float **x)
{
    *x = (float*) malloc(N*sizeof(float));
    int i;
    const float pi = 3.1415926;
    float omega = 2*pi*f;
    for(i=0; i<N; i++)
    {
        (*x)[i] = M * sin( omega*(float)i + phi );
    }

    return 1;
}

int save_seq(float *x, int N, char * file_name)
{
    FILE * file_out;
    file_out = fopen(file_name, "w");
    int i;
    for(i=0; i<N; i++)
    {
        fprintf(file_out, "%f", x[i]);
        if(i<N-1)
            fprintf(file_out, ",");
        else
            fprintf(file_out, "\n");
    }
    fclose(file_out);

    return 1;
}

int main()
{
    int N = 1000;
    float *x;
    gen_sin(N, 1.0, 0.0, 0.001, &x);
//    save_seq(x, N, "x.csv");

    float *M,*A;
    dft_1D(x, N, &M, &A);
    save_seq(M,N, "F.csv");


    return 0;
}

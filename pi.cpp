// calculate pi

#include <stdio.h>
#include <math.h>
#include <omp.h>


#define LIM 1000000
#define INC 10

double f(double x)
    {
    return 4/(1+x*x);
    }


int main()
    {
    double best = 0;
    double besterror = 1000;
    int bestres = 0;

    #pragma omp parallel
        {
        int id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        for(int res=id*INC; res<=LIM; res += num_threads*INC)
            {
            double sum = 0;
            double dx = 1.0/res;

            for(int i=0; i<res; i++)
                {
                sum += (f(i*dx) + f((i+1)*dx))/2.0 * dx;
                }
  
            double e = abs(sum - M_PI);

            #pragma omp critical
            if(e < besterror)
                {
                best = sum;
                besterror = e;
                bestres = res;

                printf("%d %6d %2.16lf %2.16lf\n", id, res, sum, sum-M_PI);
                }
            }
        }

    return 0;
    }

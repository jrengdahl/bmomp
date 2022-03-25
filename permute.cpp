#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

const int COLORS = 4;
const int MAX = 64;
const int THREADS = 4096;


inline unsigned cget(int i, unsigned input)
    {
    return (input>>(i*8)) & 255;
    }

inline unsigned cdec(int i, unsigned input)
    {
    return input - (1<<(i*8));
    }


int plevel = MAX;
int output[MAX];
int permutations = 0;
bool verbose = false;
int maxthread = 0;
int ids[THREADS+10];


void permute(unsigned input, int (&output)[MAX], int colors, int step, int left)
    {
    if(left == 0)
        {
        if(verbose)
            {
            #pragma omp critical
                {        
                for(int i=0; i<step; i++)
                    {
                    if(verbose)printf("%d ", output[i]);
                    }
                ++permutations;
                printf(" (%d)\n", omp_get_thread_num());
                }
            }
        else
            #pragma omp atomic
                ++permutations;
        }
    else
        {
        for(int i=0; i<colors; i++)
            {
            if(cget(i, input) > 0)
                {
                output[step] = i;
                #pragma omp task firstprivate(output) if(step < plevel)
                    {
                    int id = omp_get_thread_num();
                    ids[id] = id;
                    permute(cdec(i, input), output, colors, step+1, left-1);
                    }
                }
            }
        #pragma omp taskwait
        }
    }




int main(int argc, char **argv)
    {
    int colors;
    int n;
    unsigned input;

    if(argc>1)colors=atoi(argv[1]);
    else colors=2;

    if(argc>2)n=atoi(argv[2]);
    else n=2;

    if(argc>3)plevel = atoi(argv[3]);

    if(argc>4)verbose=true;

    if(colors > COLORS || n*colors > MAX)
        {
        printf("too many\n");
        return 0;
        }

    input = 0;
    for(int i=0; i<colors; i++)
        {
        input |= n<<(i*8);
        }

    for(auto &x : output) x = 0;

    #pragma omp parallel num_threads(THREADS)
    #pragma omp single
    permute(input, output, colors, 0, colors*n);

    for(auto id: ids)maxthread = id>maxthread?id:maxthread;

    printf("permutations = %d\n", permutations);
    printf("maxthread = %d\n", maxthread);

    return 0;
    }

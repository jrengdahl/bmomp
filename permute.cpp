#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

const int COLORS = 4;               // max number of colors
const int MAX = 32;                 // max number of colors times steps
const int THREADS = 4096;           // max number of threads in the team


// return a color count from the input supply
inline unsigned cget(int i, unsigned input)
    {
    return (input>>(i*8)) & 255;
    }

// decrement a color in the input supply
inline unsigned cdec(int i, unsigned input)
    {
    return input - (1<<(i*8));
    }


int colors = 2;                     // the number of colors for this test
int plevel = MAX;                   // max level to make parallel
int permutations = 0;               // total number of permutations
bool verbose = false;               // whether to print each permutation
int maxthread = 0;                  // the highest thraed number that was spawned
int ids[THREADS+10];                // an array to record what threads ran



// compute permutations of colored balls.
// There may be multiple balls of each color.
// input: the number of balls left of each color. Maximum of four colors. Each byte of the word represents one color.
// output: a reference to an array of the balls that have been chosen so far.
// step: the depth of recursion, starts at zero and increments for each level descended.
// left: the number of balls remaining to be chosen. Starts at colors*n and decrements for each recursion.

void permute(unsigned input, unsigned char (&output)[MAX], int step, int left)
    {
    int id = omp_get_thread_num();                                  // record the thread number
    ids[id] = id;                                                   // for analysis only, it's not used in the permutation calculation

    if(left == 0)                                                   // if all the balls have been chosen
        {
        if(verbose)                                                 // if verbose, print out the permutaiton
            {
            #pragma omp critical
                {        
                for(int i=0; i<step; i++)
                    {
                    if(verbose)printf("%u ", output[i]);
                    }
                ++permutations;                                     // record the number of permutations
                printf(" (%d)\n", omp_get_thread_num());
                }
            }
        else                                                        // if not verbose, simply count the number of permutations
            #pragma omp atomic
                ++permutations;
        }
    else                                                            // if not done yet
        {
        for(int i=0; i<colors; i++)                                         // for each possible color
            {
            if(cget(i, input) > 0)                                          // if there are any left of that color
                {
                output[step] = i;                                           // pick a ball of that color
                #pragma omp task firstprivate(output) if(step < plevel)     // note that firstprivate make a copy of the output for the new task
                    {
                    permute(cdec(i, input), output, step+1, left-1);        // and recurse, deleting one ball of the chosen color from the input
                    }
                }
            }
        }
    }




int main(int argc, char **argv)
    {
    int n;
    unsigned input;
    unsigned char output[MAX];

    if(argc>1)colors=atoi(argv[1]);                     // first arg: the number of color
    else colors=2;

    if(argc>2)n=atoi(argv[2]);                          // second arg: how many balls there are of each color
    else n=2;

    if(argc>3)plevel = atoi(argv[3]);                   // third arg: the number of levels to spawn new threads

    if(argc>4)verbose=true;                             // fourth arg, be verbose if any arg given

    if(colors > COLORS || n*colors > MAX)               // check validity of inputs
        {
        printf("too many\n");
        return 0;
        }

    input = 0;                                          // init the input
    for(int i=0; i<colors; i++)
        {
        input |= n<<(i*8);
        }

    for(auto &x : output) x = 0;                        // init the output
    for(auto &x : ids) x = 0;                           // init the ID array (used for curiosity only)

    #pragma omp parallel num_threads(THREADS)           // kick off the permuter
    #pragma omp single
    permute(input, output, 0, colors*n);

    for(auto id: ids)maxthread = id>maxthread?id:maxthread; // figure out how many threads were used

    printf("permutations = %d\n", permutations);        // print results
    printf("maxthread = %d\n", maxthread);

    return 0;
    }

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>
#include<string.h>
#include<geneticlib.h>

//#define DEBUG
//#define PRINTCVS
#define PRINTCHESSBOARD

#define QUEEN 8         // Num of Queens
#define POPULATION 100  // Initial population
#define MUTATIONRATE 30 // Mutation rate 30%    
#define _SIZEOF_RESULT_  struct struct_dna

#define Malloc(pointer,cast,size,qnt) if((pointer=(cast)malloc(sizeof(size)*qnt))==NULL) {printf("Memory error!");exit(1);}
#define Malloc2(pointer,cast,size) if((pointer=(cast)malloc(sizeof(size)))==NULL) {printf("Memory error!");exit(1);}

#define CHESS(text,v) printf(text); for(j=0;j<QUEEN;j++)printf(" %d",v->chess[j]);

typedef struct struct_dna {
    unsigned int chess[QUEEN]; // Placed queens encoding    
    float fitness; //  fitness function ->  1/ (num of attacks + 1)
} *dna;

dna population[POPULATION];       // Array of initial population
dna evolvedPop[POPULATION];       // Array of evolved population
dna* mating_pool;                 // Mating pool array in heap memory

unsigned int dim_mating_pool = 0; // Mating pool dimention
unsigned long generation   = 0;   // Num of interation to find solution
bool found = false;               // One solution found

float fitness_fun(dna one) {
    unsigned int attack = 0,next;
    for(int i=0;i<QUEEN;i++){
        for(next=i+1;next<QUEEN;next++){
            if(one->chess[i] == one->chess[next] || abs(i-next)==abs(one->chess[i]-one->chess[next])) attack++;
        }
    }
    return ((float)1.0)/(attack+1);
}
void NewPopulation() {
    unsigned int j,i,seed = (unsigned)time(NULL);

    for (i=0;i<POPULATION;i++) {
        Malloc2(population[i],dna,_SIZEOF_RESULT_)
        for(j=0;j<QUEEN;j++)population[i]->chess[j] = rand_r(&seed)%QUEEN;

        // foreach new chessboard perform fitness function
        population[i]->fitness = fitness_fun(population[i]);
    }
}
void MatingPool() {
    // Create new Mating pool
    unsigned short times,i,j,count=0;
    dim_mating_pool=0; 

    for(i=0;i<POPULATION;i++) dim_mating_pool += (int)((population[i]->fitness)*100);
    Malloc(mating_pool,dna*,dna,dim_mating_pool)

    for(i=0;i<POPULATION;i++) {
        times = (int)((population[i]->fitness)*100);
        for(j=0;j<times;j++) mating_pool[count++] = population[i];
    }
}
dna crossover(dna ParentA,dna ParentB,unsigned int seed) {
    unsigned int i;
    dna child;
    Malloc(child,dna,_SIZEOF_RESULT_,1)

    for(i=0;i<seed;i++)   child->chess[i] = ParentA->chess[i];
    for(i=seed;i<QUEEN;i++) child->chess[i] = ParentB->chess[i];
    child->fitness = fitness_fun(child);
    return child;
}
bool chess_cmp(int a[QUEEN],int b[QUEEN]){
    // Compare two different chessboard
    short i=0;
    while (i<QUEEN) if(a[i]!=b[i++]) return false;
    return true;
}
void Evolve() {
    dna ParentA,ParentB,child;

    int limit,gen,j = (unsigned)time(NULL)*generation++;
    srand(time(NULL));

    // Foreach element in old population , I create a evolved chessboard
    for(int i=0;i<POPULATION;i++) {  

        // Select parent A through Mating pool
        ParentA = mating_pool[rand_r(&gen)%dim_mating_pool];
        #ifdef DEBUG 
            CHESS("Parent A-> (" ,ParentA)
        #endif

        limit=0;
        do {
            // Select parent B through Mating pool, but B must be different from B
            // to avoid loops, I allow to select same chessborad, if limit>100
            ParentB = mating_pool[rand_r(&gen)%dim_mating_pool];
            limit++;
        } while (chess_cmp(ParentA->chess,ParentB->chess) && limit<=100);

        #ifdef DEBUG 
            CHESS(") Parent B-> (" ,ParentB)
        #endif

        // Evolved chessboard
        child = crossover(ParentA,ParentB,rand_r(&gen)%QUEEN);
        #ifdef DEBUG 
            CHESS(") Crossover->(",child)
        #endif

        // Mutation event
        if(rand_r(&gen)%100<=MUTATIONRATE) {
            child->chess[rand_r(&gen)%QUEEN]=rand_r(&gen)%QUEEN;
            #ifdef DEBUG 
                CHESS(") Mutated-> (",child)
            #endif
        } 
        #ifdef DEBUG 
            printf(")\n");
        #endif

        evolvedPop[i] = child;

    }
    // I exchange the old population into evolved population
    for(int i=0;i<POPULATION;i++) {free(population[i]);  population[i] = evolvedPop[i];}
    free(mating_pool);
}
void print_population(dna* p) {
    int i,j;
    for(i=0;i<POPULATION;i++) {
        for(j=0;j<QUEEN;j++) printf(" %2d ",p[i]->chess[j]);
        printf("|%2f\n",p[i]->fitness);  
    }
}
void checksolution() {
    float bestfitness = 0;
    dna bestchess;

    for(int i=0;i<POPULATION;i++) {
        if(population[i]->fitness>bestfitness) {
            bestfitness = population[i]->fitness;
            bestchess = population[i];
        }
    }
    #ifdef DEBUG
        printf("%f2\n",bestfitness);
    #endif
    
    if((int)bestfitness==1) {

        #ifdef PRINTCHESSBOARD
            printf("Solution found!\n");
            printf("Num of generation = %ld\n",generation);

            printf("(");
            for(int i=0;i<QUEEN;i++) printf("%d ",bestchess->chess[i]);
            printf(")\n");  
        #endif

        #ifdef PRINTCVS
            unsigned short temp_chess[QUEEN][QUEEN],j;
            for(int i=0;i<QUEEN;i++) {
                for(j=0;j<QUEEN;j++) {
                    if(j==bestchess->chess[i]) temp_chess[i][bestchess->chess[i]]=1; 
                    else temp_chess[i][j]=0;
                    printf("%d;",temp_chess[i][j]);
                }
                printf("\n");
            }
        #endif
       found = true;
    }
}
void Genetic_algorithm() {

    NewPopulation();

    while(!found) {
        MatingPool();
        Evolve();
        #ifdef DEBUG
            print_population(population);
        #endif 
        checksolution();
    }
    for(int i=0;i<POPULATION;i++) free(population[i]);  
}

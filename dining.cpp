/*
    WUSTL - FL2014 - CSE 422S - Lab 2
    Part 1 - Dining Philosopher

    Name: SeungJu Lee and Elliot Yun
    yune@wustl.edu
    seungjulee@wustl.edu

    Lab 2
 */
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>
#include <iostream>
#include <sstream>

#define sec 1000000/3
#define THINKING 0
#define HUNGRY 1
#define EATING 2
#define LEFT (ph_num+N-1)%N
#define RIGHT (ph_num+1)%N

using namespace std;
  
int N;  

sem_t mutexx;
vector<sem_t> S;
  
void * philospher(void *num);
void take_fork(int);
void put_fork(int);
void test(int);

vector<int> state;
vector<int> phil_num;
vector<int> eatCount;

enum errs{Success, ArgError};  
  
int main(int argc, char* argv[])
{
    if (argc != 2){
        printf("Syntax Error\n");
        printf("Correct Usage : [Program Name] [Argument 1]\n");
        printf("Argument 1 must be a integer value less than 14\n");
        return ArgError;
    }
    int myInt;
    istringstream iss(argv[1]);
    iss >> myInt;
    N = myInt;
    
    if (N > 15){
        printf("# of Philosophers must be less than 15\n");
        return ArgError;
    }

    state.resize(N);
    S.resize(N);
    phil_num.resize(N);
    for (int a = 0; a< N; a++){
        phil_num[a] = a;
    }
    eatCount.resize(N);

    int i,j;
    pthread_t thread_id[N];
    sem_init(&mutexx,0,1);
    if (N > 10){
        for (int i = 0; i < 10; i++){
            printf(" ");
        }
        for (int i = 10; i < N; i++){
            printf("1");
        }
        printf("\n");
        for (int i = 0; i < N; i++){
            printf("%d", i % 10);
        }
    }
    else {
        for (j=0; j<N; j++){
            printf("%d", j);
        }
    }
    printf("\n");
    for(i=0;i<N;i++)
        sem_init(&S[i],0,1);
    for(i=0;i<N;i++)
    {
        pthread_create(&thread_id[i],NULL,philospher,&phil_num[i]);
    }
    for(i=0;i<N;i++)
        pthread_join(thread_id[i],NULL);
    return Success;
}
  
void *philospher(void *num)
{
    int counter = 0;
    while(counter < N)
    {
        int *i =(int*)num;
        usleep(sec);
        take_fork(*i);
        usleep(0);
        put_fork(*i);
        counter = 0;
        for (int d = 0; d < N; d++){
            if (eatCount[d] >= 5){
                counter++;
            }
        }
        if (counter >= N){
            break;
        }
    }
}
  
void take_fork(int ph_num)
{
    sem_wait(&mutexx);
    state[ph_num] = HUNGRY;
    test(ph_num);
    sem_post(&mutexx);
    sem_wait(&S[ph_num]);
    usleep(sec);
}
  
void test(int ph_num)
{
    if (state[ph_num] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
    {
        state[ph_num] = EATING;
        usleep(2 * sec);
        eatCount[ph_num]++;

        int p;
        for (p = 0; p < N; p++){
            if (state[p] == EATING){
                printf("*");
            }
            else{printf(" ");}
        }
        printf("\n");
        sem_post(&S[ph_num]);
    }
}
  
void put_fork(int ph_num)
{
    sem_wait(&mutexx);
    state[ph_num] = THINKING;
    test(LEFT);
    test(RIGHT);
    sem_post(&mutexx);
}
/*
    WUSTL - FL2014 - CSE 422S - Lab 2
    Part 2 - Drinking Philosopher

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
#include <fstream>
#include <string>
#include <tuple>
#include <algorithm>

#define sec 1000000
#define THINKING 0
#define THIRSTY 1
#define DRINKING 2
#define LEFT (ph_num+N-1)%N
#define RIGHT (ph_num+1)%N

using namespace std;
  
int N;
int rowNum;
sem_t mutexx;
vector< vector<sem_t> > S;
vector< vector<bool> > semStat;
 
void * philospher(void *num);
void take_bottle(int);
void put_bottle(int);
void speak(int);
vector<int> state;
vector<int> phil_num;
vector<int> drinkCount;
vector<vector <int> > rowVec;
vector<vector<tuple <int, int> > > whichBottle;
vector<bool> pickedUp;

int errorCheck;

enum errs{Success, ArgError, FileError, DataErr};  
  
int main(int argc, char* argv[])
{
    if (argc != 2){
        printf("Syntax Error\n");
        printf("Correct Usage : [Program Name] [Argument 1]\n");
        printf("Argument 1 must be a file with n by n matrix entries\n");
        return ArgError;
    }
    string fileName;
    istringstream iss(argv[1]);
    // open file to initialize with input
    if (iss >> fileName){
        ifstream myFile(fileName);
        // grabbing input data
        if (myFile.is_open()){
            int whichLine = 0;
            string firstLine;
            getline(myFile, firstLine);
            int vals;
            int oCount = 0;
            istringstream fs(firstLine);
            vector<int> temp;
            while(fs >> vals){
                temp.push_back(vals);
                oCount++;
            }
            rowVec.push_back(temp);
            whichLine++;
            while (myFile.good()){
                string lineString;
                getline(myFile, lineString);
                int objCount = 0;
                int val;
                istringstream linestream(lineString);
                vector<int> temp2;
                while (linestream >> val){
                    temp2.push_back(val);
                    objCount++;
                }
                rowVec.push_back(temp2);
                whichLine++;
            }
            if (whichLine != oCount){
                printf("Erroneous Data Input.");
                return DataErr;
            }
            N = whichLine;
        }
        else{
            printf("File Does Not Exist.");
            return FileError;
        }
    }
    else { printf("Check File Name.\n");return ArgError;}
    if (N > 15){
        printf("# of Philosophers must be less than 15\n");
        return ArgError;
    }
    S.resize(N);
    semStat.resize(N);
    whichBottle.resize(N);
    pickedUp.resize(N);

    for (int i = 0; i < S.size(); i++){
        S[i].resize(rowVec[i].size());
        semStat[i].resize(rowVec[i].size());
    }
    int bottleCount;
    // initializing bottles
    for (int i = 0; i < rowVec.size(); i++){
        for (int j = i; j < rowVec[i].size(); j++){
            // if bottle exists, initialize a handle and mark availability
            if (rowVec[i][j] == 1){
                bottleCount++;
                sem_init(&S[i][j], 0, 1);
                semStat[i][j] = true;
            }
            else {
                semStat[i][j] = false;
            }
        }
    }
    
    state.resize(N);
    phil_num.resize(N);
    for (int a = 0; a< N; a++){
        phil_num[a] = a;
    }
    drinkCount.resize(N);
    int i,j;
    pthread_t thread_id[N];
    
    // initialize mutex
    sem_init(&mutexx,0,1);
    
    for(i=0;i<N;i++)
    {
        // create threads for each process. Handling concurrency with mutexes
        pthread_create(&thread_id[i],NULL,philospher,&phil_num[i]);
    }
    for(i=0;i<N;i++)
        // merging threads
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
        take_bottle(*i);
        speak(*i);
        // timeout
        usleep(0);
        put_bottle(*i);
        // don't change
        counter = 0;
        for (int d = 0; d < N; d++){
            if (drinkCount[d] >= 5){
                counter++;
            }
        }
        if (counter >= N){
            break;
        }
    }
}
/* Method for printing current overall status */
void speak(int who)
{
    // wait for handle to become available
    sem_wait(&mutexx);
    stringstream printS;
    printS << "Philosopher " << who << " is drinking from Bottles: ";
    string printString = printS.str();
    // interate over bottle list to see who's drinking from each bottle
    for (tuple<int,int> t : whichBottle[who]){
        int x = get<0>(t);
        int y = get<1>(t);
        stringstream temp;
        temp << "(" << x << " , " << y << ")";
        printString += temp.str() + " ";
        temp.flush();
    }
    cout << printString << endl<<endl;
    printS.flush();
    usleep(sec);
    // release handle
    sem_post(&mutexx);
}

/* Method to handle asking for bottles */
void take_bottle(int sem)
{
    // wait for handle to become available
    sem_wait(&mutexx);
    // updating status
    state[sem] = THIRSTY;
    pickedUp[sem] = false;
    int semCount = 0;
    for (int i = 0; i < rowVec[sem].size(); i++){
        int first;
        int second;
        if (i < sem){
            first = i;
            second = sem;
        }
        else {
            first = sem;
            second = i;
        }
        // if bottle is available
        if (semStat[first][second]){
            semCount++;
        }
    }
    
    int semPassed = 0;
    for (int i =0; i < rowVec[sem].size(); i++){
        int first;
        int second;
        if (i < sem){
            first = i;
            second = sem;
        }
        else {
            first = sem;
            second = i;
        }
        // if bottle is available
        if (semStat[first][second]){
            if (sem_trywait(&S[first][second]) == 0){
                if (semPassed == (semCount - 1)){
                    tuple<int,int> bottleIs = make_tuple(first, second);
                    // add to bottles currently picked up
                    whichBottle[sem].push_back(bottleIs);
                    // update status
                    pickedUp[sem] = true;
                    drinkCount[sem]++;
                }
                int randNum = rand() % 10;
                if (randNum <= 5){
                    // update semaphore
                    sem_post(&S[first][second]);
                    semPassed++;
                }
                else{
                    tuple<int,int> bottleIs = make_tuple(first, second);
                    whichBottle[sem].push_back(bottleIs);
                    // increment drink count
                    drinkCount[sem]++;
                }
            }
        }
    }
    // release handle
    sem_post(&mutexx);
    // timeout
    usleep(sec);
}
  
void put_bottle(int ph_num)
{
    // wait for handle
    sem_wait(&mutexx);
    // update status
    state[ph_num] = THINKING;
    stringstream printS;
    printS << "Philosopher " << ph_num << " is putting down Bottles: ";
    string printString = printS.str();
    for (tuple<int,int> t : whichBottle[ph_num]){
        int x = get<0>(t);
        int y = get<1>(t);
        // release semaphore for bottles
        sem_post(&S[x][y]);
        stringstream temp;
        temp << "(" << x << " , " << y << ")";
        printString += temp.str() + " ";
        temp.flush();
    }
    whichBottle[ph_num].clear();
    cout << printString << endl<<endl;
    printS.flush();
    // release handle
    sem_post(&mutexx);
}

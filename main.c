#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include <fcntl.h>
#include <unistd.h>

#define N1 2
#define N2 4
#define N3 8
#define N4 16

int size = 0;
int trueSize;

int* loadArray(int* arr, char** argv, int N){

    FILE* f = fopen(argv[1], "r");
    int num;
    arr = (int*)malloc(sizeof(int) * 1);
   
    while(fscanf(f, "%d", &num) == 1){
        arr[size] = num;
        size++;
        arr = realloc(arr, sizeof(int) * (size + 1));
    }
    
    trueSize = size + 1;
    int n = (int)ceil(log2(size));
    int pow = 1<<n;
    arr = realloc(arr, sizeof(int) * pow);
    
    for(; size < pow; size++){
        arr[size] = 0;
    }
    return arr;
}

//Utility function to print an array
printArray(int* arr){
    
    for(int i = 0; i < trueSize; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
}


//Utility function to find the prefix sum of an array of numbers
int* prefix(int* arr, int N){

    int last = arr[size - 1];
    
    omp_set_num_threads(N);

    //int p = 0;
    int p = 1;
    int p_ = (int)log2(size);
    
    for(int d = 0; d <= p_ - 1; d++){
        p *= 2;
        #pragma omp parallel shared(arr, p)
        {
            #pragma omp for
                for(int i = 0; i <= (size - 1); i += p){
                    arr[i + p -1] = arr[i + p/2 - 1] + arr[i + p -1];
                }
        }

    }

    arr[size - 1] = 0;
    int temp;

    for(int d = p_ - 1; d >=0; d--){
        p /= 2;
        #pragma omp parallel shared(arr, p, temp)
        {
            #pragma omp for
                for(int i = 0; i <= (size - 1); i+=2 * p){
                    temp = arr[i + p -1];
                    arr[i + p - 1] = arr[i + 2 * p -1];
                    arr[i + 2 * p -1] = temp + arr[i + 2 * p -1];
                }
        }
    }

    int* prefSum = (int*)malloc(sizeof(int) * size);
    
    #pragma omp parallel shared(prefsum)
    {
        #pragma omp for
            for(int i = 0; i < size - 1; i++){
                prefSum[i] = arr[i + 1];
            }   
    }
    prefSum[size - 1] = prefSum[size - 2] + last;

    return prefSum;
}


//Function that simulates data compaction
int* compact(int* A, int N){
    
    int* A_ = (int*)calloc(sizeof(int), size);
    int* Aux = (int*)calloc(sizeof(int), size);
    int* B = (int*)calloc(sizeof(int), size);


    omp_set_num_threads(N);

    #pragma omp parallel shared(A, A_)
    {
        #pragma omp for
        for(int i = 0; i < size; i++){
            if(A[i] != 0){
                A_[i] = 1;    //Adding 1 wherever data is present
            }
        }
    }

    //Prefix sum
    Aux = prefix(A_, N);

    #pragma omp parallel shared(A, B, Aux)
    {
        #pragma omp for
            for(int i = 0; i < size; i++){
                if(A[i] != 0){
                    B[Aux[i] - 1] = A[i];
                }
            }
    }
    
    return B;
}




int main(int argc, char* argv[]){

    if(argc == 1){
        
        printf("Please supply the file whose data needs to be compacted\n");
    }else if(argc > 2){
        
        printf("Can compact data of only one file at a time\n");
    }else{
        int* arr;
        int N;
        printf("Enter the number of threads you want : ");
        scanf("%d", &N);

        arr = loadArray(arr, argv, N);
        int* b = compact(arr, N);
        
        FILE* file = fopen("Compacted_Array.txt", "w");
        fclose(file);

        int f = open("Compacted_Array.txt", O_WRONLY | O_CREAT, 0777);
        dup2(f, STDOUT_FILENO);
        close(f);
        printArray(b);
    }
}


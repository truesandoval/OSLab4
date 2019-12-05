#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 4

int matA[MAX][MAX]; 
int matB[MAX][MAX]; 

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX]; 
int matProductResult[MAX][MAX]; 

struct dim {
  size_t i;
  size_t j;
};

void fillMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            matrix[i][j] = (rand() % 10 + 1);
        }
    }
}

void printMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the sum of the
// values at the coordinates of matA and matB.
void* computeSum(void* args) {
  int coord = *(int*)args;
  for (int i = coord * MAX / 4; i < (coord + 1) * MAX / 4; i++){
    for (int j = 0; j < MAX; j++){
      matSumResult[i][j] = matA[i][j] + matB[i][j];
    }
  }
    return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the difference of the
// values at the coordinates of matA and matB.
void* computeDiff(void* args) {
  int coord = *(int*)args;
  for (int i = coord * MAX / 4; i < (coord + 1) * MAX / 4; i++){
    for (int j = 0; j < MAX; j++){
      matDiffResult[i][j] = matA[i][j] - matB[i][j];
    }
  }
    return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the inner product
// of matA and matB.
void* computeProduct(void* args) {
  struct dim *data = (struct dim *)args;
  size_t x,y;
  for (x = 0; x < MAX; x++){
    size_t i = (data[x]).i;
    size_t j = (data[x]).j;
    double sum = 0;
    for (y = 0; y < MAX; y++){
      sum = sum + matA[i][y]*matB[y][j];
    }
    matProductResult[i][j] = sum;
    sum = 0;
  }
    return NULL;
}

// Spawn a thread to fill each cell in each result matrix.
// How many threads will you spawn?
int main() {
    srand(time(0));  // Do Not Remove. Just ignore and continue below.
    
    // 1. Fill the matrices (matA and matB) with random values.
    
    fillMatrix(matA);
    fillMatrix(matB);
    
    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);
    
    // 3. Create pthread_t objects for our threads.
    
    pthread_t threads[MAX];
    
    // 4. Create a thread for each cell of each matrix operation.
    // 
    // You'll need to pass in the coordinates of the cell you want the thread
    // to compute. One way to do this is to "encode" the coordinates into a number
    // and then pass it into the thread function. For example,
    // 
    // long coordinates = MAX*row + col;
    // 
    // Then the thread will have to "decode" that number back into the row and col.
    // 
    // Another option is to malloc memory for the coordinates, populate the coordinates
    // into that space, and pass that address to the thread. The thread will then have
    // to free that space when it's done with what's in that memory.
    
    size_t i, k;
    struct dim **values;
    values = ((struct dim **)malloc(MAX * sizeof (struct dim)));
    for (i = 0; i < MAX; i++){
      values[i] = (struct dim *)malloc(MAX * sizeof (struct dim));
      for (k = 0; k < MAX; k++){
        values[i][k].i = i;
        values[i][k].j = k;
      }
      pthread_create(&threads[i], NULL, computeProduct, values[i]);
      pthread_create(&threads[i], NULL, computeSum, values[i]);
      pthread_create(&threads[i + MAX], NULL, computeDiff, values[i]);
    }
  free(values);
    // 5. Wait for all threads to finish.
    
  for (i = 0; i < MAX; i++){
    pthread_join(threads[i], NULL);
  }
    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);
    return 0;
}
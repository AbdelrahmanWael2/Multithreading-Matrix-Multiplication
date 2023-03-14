#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

double** matA;
double** matB;
double** matC;
int aRow, bRow, aCol, bCol;

//dimension of matrices to be read
struct dim{
    int row;
    int col;
};
struct dim d;


double** readFile(char *file) {
    char reader[100];double** matrix;
    //strcat(file, ".txt");
    FILE *fileptr = fopen(file, "r");

    if (fileptr == NULL) {
        printf("Error: FILE NOT EXIST");
        return 0;
    }
    fgets(reader, 500, fileptr);
    sscanf(reader, "row=%d col=%d", &d.row, &d.col);
    matrix = (double**) malloc(d.row*sizeof(double*));
    for(int i = 0; i < d.row; i++){
        matrix[i] = (double*) malloc(d.col*sizeof(double*));
    }
    for(int i = 0; i < d.row; i++){
        for(int j = 0;j < d.col;j++){
            fscanf(fileptr, "%lf", &matrix[i][j]);
        }
    }
    fclose(fileptr);
    return matrix;
}

//No threads used
void mulByMatrix(){
    for(int i = 0; i < aRow; i++){
        for(int j = 0; j < bCol; j++){
            matC[i][j] = 0;
            for(int k = 0; k < bRow; k++){
                matC[i][j] = matC[i][j] + matA[i][k] * matB[k][j];
            }
        }
    }
}

void noThreads() {
    struct timeval stop, start;
    gettimeofday(&start, NULL);
    mulByMatrix();
    gettimeofday(&stop, NULL);
    printf("Method 1:\n Seconds Taken %lu \n", stop.tv_sec - start.tv_sec);
    printf("MicroSeconds Taken %lu\n", stop.tv_usec - start.tv_usec);
    printf("Number of Threads = 1\n");
}
void *mulByRow(void *arg){
    int i = *(int*)arg;
    double res;
    for(int j = 0; j < bCol; j++){
        for(int k = 0; k < bRow; k++){
            res = res + (matA[i][k]*matB[k][j]);
        }  matC[i][j] = res;
            res = 0;

    }
    pthread_exit(NULL);
}

void rowThreads(){
    struct timeval start,stop;
    gettimeofday(&start, NULL);
    pthread_t thread[aRow];
    int tid[aRow];
    for(int i = 0; i < aRow; i++) {
        tid[i] = i;
        if (pthread_create(&thread[i], NULL, mulByRow, &tid[i])) {
            printf("Error in thread creation");
            free(matC);
            free(matA);
            free(matB);
            exit(1);
        }
    }
        for(int i = 0; i < aRow; i++){
            pthread_join(thread[i],NULL);
        }
        gettimeofday(&stop,NULL);
        printf("Method 2: \nSeconds Taken %lu \n", stop.tv_sec - start.tv_sec);
        printf("MicroSeconds Taken %lu\n", stop.tv_usec - start.tv_usec);
        printf("Number of Threads = %d\n", aRow);

}

void *mulByElement(void *arg){
    struct dim *D = arg;
    double res = 0;
    for(int i = 0; i < bRow; i++){
        res = res + (matA[D->row][i]*matB[i][D->col]);
    }
    matC[D->row][D->col] = res;
    pthread_exit(NULL);
}

void elementThreads(){
    pthread_t threads2[aRow][bCol];
    struct dim *D;
    struct timeval start,stop;
    gettimeofday(&start,NULL);
    for(int i = 0; i < aRow; i++){
        for(int j = 0; j < bCol; j++){
            D = malloc(sizeof(struct dim));
            D->row = i;
            D->col = j;
            if(pthread_create(&threads2[i][j],NULL, mulByElement,D)){
                printf("Error in creating threads");
                free(matC);
                free(matB);
                free(matA);
                exit(1);
            }
        }
    }
    for(int i = 0 ; i < aRow; i++){
        for(int j = 0; j < bCol; j++){
            pthread_join(threads2[i][j],NULL);
        }
    }
    gettimeofday(&stop,NULL);
    printf("Method 3: \n Seconds Taken %lu \n", stop.tv_sec - start.tv_sec);
    printf("MicroSeconds Taken %lu\n", stop.tv_usec - start.tv_usec);
    printf("Number of Threads = %d \n", aRow*bCol);
}

void getResult(char fileName[100], int method){
    FILE *fileptr = fopen(fileName,"w");
    fprintf(fileptr, "Method : %d \n", method);
    for(int i = 0; i < aRow; i++){
        for(int j = 0; j < bCol; j++){
            fprintf(fileptr, "%.1f ", matC[i][j]);
        }
        fprintf(fileptr,"\n");
    }
    fclose(fileptr);
}

int main(int argc, char *argv[]) {

    char out1[100], out2[100], out3[100];
    char a[10] = "", b[10] = "", c[10] = "";
    strcat(a, argv[1]);
    strcat(a, ".txt");

    strcat(b, argv[2]);
    strcat(b, ".txt");

    strcat(c, argv[3]);

    //nonempty arguments
    if(argc == 4){
        matA = readFile(a);
        aRow = d.row;
        aCol = d.col;
        matB = readFile(b);
        bRow = d.row;
        bCol = d.col;
        strcpy(out1,c);
        strcat(out1, "_per_matrix");
        strcpy(out2,c);
        strcat(out2,"_per_row");
        strcpy(out3,c);
        strcat(out3,"_per_element");


    }else{  //empty args
        matA= readFile("a.txt");
        aRow = d.row;
        aCol = d.col;
        matB = readFile("b.txt");
        bRow = d.row;
        bCol = d.col;
        strcpy(out1, "c_per_matrix.txt");
        strcpy(out2, "c_per_row.txt");
        strcpy(out3, "c_per_element.txt");
    }


    //check dimensions
    if(aCol != bRow){
        printf("Error in matrices dimensions !\n");
        free(matA);
        free(matB);
    }
    //matrix C allocation
    matC = (double**) malloc (aRow*sizeof(double*));
    for(int i = 0; i < aRow; i ++){
        matC[i] = (double*) malloc(bCol*sizeof(double));
    }

    noThreads();
    getResult(out1, 1);

    rowThreads();
    getResult(out2, 2);

    elementThreads();
    getResult(out3, 3);

    free(matA);
    free(matB);
    free(matC);

    return 0;
}

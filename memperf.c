#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include<time.h>
#include <sys/time.h>

#include "mymalloc.h"

int main(){

	/*----------------------------first Fit---------------------------*/
      time_t seed;	
	srand((unsigned) time(&seed));
	int firstFitCount = 0;
	struct timeval tv1, tv2, tv3, tv4, tv5, tv6;
	double utilNum;
	myinit(0);
	int amount = 100000;		
	void* pointers[20];
	if(!pointers[0]){
		printf("NULL");
	}
	for(int i =0; i<20; i++){
		pointers[i] = NULL;
	}
	gettimeofday(&tv1, NULL);
	for( int n = 0 ; n < amount; n++ ) {
		int randGen = rand() % 3;
		int pointerIndex = rand()%20;
		int size = (rand()%256)+1;
		//int randGen = 0;
		if(randGen==0){
			if(pointers[pointerIndex]!=NULL){				
				myfree(pointers[pointerIndex]);
			}
			pointers[pointerIndex] = mymalloc(size);
		}
		if(randGen ==1){
			pointers[pointerIndex] = myrealloc(pointers[pointerIndex], size);
		}
		if(randGen > 1){
			myfree(pointers[pointerIndex]);
			
		}
		firstFitCount++;
	}
	gettimeofday(&tv2, NULL);
	utilNum = utilization();
	mycleanup();

	/*double totalTime = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec);
	printf("First fit utilization: %f\n", utilNum);
	printf("count is %d\n", firstFitount);
	printf ("Total time = %f seconds\n", totalTime);
	printf ("Next Fit throughput = %f ops/sec\n", firstFitCount/totalTime);*/
	//printf("____________________________________________________________________________\n");
	/*----------------------------Next Fit---------------------------*/
	time_t seed2;	
	srand((unsigned) time(&seed2));
	int nextFitCount = 0;
	double nfUtil;
	myinit(1);
	int amount2 = 10000;		
	void* pointers2[20];
	if(!pointers2[0]){
		printf("NULL");
	}
	for(int i =0; i<20; i++){
		pointers2[i] = NULL;
	}
	gettimeofday(&tv3, NULL);
	for( int n = 0 ; n < amount2; n++ ) {
		int randGen = rand() % 3;
		int pointerIndex = rand()%20;
		int size = (rand()%256)+1;
		//int randGen = 0;
		if(randGen==0){
			if(pointers2[pointerIndex]==NULL){				
				myfree(pointers2[pointerIndex]);
				//pointers2[pointerIndex] = mymalloc(size);
			}
			pointers2[pointerIndex] = mymalloc(size);
		}
		if(randGen ==1){
			pointers2[pointerIndex] = myrealloc(pointers2[pointerIndex], size);
		}
		if(randGen > 1){
			myfree(pointers2[pointerIndex]);
			
		}
		nextFitCount++;
	}

	gettimeofday(&tv4, NULL);
	nfUtil = utilization();
	mycleanup();

	/*----------------------------Best Fit---------------------------*/
	time_t seed3;	
	srand((unsigned) time(&seed3));
	int bestFitCount = 0;
	double bfUtil;
	myinit(2);
	int amount3 = 100000;		
	void* pointers3[20];
	if(!pointers3[0]){
		printf("NULL");
	}
	for(int i =0; i<20; i++){
		pointers3[i] = NULL;
	}
	gettimeofday(&tv5, NULL);
	for( int n = 0 ; n < amount3; n++ ) {
		int randGen = rand() % 3;
		int pointerIndex = rand()%20;
		int size = (rand()%256)+1;
		//int randGen = 0;
		if(randGen==0){
			if(pointers3[pointerIndex]!=NULL){				
				myfree(pointers3[pointerIndex]);
			}
			pointers3[pointerIndex] = mymalloc(size);
		}
		if(randGen ==1){
			pointers3[pointerIndex] = myrealloc(pointers3[pointerIndex], size);
		}
		if(randGen > 1){
			myfree(pointers3[pointerIndex]);
			
		}
		bestFitCount++;
	}

	gettimeofday(&tv6, NULL);
	bfUtil = utilization();
	mycleanup();

	/*----------------------------------------*/

	double totalffTime = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 + (double) (tv2.tv_sec - tv1.tv_sec);
	double totalnfTime = (double) (tv4.tv_usec - tv3.tv_usec) / 1000000 + (double) (tv4.tv_sec - tv3.tv_sec);
	double totalbfTime = (double) (tv6.tv_usec - tv5.tv_usec) / 1000000 + (double) (tv6.tv_sec - tv5.tv_sec);

	printf("First fit throughput: %f ops/sec\n", firstFitCount/totalffTime);
	printf("First fit utilization: %f\n", utilNum);
	//printf("count is %d\n", firstFitount);

	printf("Next fit throughput: %f ops/sec\n", nextFitCount/totalnfTime);
	printf("Next fit utilization: %f\n", nfUtil);

	printf("Best fit throughput: %f ops/sec\n", bestFitCount/totalbfTime);
	printf("Best fit utilization: %f\n", bfUtil);

    return 0;
}
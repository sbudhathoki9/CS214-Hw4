#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include "mymalloc.h"

//.............................
#define heapSize 1048576

typedef struct Block Block;

struct Block{
      size_t sz;
      int isFree;
      size_t payload;
      void* ptr;
      Block *next;
};

typedef struct FBlock FBlock;
struct FBlock{
      size_t sz;
      FBlock* prev;
      FBlock* next;
};

//.............................
int flag = -1;
size_t hSize = 0; 
char* heapBlock;
char* mBlock;

struct Block *mallocBlock;
struct FBlock *globalFBlock;

//.............................*/


volatile FBlock* lastAllocatedBlock;

//.......................
size_t padding(size_t input);
void merge(FBlock* ptr);

//........................

void myinit(int allocAlg){
 
                                  
      flag = allocAlg;
      heapBlock = malloc(heapSize);
      globalFBlock = (void*) heapBlock;
      globalFBlock ->sz = heapSize;
      globalFBlock ->prev = NULL;
      globalFBlock ->next = NULL;

      
      mBlock= malloc(sizeof(Block) * (2 * (heapSize+1)));
      mallocBlock =(void*) mBlock;
      mallocBlock->sz = 0;
      mallocBlock->payload = 0;
      mallocBlock->isFree = 0;
      mallocBlock->ptr = NULL;
      mallocBlock->next = NULL;

      lastAllocatedBlock = NULL;                                                                                

      
}



FBlock* splitFB(FBlock* ptr, size_t size){

      FBlock *newBlock=(FBlock*)((void*)ptr+size);

	size_t diff = (ptr->sz) - size;
	newBlock->sz=diff;

	newBlock->next = ptr->next;
	newBlock->prev = ptr->prev;
      

      lastAllocatedBlock = newBlock;
      return newBlock; 
}

FBlock* splitFF(FBlock* ptr, size_t size){
      FBlock *newBlock=(FBlock*)((void*)ptr+size);

	size_t diff = (ptr->sz) - size;
	newBlock->sz=diff;
                                                                  //Attaching the newBlock to the list, Pending to attach list to the newBlock 
	newBlock->next = ptr->next;
	newBlock->prev = ptr->prev;
                                                                  //Attaching list to the newBLock and dettach ptr

      if(ptr->next ==NULL && ptr->prev ==NULL){
            globalFBlock = newBlock;
      }

      if(ptr->prev){
            ptr->prev->next = newBlock;

      }else{
         globalFBlock = newBlock;   
      }

      if(ptr->next){
            ptr->next->prev = newBlock;
      }

      return newBlock; 
}

void insertMallocBlock(void* curr, size_t size){
      size_t  padded = padding(size);
      if(mallocBlock->sz == 0){
            mallocBlock->sz = padded;
            mallocBlock->payload = size;
            mallocBlock ->isFree = 0;
            mallocBlock ->ptr = (void*) curr;                      
            mallocBlock -> next = NULL;
      
      }else {
            //Block* newBlock = NULL;
            Block* temp1 = mallocBlock;
            if(temp1 == NULL ){
                  
            }else{
                  while(temp1->next != NULL){
                        if(temp1->ptr == curr ){ 
                              //printf("in insert %p\n", temp1->ptr);               
                              temp1->sz = padded;
                              temp1->payload = size;
                              temp1->ptr = curr;
                              temp1 ->isFree = 0;
                              return;
                        }

                        temp1 = temp1->next;
            }

            if(temp1->ptr == curr ){ 
                  //printf("in insert %p\n", temp1->ptr);               
                  temp1->sz = padded;
                  temp1->payload = size;
                  temp1->ptr = curr;
                  temp1 ->isFree = 0;
                  return;
            }

                  Block* ptr = (Block*) ((void*)temp1+sizeof(Block));

                  temp1->next = ptr;
                  ptr->sz = padded;
                  ptr->payload = size;
                  ptr-> ptr = curr;
                  ptr ->isFree = 0;
                  ptr->next = NULL;                            
            } 
      } 
}
void*  firstFit(size_t size){
      FBlock* temp = globalFBlock;
      //(void*) newPtr = NULL;
      FBlock* freeBlock = NULL;
      size_t byteSize = padding(size);
      
      while(temp != NULL){
            //printf("In while loop\n");
            if(temp->sz == byteSize){
                  insertMallocBlock(temp, size);
                  if (temp->prev == NULL) {
                        lastAllocatedBlock = temp->next;
				globalFBlock = temp->next;
				if (globalFBlock)
					globalFBlock->prev = NULL;
			} else {
				freeBlock->next = temp->next;
				if (temp->next)
					temp->next->prev = freeBlock;
			}
			return (void*) (temp);
            }else if(temp->sz > byteSize && temp->sz - byteSize >= (sizeof(FBlock)+8)){
                  FBlock* remBlock =(FBlock*) splitFB(temp, byteSize);

                  insertMallocBlock(temp, size);

                  if(temp->next){
                        temp->next->prev = remBlock;       
                  }
                  if (temp->prev){
                        temp->prev->next = remBlock;
                  }else{
                        globalFBlock = remBlock;   
                  }
                        return (void*)(temp);
            }
            freeBlock = temp;
            temp = temp->next;
      }

      return NULL;    
}

void* bestFit(size_t size){               //looks through best available space
      FBlock* temp = globalFBlock;
      FBlock* freeBlock = NULL;
      FBlock* current = NULL;
      size_t byteSize = padding(size);
      int offset = heapSize;

      while(temp != NULL){
            if(temp->sz == byteSize){           //Perfect fit
                  insertMallocBlock(temp, size);
                  if (temp->prev == NULL) {
                        lastAllocatedBlock = temp->next;
				globalFBlock = temp->next;
				if (globalFBlock)
					globalFBlock->prev = NULL;
			} else {
				freeBlock->next = temp->next;
				if (temp->next)
					temp->next->prev = freeBlock;
			}
			return (void*) (temp);

            }else if(temp->sz >byteSize && temp->sz - byteSize >= (sizeof(FBlock)+8)){
                  if(temp->sz - byteSize < offset){
                        offset = temp->sz - byteSize;
                        current = temp;
                  }
            }
            freeBlock = temp;
            temp = temp->next; 
            
      }

      if(current != NULL){
            temp = current;
            FBlock* remBlock =(FBlock*) splitFB(temp, byteSize);
                  
                  insertMallocBlock(temp, size);
                  temp->sz = byteSize;
                  if(temp->next){
                        temp->next->prev = remBlock;       
                  }

                  if (temp->prev){
                        temp->prev->next = remBlock;
                  }else{
                        globalFBlock = remBlock;
                        
                  }

		return (void*) (temp);
	}
	return NULL;

}

void* nextFit(size_t size){

      FBlock* temp;
      
      if(lastAllocatedBlock == NULL){
        lastAllocatedBlock = globalFBlock;
        temp = globalFBlock;
        
      }else{  
        temp =(FBlock*)((void*)lastAllocatedBlock);
      
      }

      if(lastAllocatedBlock->sz < padding(size) && lastAllocatedBlock->next == NULL){
            lastAllocatedBlock = globalFBlock;
            temp = (FBlock*) ((void*)lastAllocatedBlock);
      }

      FBlock* prevFBlock = NULL;
      size_t byteSize = padding(size);
      if((void*)temp >= (void*)heapBlock+heapSize-32){
            lastAllocatedBlock = globalFBlock;
            temp = globalFBlock;

      }

      
      while(temp != NULL && (void*)temp <= (void*)heapBlock+heapSize-32){
            
            if(temp->sz == byteSize){     //When the size is exacty equal 
                  
                   insertMallocBlock(temp, size);
                  if (temp->prev == NULL && prevFBlock ==NULL) {   // we are at the top of the list and need to update global block after removing temp 
                        lastAllocatedBlock = temp->next;
				globalFBlock = temp->next;
				if (globalFBlock)
					globalFBlock->prev = NULL;
			} else {                                        // We are somewhere in middle and neet to remove temp from it
                        if(prevFBlock){
				      prevFBlock->next = temp->next;
                        }
				if (temp->next){
                              lastAllocatedBlock = temp->next;
					temp->next->prev = prevFBlock;
                        }else{
                             lastAllocatedBlock = globalFBlock; 
                        }
			}
			return (void*) (temp);      

            }else if(temp->sz > byteSize && temp->sz - byteSize >= (sizeof(FBlock)+8) )
            {
           lastAllocatedBlock= splitFF(temp, byteSize);
                  
                  insertMallocBlock(temp, size);
                  return (void*)(temp);
            }

            

            prevFBlock = temp;
            temp = temp->next;
      }

      return NULL;    


}

void* mymalloc(size_t size){

      if(size == 0){
            return NULL;
      }
      if(heapSize < (hSize+padding(size))){
            return NULL;
      }
      void *newPtr = NULL;
      switch (flag){           
            case 0:
                  newPtr= firstFit(size);
                  if(newPtr){
                         hSize+=padding(size);
                  }                  
                  break;              
            case 1:
                  newPtr = nextFit(size);
                  
                   if(newPtr){
                         hSize+=padding(size);
                  } 
                  break;
            case 2:
                  newPtr = bestFit(size);
                  
                   if(newPtr){
                         hSize+=padding(size);
                  } 
                  break;
      }
      //printf("Malloc successful\n");
      return newPtr;
}


void myfree(void* ptr){

      if(ptr == NULL){
            return;
      }

      if(((void*)heapBlock+heapSize+1) <= ptr ||  (void*) heapBlock > ptr ){

            printf("error: not a heap pointer\n");
            return;
      }

      if( (uintptr_t) (ptr) % 8 != 0){
            printf("error: not a malloced address\n");
            return;
      }

      Block* allocedBlock = mallocBlock;
      FBlock * prevFblock =NULL;
      FBlock * currFblock =(FBlock*) globalFBlock;
      FBlock* curr =NULL;
      Block* allcPtr = NULL;
      while(allocedBlock != NULL){
            if(allocedBlock->ptr == ptr){
                  if(allocedBlock->isFree == 1){
                        printf("error: double free\n");
                        return;
                  }
                  allocedBlock->isFree = 1;
                  allcPtr = allocedBlock;
                  break;
            }
            allocedBlock = allocedBlock->next;
      }

      size_t s = allcPtr->sz;
      curr = (FBlock*) ((void*)allcPtr->ptr) ;
      hSize-=allcPtr->sz;
      curr->sz = s;
      while(currFblock != NULL){
            if( currFblock - curr > 0){ 
                  curr->prev = prevFblock;
                  curr->next = currFblock;

                  if(prevFblock){
                      prevFblock->next = curr;  
                  }
                   currFblock->prev = curr; 

                  merge(curr);
                  //printf("free successful\n");
                  return;
                        
            }
             prevFblock = currFblock;

            if(currFblock->next == NULL){
                  break;
            }
            currFblock = currFblock->next;
      }

      if(curr == NULL){
            printf("error: not a malloced address\n");
            return;
      }
	return;

}

void* myrealloc(void* ptr, size_t size){
      if(ptr ==NULL && size == 0){
            return NULL;
      }

      if(ptr ==NULL){
            return mymalloc(size);

      }

      if(size == 0){
            
            myfree(ptr);

            return NULL;
      }
            
      Block* temp = (Block*) mallocBlock;
      while(temp!= NULL){
            if(temp->ptr == ptr){
                  
                  if(temp->sz == padding(size) && temp->isFree == 0){
                        
                        temp->payload = size;
                        return temp->ptr;
                  }else{
                        
                        if(temp->isFree == 0){
                              myfree(temp->ptr);
                              temp->isFree = 1;
                              
                        }
                        
                        void* curr = mymalloc(size);
                        return curr;
                        
                  }

            }
            temp= temp->next;
      }

      return NULL;
}
void mycleanup(){

      hSize = 0;
      free(heapBlock);
      free(mBlock);
}

double utilization(){

      double totalusersz= 0;
      double totalheapsz= 0;

      Block* temp = mallocBlock;
      if(temp==NULL){
            return 0;

      }else{

      while(temp->next != NULL){
            //printf("In HERE\n");
            if(temp->isFree ==0){
                 totalheapsz += temp->sz;
                  totalusersz += temp->payload;
            }

            temp= temp->next;
      }
      }

      
      totalusersz+= temp->payload;
      double totalhsz = (double)((void*)temp->ptr - (void*)heapBlock)+ temp->sz; 
      if(totalhsz == 0){
           return 0;
      }

      return  totalusersz/totalhsz;

}


size_t padding(size_t input){
      size_t final = input+24;
      if(final % 8 != 0){
            size_t ans = 8 - (final%8);
            return final + ans;
      }else{
            return final;
      }
}


void merge(FBlock* ptr){
      if(ptr==NULL){// if the prt is NULL
            return;
      }

      if(ptr->next ==NULL && ptr->prev==NULL){              // when no adjecent block 
           //printf("In merge1 ptr: %p\n",(void*)ptr);
            return;

      }else if(ptr->next == NULL){ 
            //printf("In merge2 ptr: %p\n",(void*)ptr);                         // bottom of the list 
            size_t ptrPrevsz = ptr->prev->sz;

            if(((void*)ptr->prev + ptrPrevsz) == ptr){

                  if(ptr == lastAllocatedBlock){
                        lastAllocatedBlock = ptr->prev;
                        //printf("LastBlock *Updated* in Merge 1: %p\n", (void*)lastAllocatedBlock);
                  }

                  ptr->prev->sz += ptr->sz;
                  ptr->prev->next = ptr->next;
                  return;
            }else{
                  return;
            }
            

      }else if(ptr->prev ==NULL){
                                                            //top of the list 
                  
            //printf("In merge3 ptr: %p\n",(void*)ptr); 
            size_t ptrsz = ptr->sz;


             if(((void*)(ptr)+ptrsz) == ptr->next){
                  FBlock* nextBlock = ptr->next; 

                 //nextBlock->prev = ptr;

                  if(nextBlock->next){
                        //printf("In merge3.1 ptr: %p\n",(void*)ptr); 
                        ptr->sz += nextBlock->sz;
                        nextBlock->next->prev = ptr;
                        ptr->next = nextBlock->next;
                      

                        if(globalFBlock == lastAllocatedBlock){
                              lastAllocatedBlock =ptr;
                        }
                        globalFBlock = ptr;

                        return;

                  }else{
                        //printf("In merge3.3 ptr: %p\n",ptr); 
                        ptr->sz += nextBlock->sz;
                        ptr->next = NULL;
                        if(globalFBlock == lastAllocatedBlock){
                              lastAllocatedBlock = ptr;
                        }
                        globalFBlock = ptr;
                        return;
                  }

            }
            
                  //not merged with anything so have to set ptr as Global block since its top of the list 
                  globalFBlock = ptr;

      }else{    
            //printf("In merge4 ptr: %p\n",(void*)ptr);  
            FBlock * prevBlock = ptr->prev;                                            // in middle somewhere 
            FBlock * nextBlock = ptr->next;

            size_t ptrPrevsz = ptr->prev->sz;
            size_t ptrsz = ptr->sz;
            

            if(((void*)prevBlock+ptrPrevsz)==ptr && ((void*)ptr + ptrsz) ==nextBlock ){         //Both adjacent Block needs to be merged
                  size_t totalsz = ptr->sz + prevBlock->sz + nextBlock->sz;

                  
                  
                  if(nextBlock == lastAllocatedBlock || ptr == lastAllocatedBlock ){
                        
                        lastAllocatedBlock = prevBlock;
                  }

                  prevBlock->sz = totalsz;

                  prevBlock->next = NULL;

                  if(nextBlock->next){
                        prevBlock->next = nextBlock->next;
                      nextBlock->next->prev = prevBlock; 
                      return;
                  }else{
                        return;       
                  }

            }else if(((void*)prevBlock+ptrPrevsz)==ptr ) {        //if the prev block needs to be merged
                  size_t totalsz = prevBlock->sz + ptr->sz;
                 
                  if(ptr == lastAllocatedBlock){
                        lastAllocatedBlock = prevBlock;
                  }


                  prevBlock->sz = totalsz;
                  prevBlock->next = ptr->next;

                  ptr->next->prev = prevBlock;

                  return;
                 
            }else if(((void*)ptr + ptrsz) == nextBlock ){          //if the next block needs to be merged

  
                  size_t totalsz = nextBlock->sz + ptr->sz;
                  ptr->sz = totalsz;

                  if(nextBlock == lastAllocatedBlock){
                        lastAllocatedBlock = ptr;
                  }

                  ptr->next = nextBlock->next;

                  if(nextBlock->next){
                        nextBlock->next->prev = ptr;
                  }

                  return;
            }
      }

}
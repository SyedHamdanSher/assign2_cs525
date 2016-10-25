#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer_mgr.h"
#include "STORAGE_MGR_H.h"
#include "dberror.h"

#define LRUCOUNTERFRAME 100
#define PAGEMAXSIZE 7000
#define FRAMEMAXSIZE 100

BM_MgmtData *mgmt=NULL;

//stores important information about buffer manager which is to be attached to mgmtdata of buffer manager
	typedef struct bufferPoolInfo{
		int pagesToPageFrame[PAGEMAXSIZE];//mapping of pageNumbers to page frames
		int pageFramesToPage[FRAMEMAXSIZE];//mapping of page frames to pages
		bool pageFrameDirtyBit[FRAMEMAXSIZE];// maintains dirty flags for each page frames
		int pageFrameFixedCount[FRAMEMAXSIZE];//maintains fixed count of each page frame
		int numReadIO;// stores total number of Read IO
		int numWriteIO;//stores total number of Write IO
		int totalFrames;//maintains total number of filled frames count
		int maxFrames;//stores number of page frames inside buffer pool
		int totalCount;//maintains total number of pages in frames of buffer pool
		SM_FileHandle filePointer;//stores file address of file
		struct pageFrame *head; //head of page frames linked list
		struct pageFrame *tail;//tail of page frames linked list
		int lruCounter4PageFrame[LRUCOUNTERFRAME];// LRU counter information
		struct pageFrame *lastNode;//maintains last node address of page frames list
	}bufferPoolInfo;
	
	//keeps the iformation regarding each node in linked list of page frames
	typedef struct pageFrame{
		bool dirtyBit; //dirty bit for page  true=dirty false= Not dirty
		int fixedCount;//fixed count to mark usage of page by client
		int pinUnpin; //pinning and unpinnig of page
		int pageNumber;//page number stored in buffer page frame
		int pageFrameNo;//frame number in page frames linked list
		int filled; // whether frame is filled or not
		char *data;//stores content of page.
		struct pageFrame *next;//pointer to next node in page frames linked list
		struct pageFrame *previous;//pointer to previous node in page frames linked list
	}pageFrame;


	//called when new page frame is created during initialization of buffer, each information is initialized with default value.
	pageFrame *getNewNode(){
		pageFrame *linkNode = calloc(PAGE_SIZE,sizeof(SM_PageHandle));
		linkNode->dirtyBit=false;
		linkNode->pinUnpin=0;
		linkNode->pageNumber=NO_PAGE;
		linkNode->pageFrameNo=0;
		linkNode->fixedCount=0;
		linkNode->filled=0;
		linkNode->next=NULL;
		linkNode->previous=NULL;
		linkNode->data=(char *)calloc(PAGE_SIZE,sizeof(SM_PageHandle));;
		return linkNode;
		
	}

	//storing the important information about buffer manager during initialization of buffer. 
	bufferPoolInfo *initBufferPoolInfo(const int numPages,SM_FileHandle fileHandle){
		bufferPoolInfo *bufferPool=calloc(PAGE_SIZE,sizeof(SM_PageHandle));
		bufferPool->numReadIO=0;
		bufferPool->numWriteIO=0; 
		bufferPool->totalFrames=0;
		bufferPool->totalCount=0;
		bufferPool->maxFrames=numPages;//setting to number of frames maintained by Buffer Mananger
		bufferPool->filePointer=fileHandle;//file used by buffer manager
		
		//allocating memory to each array of buffer information.
		memset(bufferPool->lruCounter4PageFrame,NO_PAGE,LRUCOUNTERFRAME*sizeof(int));
		memset(bufferPool->pagesToPageFrame,NO_PAGE,PAGEMAXSIZE*sizeof(int));
		memset(bufferPool->pageFramesToPage,NO_PAGE,FRAMEMAXSIZE*sizeof(int));
		memset(bufferPool->pageFrameDirtyBit,NO_PAGE,FRAMEMAXSIZE*sizeof(bool));
		memset(bufferPool->pageFrameFixedCount,NO_PAGE,FRAMEMAXSIZE*sizeof(int));
		return bufferPool;
	}

//used to create a buffer pool for an existing page file
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,const int numPages, ReplacementStrategy strategy,void *stratData)
{
	/*initBufferPool creates a new buffer pool with numPages page frames using the page replacement strategy strategy. 
	The pool is used to cache pages from the page file with name pageFileName  Initially, all page frames should be empty. The page file should already exist, i.e., this method should not generate a new page file.
	stratData can be used to pass parameters for the page replacement strategy. For example, for LRU-k this could be the parameter k.*/

}

//returns BM info.
	bufferPoolInfo *getMgmtInfo(BM_BufferPool *const bm){
		if(bm!=NULL){
		bufferPoolInfo *mgmtInfo=(bufferPoolInfo*)bm->mgmtData;
		return mgmtInfo;
		}
	}

	//returns fixed count value at each page frames.
	int *getFixCounts (BM_BufferPool *const bm){
		if(bm!=NULL){
			bufferPoolInfo *mgmtInfo=getMgmtInfo(bm);
			pageFrame *temp=mgmtInfo->head;//starting from head
			while(temp!=NULL){//traverse till there are no frames.
				//stores fixed count value at each page frame to an aaray
				(mgmtInfo->pageFrameFixedCount)[temp->pageFrameNo]=temp->fixedCount; 
				temp=temp->next;
			}
			free(temp);
			return mgmtInfo->pageFrameFixedCount;
		}
	}

	//returns total number of read operation done by BM from disk
	int getNumReadIO (BM_BufferPool *const bm){
		if(bm!=NULL){
			bufferPoolInfo *mgmtInfo=getMgmtInfo(bm);
			return mgmtInfo->numReadIO;
		}
	}

	
	//returns an array of dirty flags at each page frame of BM
	bool *getDirtyFlags (BM_BufferPool *const bm){
		if(bm!=NULL){
		bufferPoolInfo *mgmtInfo=getMgmtInfo(bm);
		pageFrame *temp=mgmtInfo->head;
			while(temp!=NULL){
				//stores dirty bit value at each page in page frame.
				(mgmtInfo->pageFrameDirtyBit)[temp->pageFrameNo]=temp->dirtyBit;
				temp=temp->next;
			}
		free(temp);
		return mgmtInfo->pageFrameDirtyBit;
		}
	}
	
	//returns total number of write operation performed by BM to disk.
	int getNumWriteIO (BM_BufferPool *const bm){
		if(bm!=NULL){
		bufferPoolInfo *mgmtInfo=getMgmtInfo(bm);
		return mgmtInfo->numWriteIO;
		}
	}
//shutdown a buffer pool and free up all associated resources (shutdownBufferPool)
RC shutdownBufferPool(BM_BufferPool *const bm)
{

}
// to force the buffer manager to write all dirty pages to disk
RC forceFlushPool(BM_BufferPool *const bm)
{

}
/*
Page Management Functions

These functions are used pin pages, unpin pages, mark pages as dirty, and force a page back to disk.
*/
//markDirty marks a page as dirty.
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page)//Syed
{

}
//unpinPage unpins the page page. The pageNum field of page should be used to figure out which page to unpin.
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)//Syed
{

}
//forcePage should write the current content of the page back to the page file on disk.
RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)//Syed
{

}

//returns an array page numbers of page stored in page frames
	PageNumber *getFrameContents (BM_BufferPool *const bm){
		bufferPoolInfo *mgmtInfo=getMgmtInfo(bm);
		return mgmtInfo->pageFramesToPage;
	}

	//finds and returns node's page with pageNumber with pageNum
	pageFrame *locateNode(BM_BufferPool *const bm,BM_PageHandle *const page,const PageNumber pageNum ){	 
	if(bm!=NULL){
			bufferPoolInfo *mgmtInfo = getMgmtInfo(bm);
			pageFrame *exist=mgmtInfo->head;
			   while(exist!=NULL){
				   if(exist->pageNumber==pageNum){//compares frame page's page number with pageNum to be search
						return exist;
					 }
					exist=exist->next;
				}
				return NULL;
		}else{
			RC_message="Buffer is not initialized ";
			return NULL;
		}
	}//
	
	//finds if page to be read by client is already there in memory or not
	pageFrame *locateNodeinMemory(BM_PageHandle *const page,const PageNumber pageNum,BM_BufferPool *const bm){
		if(bm!=NULL){
		bufferPoolInfo *mgmtInfo=getMgmtInfo(bm);
		pageFrame *temp=mgmtInfo->head;
		pageFrame *exist;
		 if((mgmtInfo->pagesToPageFrame)[pageNum] != NO_PAGE){
			if(((exist = locateNode(bm,page, pageNum)) != NULL)){
			//if page to be read is already in memory then increase the fixed count of page
			//and set the page handle information
			exist->fixedCount+=1;
			page->pageNum = pageNum;
			page->data = exist->data; 
			return exist;
		}else{
			return NULL;
		}
		}else{
			return NULL;
		}
	}else{
		RC_message="Buffer is not initialized ";
		return NULL;
	}
	
	}
	
	//called at the time of page replacement
	void modifyPageHead(BM_BufferPool *const bm,pageFrame *node){
		
		if(bm!=NULL){
					bufferPoolInfo *mgmtInfo=getMgmtInfo(bm);
					pageFrame *h=mgmtInfo->head;
					
					//if page to be replaced is at head of page frame linked list then no replacement
					if(node==mgmtInfo->head){
						return;
					}
					
					
					else{
						//adjust page frames if page to be replaced is last node in the page frames linked list
						if(node==mgmtInfo->lastNode){
							pageFrame *t = mgmtInfo->lastNode->previous;
							mgmtInfo->lastNode = t;
							t->next = NULL;
							h->previous = node;
							node->next = h;
							mgmtInfo->head=node;
							node->previous = NULL;
							mgmtInfo->head->previous=NULL;
							mgmtInfo->head=node;
							mgmtInfo->head->previous=NULL;
							return;
						}else{
							//adjust page frames if page to be replaced, if its in-between node
							node->previous->next = node->next;
							node->next->previous = node->previous;
							h->previous = node;
							node->next = h;
							mgmtInfo->head=node;
							node->previous = NULL;
							mgmtInfo->head->previous=NULL;
							mgmtInfo->head=node;
							mgmtInfo->head->previous=NULL;
							return;
						}
							
				}
			
		}else{
			return;
		}
	}//
//pinPage pins the page with page number pageNum. The buffer manager is responsible to set the pageNum field of the page handle passed to the method. 
//Similarly, the data field should point to the page frame the page is stored in (the area in memory storing the content of the page).
RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)//Syed
{

}

RC Fifo_strategy(BM_BufferPool *const bm, const PageNumber pageNum)  
{

}

RC Lru_strategy(BM_BufferPool *const bm, const PageNumber pageNum)  
{

}

//called at the time of replacement of pages in page frame
	RC updatePage(BM_BufferPool *const bm,BM_PageHandle *const page,pageFrame *node,const PageNumber pageNum){
		bufferPoolInfo *mgmtInfo = getMgmtInfo(bm);
		RC flag;
		SM_FileHandle fileHandle;
		if(bm!=NULL){
			if ((flag = openPageFile ((char *)(bm->pageFile), &fileHandle)) == RC_OK){
			//if page to be replaced is dirty then write it back to disk before replacement 
			if(node->dirtyBit){
				ensureCapacity(pageNum, &fileHandle);
				if((flag = writeBlock(node->pageNumber,&fileHandle, node->data)) == RC_OK){
					(mgmtInfo->numWriteIO)+=1;
			  }else{
				  return flag;
				}		
			}
			//ensuing capacity of file before reading the page from file
			ensureCapacity(pageNum, &fileHandle);
			//reading the pageNum from file to data field.
			if((flag = readBlock(pageNum, &fileHandle, node->data)) == RC_OK){
				(mgmtInfo->pagesToPageFrame)[node->pageNumber] = NO_PAGE;
				node->pageNumber = pageNum;
				(mgmtInfo->pagesToPageFrame)[node->pageNumber] = node->pageFrameNo;
				(mgmtInfo->pageFramesToPage)[node->pageFrameNo] = node->pageNumber;
				node->dirtyBit = false;
				node->fixedCount = 1;
				page->pageNum = pageNum;
				page->data = node->data;
				mgmtInfo->numReadIO+=1;
			}else{
					return flag;
				}
			return RC_OK;
			   }else{
				   return flag;
			  }
		 }else{
			 RC_message="Buffer is not initialized ";
			 return RC_BUFFER_NOT_INITIALIZED;
			}
	}
	
	//calls modify page head
	void update(BM_BufferPool *const bm,pageFrame *node,const PageNumber pageNum){
		int i=0,j=0;
		for(i=0;i<bm->numPages;i++){
			j++;
		}
		modifyPageHead(bm,node);
	}
	
	//calls updatepage function
	RC updatePageFrame(BM_BufferPool *const bm,BM_PageHandle *const page,pageFrame *node,const PageNumber pageNum){
		int i=0,j=0;
		RC flag;
		for(i=0;i<bm->numPages;i++){
			j++;
		}
		flag=updatePage(bm, page, node, pageNum);
		return flag;
	}

PageNumber *getFrameContents (BM_BufferPool *const bm)
{ 

}

bool *getDirtyFlags (BM_BufferPool *const bm)
{

}

int *getFixCounts (BM_BufferPool *const bm)
{ 

}

int getNumReadIO (BM_BufferPool *const bm)
{

}

int getNumWriteIO (BM_BufferPool *const bm)
{  

}

	//This page replacement algorithm will replace the page from page frame which has read into meory first
	RC fifo (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum){
	if(bm!=NULL){
				bufferPoolInfo *mgmtInfo=getMgmtInfo(bm);
				RC flag;
				pageFrame *existStatus;
				int j;
				
				//if page to be read is already in memory then return pointer to that frame
				if((existStatus=(locateNodeinMemory(page,pageNum,bm)))!=NULL){
					return RC_OK;
				}
				
				//check if there are any empty frames in memory, if yes then read page to that frame from disk file.
				if((mgmtInfo->totalFrames) < mgmtInfo->maxFrames){
					existStatus = mgmtInfo->head;
			   
					for(j=0;j<mgmtInfo->totalFrames;++j){
						existStatus = existStatus->next;
					}
					//empty frame is found so read page to page frame and increase the total page count in page frames.
					(mgmtInfo->totalFrames)+=1;
					update(bm, existStatus,pageNum);
					
					//reading page to page frame
					flag = updatePageFrame(bm, page, existStatus, pageNum);
						if(flag!=RC_OK){
								return flag;
						}
					return RC_OK;	
				}//
				else{
					//if page frame is not empty then search for a page which has come first with fixed count=0
					existStatus = mgmtInfo->lastNode;
					for(j=0;(existStatus != NULL && existStatus != NULL && existStatus->fixedCount != 0);j++){
						existStatus = existStatus->previous;
					}
					update(bm, existStatus,pageNum);
					//read the page from disk file to the frame whose page is being replaced.
					flag = updatePageFrame(bm, page, existStatus, pageNum);
						if(flag!=RC_OK){
								return flag;
						}
					return RC_OK;	
				}
							
							
			
	}else{
			RC_message="Buffer is not initialized ";
			return RC_BUFFER_NOT_INITIALIZED;
	}
}	
			

//this page replacement strategy will find and replace the page which is least recently used by client
RC lru (BM_BufferPool *const bm, BM_PageHandle *const page,const PageNumber pageNum)
{
	if(bm!=NULL){
			RC flag;
			pageFrame *existStatus;
			bufferPoolInfo *mgmtInfo=getMgmtInfo(bm);
			int j;
			//if page to be read is already in memory then return the pointer to page to client
			if((existStatus = locateNodeinMemory(page,pageNum,bm)) != NULL){
				update(bm, existStatus,pageNum);
				return RC_OK;
			}
			
			//check if there are any empty frames in memory, if yes then read page to that frame from disk file.
			if((mgmtInfo->totalFrames) < mgmtInfo->maxFrames){
					existStatus = mgmtInfo->head;
						for(j=0;j<mgmtInfo->totalFrames;++j){
							existStatus = existStatus->next;
						}
		//empty frame is found so read page to page frame and increase the total page count in page frames.
					mgmtInfo->totalFrames+=1;
					
				//reading page to page frame
					if((flag = updatePageFrame(bm, page, existStatus, pageNum)) == RC_OK){
							update(bm, existStatus,pageNum);
					
					}else{
							return flag;
					}
					return RC_OK;
			}
			else{
				//if no page frame is empty then serach for a page which is least recently used in past with fixed count 0.
				existStatus = mgmtInfo->lastNode;      
				while(existStatus != NULL && existStatus->fixedCount != 0){
					existStatus = existStatus->previous;
				}
				
				//if frame is found with page fixed count 0 then read the page to that page frame.
				//if no frames are found with fixed count 0 then return error
				if (existStatus != NULL){
					if((flag = updatePageFrame(bm, page, existStatus, pageNum)) == RC_OK){
						update(bm, existStatus,pageNum);
					}else{
						return flag;
					}
					return RC_OK;
				}else{
					RC_message="No Frame is availble for page to load ";
					   return RC_NO_MORE_EMPTY_FRAME;
				}
			}
    
	}else{
		RC_message="Buffer is not initialized ";
		return RC_BUFFER_NOT_INITIALIZED;
	}
}

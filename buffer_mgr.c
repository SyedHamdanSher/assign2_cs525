#include <stdio.h>
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include "dberror.h"
#include <stdlib.h>
#include <string.h>

//#define FRAMELRU 100
#define bufferPoolInfo* bufferNode;
#define pageFrame* pageNode;

/******************************************Creating structure for bufferPool and pageFrames************************************/
//stores important information about buffer manager for mgmtdata
typedef struct bufferPoolInfo{

		int num_Read_IO;// stores total number of Read IO
		int num_Write_IO;//stores total number of Write IO
		bool page_Frame_Dirty[FRAMEMAX];// maintains dirty flags for each pageFrames
		int page_Frame_Fixed_Count[FRAMEMAX];//maintains fixed count of each pageFrame
		
		
		int pages_To_Page_Frame[PAGEMAX];//mapping of pageNumbers to page frames
		int page_Frames_To_Page[FRAMEMAX];//mapping of page frames to pages
		int total_Frames_Filled;//stores total number of filled frames count
		int max_Frames;//stores number of pageFrames inside buffer pool
		int lRU_Counter_PageFrame[FRAMELRU];// LRU counter information
		int total_Count_Pages;//stores total number of pages in frames of bufferPool

		SM_FileHandle filePointer;//stores file address of file
		struct pageFrame *head; //head of page frames linked list
		struct pageFrame *tail;//tail of page frames linked list
		struct pageFrame *lastNode;//maintains last node address of page frames list
	}bufferPoolInfo;
	
	//keeps the iformation regarding each node in linked list of page frames
	typedef struct pageFrame{
		bool dirty_Bit; //dirty bit for page  true=dirty false= Not dirty
		int page_Number;//page number stored in buffer pageFrame
		int page_Frame_No;//frame number in page frames linked list
		
		int fixed_Count_Marked;//fixed count to mark usage of page by client
		int pinning; //pinning and unpinnig of page
		int filled; // whether frame is filled or not	
		
		char *data;//stores content of page.
		struct pageFrame *next;//pointer to next node in page frames linked list
		struct pageFrame *previous;//pointer to previous node in page frames linked list
	}pageFrame;

/***********************************************************************************************************************************/

/******************************************Initializing Nodes for bufferPool and pageFrames*****************************************/
	//called when new page frame is created during initialization of buffer, each information is initialized with default value.
	pageNode getNewNode(){
		pageNode Node = calloc(PAGE_SIZE,sizeof(SM_PageHandle));
		linkNode->dirty_Bit=false;
		linkNode->page_Number=NO_PAGE;
		linkNode->page_Frame_No=0;
		
		linkNode->fixed_Count_Marked=0;
		linkNode->pinning=0;
		linkNode->filled=0;
		linkNode->next=NULL;
		linkNode->previous=NULL;
		linkNode->data=(char *)calloc(PAGE_SIZE,sizeof(SM_PageHandle));;
		return Node;
	}

	//storing the important information about buffer manager during initialization of buffer. 
	bufferNode initBufferPoolInfo(const int numPages,SM_FileHandle fileHandle){
		bufferNode bufferPool=calloc(PAGE_SIZE,sizeof(SM_PageHandle));
		bufferPool->num_Read_IO=0;
		bufferPool->num_Write_IO=0; 
		bufferPool->total_Frames_Filled=0;
		bufferPool->total_Count_Pages=0;
		bufferPool->max_Frames=numPages;//setting to number of frames maintained by BufferMananger
		bufferPool->filePointer=fileHandle;//file used by buffer manager
		
		//allocating memory
		memset(bufferPool->pages_To_Page_Frame,NO_PAGE,PAGEMAX*sizeof(int));
		memset(bufferPool->page_Frames_To_Page,NO_PAGE,FRAMEMAX*sizeof(int));
		memset(bufferPool->lRU_Counter_PageFrame,NO_PAGE,FRAMELRU*sizeof(int));
		memset(bufferPool->page_Frame_Dirty,NO_PAGE,FRAMEMAX*sizeof(bool));
		memset(bufferPool->page_Frame_Fixed_Count,NO_PAGE,FRAMEMAX*sizeof(int));
		return bufferPool;
	}
/***********************************************************************************************************************************/

	//creates a new buffer pool with numPages page frames using the page replacement strategy strategy. 
	//The pool is used to cache pages from the page file with name pageFileName. Initially, all page frames should be empty. 
	//The page file should already exist, i.e., this method should not generate a new page file
	RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, const int numPages, ReplacementStrategy strategy, void *stratData)
	{
		
		SM_FileHandle fileHandle;
		int i=1;
		//openingPageFile using storage_mgr.c function
		if (openPageFile ((char *)pageFileName, &fileHandle) == RC_OK){
		bufferNode bufferPool=initBufferPoolInfo(numPages,fileHandle);
		
		//setting bufferManager field values
		bm->numPages=numPages;
		bm->pageFile=(char *)pageFileName;
		bm->strategy=strategy;//stores strategy to be used by BM when the page is to be replaced in frames.
		bufferPool->head=bufferPool->tail=getNewNode();
		bufferPool->head->page_Frame_No=0;
		
		//creating pageFrame Nodes which equals to numPages and using int i variable to use it as page_Frame_No of each frames
		while(i<numPages){
			bufferPool->tail->next = getNewNode();
			bufferPool->tail->next->previous = bufferPool->tail; //insert in the front
			bufferPool->tail = bufferPool->tail->next; //linking the link lists
			bufferPool->tail->page_Frame_No = i;
			i++;
		}

		bufferPool->lastNode=bufferPool->tail;
		bm->mgmtData=bufferPool;//storing bufferPoolInfo to managementData of BufferManagement to be used by various functions of BufferManager
		return RC_OK;
		}else{
		//RC_message="File getting opened doesn't exist";
		return RC_FILE_NOT_FOUND;
		}
	}
	
	
	
	//this function returns bufferManagerInfo.
	bufferNode getMgmtInfo(BM_BufferPool *const bm){
		if(bm!=NULL){
		bufferNode mgmtInfo=(bufferNode)bm->mgmtData;
		return mgmtInfo;
		}
        else{
            //RC_message="File to be opened doesn't exist";
            return RC_FILE_NOT_FOUND;
        }
	}
	

	//this function returns page_Frame_Fixed_Count value at each pageFrames.
	int *getFixCounts (BM_BufferPool *const bm){
		if(bm!=NULL){
			bufferNode mgmtInfo=getMgmtInfo(bm);
			pageNode tmp=mgmtInfo->head;//starting from head
			while(tmp!=NULL){//traverse till there are no frames.
				//stores fixed count value at each page frame to an aaray
				(mgmtInfo->page_Frame_Fixed_Count)[tmp->page_Frame_No]=tmp->fixed_Count_Marked; 
				tmp=tmp->next;
			}
			free(tmp);
			return mgmtInfo->page_Frame_Fixed_Count;
		}
        else{
            //RC_message="File to be opened doesn't exist";
            return RC_FILE_NOT_FOUND;
        }
	}
	
	//this function returns total num_Read_IO i.e. read operation done by BufferManager from disk
	int getNumReadIO (BM_BufferPool *const bm){
		if(bm!=NULL){
			bufferNode mgmtInfo=getMgmtInfo(bm);
			return mgmtInfo->num_Read_IO;
		}
        else{
            //RC_message="File to be opened doesn't exist";
            return RC_FILE_NOT_FOUND;
        }
	}

	
	//this function returns page_Frame_Dirty i.e. an array of dirtyFlags at each pageFrame of BufferManager
	bool *getDirtyFlags (BM_BufferPool *const bm){
		if(bm!=NULL){
		bufferNode mgmtInfo=getMgmtInfo(bm);
		pageNode tmp=mgmtInfo->head;
			while(tmp!=NULL){
				//stores dirty bit value at each page in page frame.
				(mgmtInfo->page_Frame_Dirty)[tmp->page_Frame_No]=tmp->dirty_Bit;
				tmp=tmp->next;
			}
		free(tmp);
		return mgmtInfo->page_Frame_Dirty;
		}
        else{
            //RC_message="File to be opened doesn't exist";
            return RC_FILE_NOT_FOUND;
        }
	}
	
	//this function returns total num_Write_IO i.e. Write operation done by BufferManager from disk
	int getNumWriteIO (BM_BufferPool *const bm){
		if(bm!=NULL){
		bufferNode mgmtInfo=getMgmtInfo(bm);
		return mgmtInfo->num_Write_IO;
		}
        else{
            //RC_message="File to be opened doesn't exist";
            return RC_FILE_NOT_FOUND;
        }
	}
	
	//this function marks page specified in page->pageNum as dirty
	RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page){
		if(bm!=NULL){
			bufferNode mgmtInfo=getMgmtInfo(bm);
			pageNode tmp=mgmtInfo->head;
			while(tmp!=NULL){
				if(tmp->page_Number==page->pageNum){//searches page to be marked dirty in page frames.
					tmp->dirty_Bit=true;//marking page as dirty.
				}
				tmp=tmp->next;
			}
			free(tmp);
            return RC_OK;
			
		}else{
			//RC_message="Buffer is not initialized ";
			return RC_BUFFER_NOT_INITIALIZED;
		}
	}
	
	//unpins the page page. The pageNum field of page should be used to figure out which page to unpin.
	//unpins the page specified in page->pageNum only if the page has fixed_Count greater than 1.
	RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page){
		if(bm!=NULL){
			bufferPoolInfo *mgmtInfo=getMgmtInfo(bm);
			pageFrame *tmp=mgmtInfo->head;
			while(tmp!=NULL){
				if(tmp->page_Number==page->pageNum && tmp->fixed_Count_Marked>0){
					tmp->fixed_Count_Marked-=1;//decreasing fixed count value of page.
				}
				tmp=tmp->next;
			}
			free(tmp);
			return RC_OK;
		}else{
			//RC_message="Buffer is not initialized ";
			return RC_BUFFER_NOT_INITIALIZED;
		}
        
    }
	
	

	//pins the page with page number pageNum.
	//The buffer manager is responsible to set the pageNum field of the page handle passed to the method
	RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum){
	if(bm!=NULL){
		
			if(bm->strategy==RS_FIFO){
				 fifo_Technique(bm,page,pageNum);//FIFO.
				
			}else if(bm->strategy==RS_LRU){
				lru_Technique(bm,page,pageNum);//LRU
			}
				return RC_OK;
			}else{
				//RC_message="Buffer is not initialized ";
				return RC_BUFFER_NOT_INITIALIZED;
			}
	}
		
	//destroys a buffer pool. This method should free up all resources associated with buffer pool. 
	//For example, it should free the memory allocated for page frames. 
	//If the buffer pool contains any dirty pages, then these pages should be written back to disk before destroying the pool. 
	//It is an error to shutdown a buffer pool that has pinned pages.
	RC shutdownBufferPool(BM_BufferPool *const bm){
		if(bm!=NULL){
			forceFlushPool(bm);//flushes the page frames and writes dirty pages return to disk
			bufferPoolInfo *mgmtInfo=getMgmtInfo(bm);
			pageFrame *temp=mgmtInfo->head;
			int i;
			for(i=0;temp!=NULL;i++){
				
				//assignes each page frames to head and releases head node.
				free(mgmtInfo->head->data);
				free(mgmtInfo->head);
				temp=temp->next;
				mgmtInfo->head=temp;
			}
		//makes head and tail node of linked listpage frames to NULL
		mgmtInfo->head=NULL;
		free(temp);
		mgmtInfo->tail=NULL;
		return RC_OK;
		}else{
			RC_message="Buffer is not initialized ";
			return RC_BUFFER_NOT_INITIALIZED;
		}
	}

	
	//flushesall pages in page frames and if any page is dirty then write it back to disk.
	RC forceFlushPool(BM_BufferPool *const bm){
		if(bm!=NULL){
			SM_FileHandle fileHandle;
			bufferPoolInfo *mgmtInfo=getMgmtInfo(bm);
			pageFrame *temp=mgmtInfo->head;
			if (openPageFile ((char *)(bm->pageFile), &fileHandle) == RC_OK){
			while(temp != NULL){
			if(temp->dirtyBit){//if dirty flag is true
			//write the page back to disk file
            if(writeBlock(temp->pageNumber, &fileHandle, temp->data) == RC_OK){
				temp->dirtyBit = false;
				(mgmtInfo->numWriteIO)+=1;//updtaes write count of BM by 1.
			}else{
			  return RC_WRITE_FAILED;
			}
			}
			temp = temp->next;
			}
			free(temp);
			return RC_OK;
		}else{
			RC_message="file to be opened doesn't exist";
			return RC_FILE_NOT_FOUND;
		}
		}else{
			RC_message="Buffer is not initialized ";
			return RC_BUFFER_NOT_INITIALIZED;
		}
		
	}
	
	RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page){
		if(bm!=NULL){
			SM_FileHandle fileHandle;
			bufferPoolInfo *mgmtInfo=getMgmtInfo(bm);
			pageFrame *temp=mgmtInfo->head;
		if (openPageFile ((char *)(bm->pageFile), &fileHandle) == RC_OK){		
			while(temp!=NULL){
				if(temp->pageNumber==page->pageNum && temp->dirtyBit){
					if(writeBlock(temp->pageNumber,&(mgmtInfo->filePointer),temp->data)==RC_OK){
						temp->dirtyBit=false;
						mgmtInfo->numWriteIO+=1;
					}
				}
				temp=temp->next;
			}
			free(temp);
			return RC_OK;
		}else{
			RC_message="file to be opened doesn't exist";
			return RC_FILE_NOT_FOUND;
		}
		}else{
			RC_message="Buffer is not initialized ";
			return RC_BUFFER_NOT_INITIALIZED;
		}
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
		
	//This page replacement algorithm will replace the page from page frame which has read into meory first
	RC fifo_Technique (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum){
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
RC lru_Technique (BM_BufferPool *const bm, BM_PageHandle *const page,const PageNumber pageNum)
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

	

	
	

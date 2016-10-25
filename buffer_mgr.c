#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include "dberror.h"

BM_MgmtData *mgmt=NULL;

RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName,const int numPages, ReplacementStrategy strategy,void *stratData)
{

}

RC shutdownBufferPool(BM_BufferPool *const bm)
{

}

RC forceFlushPool(BM_BufferPool *const bm)
{

}

RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page)
{

}

RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)
{

}

RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)
{

}

RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
{

}

RC Fifo_strategy(BM_BufferPool *const bm, const PageNumber pageNum)  
{

}

RC Lru_strategy(BM_BufferPool *const bm, const PageNumber pageNum)  
{

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
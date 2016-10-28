 ( Name, CWID, Email id, Leader Name)
1. Ericson Akatakpo, A20349354, eakatakp@hawk.iit.edu, Ericson Akatakpo
2. Syed Hamdan Sher, A20378710, ssher1@gmail.com, Ericson Akatakpo
3. James Mwakichako, A20297757, jmwakich@hawk.iit.edu, Ericson Akatakpo
4. Gaurav Gadhvi, A20344904, ggadhvi@gmail.com, Ericson Akatakpo


*********************************************************
Steps to Execute the buffer manager 

1.	Get all file into a single folder
2.	Move to folders where files are stored in Unix Terminal
3.	Execute the make file with the “make” command
4.	Execute the test with the “./test_assign2” command

The complete structure of the buffer manager
******************************************


The execution process for Buffer Manager begins with building the code with Make file 
which is responsible for creating the test_assign2 Binary file and linking 
test_assign2_1.c file with all corresponding *.h and *.c files. 
Make file which creates the test_assign2_1 binary file and links the test_assign2_1.c with the 
Exact *.h and *.c files
Make file inter-links each of the files in the directory with each other.



File list:

1. dberror.h
2. dberror.c
3. storage_mgr.c
4. storage_mgr.h
5. test_assign2_1.c
6. test_helper.h
7. buffer_mgr.c
8. buffer_mgr.h
9. buffer_mgr_stat.c
10. dt.h
11. test_assign1_1.c

Structure of Buffer manager

buffer_mgr.h --> It has definition for various structures and functions to be used by 
buffer_mgr.c. 

buffer_mgr.c --> It is the main file which contains the Buffer manager function used to initialize
buffer as per the number of PageFrames. It is used to copy file in pages from disk to 



The execution process for the buffer manager begins with the execution of the make file 
Which creates the test_assign2_1 Binary file and links the test_assign2_1.c with the 
Exact *.h and*.c files.
The make also inter-links every file in the directory with each other
buffer_mgr.h === > 
It defines the various functions and stuctures to be used by buffer_mgr.c

buffer_mgr.c === >
Its function is to initialize the buffer as per the number of PageFrames and
Also stores important information about buffer manager for mgmtdata. It is also used to copy
Files in pages from disk to the PageFrame. It checks for Dirty bits and fixedcount of pages
To write the edited page back to the disk. The buffer_mgr.c contain FIFO and LRU agorithm to evict pages based on algorithm strategy if page frames are full.

buffer_mgr_stat.c === > This file is made up of the BufferPool statistic functions.

buffer_mgr_stat.h === > This file is made up of the BufferPool statistic functions definitions.

dberror.c === > It files is made up of the errormessage and printerror function. 
the specific error it outputs the specific and error code error message

dberror.h === > this files defines page_size constant as 4096 and  a complete definition for
RC(return codes) for various Block and File functions in storage_mgr.
 
dt.h === > It contains is made up of variables for True and False. 
 
storage_mgr.h  === > This is the file responsible for containing various block and file
functions which are called from test_assign1_1.c It is made up of the read and write function from
file on disk to buffer and vise-versa. It contains creating, closing and opening file 
functions as well. It implements test_helpers.h and dberror.h file.
 
test_assign2_1.c === > This file calls upon each and every function within Buffer_mgr.c file.
We have added 3 extra test cases to test FIFO, Least Recently Used and the buffer Manager 
the test cases are testBufferManager(), test_lru_2(), and test_fifo_2().

test_helper.h === >  The file contains validation tasks for each function that are called 
within test_assign1. Its major function is for printing the RC code and RC message passed 
based on function's success or failure depnding on the values being passed to it.



The Complete Buffer Manager Functions:-
*******************************************************
1. initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, 
		  const int numPages, ReplacementStrategy strategy, 
		  void *stratData)
This function initializes the buffer Pool manager. It also allocates memory to variables
pointed by bufferpool using memset function and initializes few others to zero.
It creates a subsequent and Head node frames in BufferPool.

*******************************************************

2. int *getFixCounts (BM_BufferPool *const bm)
The major function is to loop through all the frames created in BufferPool and returns the PageFrameFixedCount
for each frame in BufferPool.

 *******************************************************

3. int getNumReadIO (BM_BufferPool *const bm)
The major function is to returns the number of Pages that have been read from disk 
from the time BufferPool is been initialized.

 *******************************************************

4. int getNumWriteIO (BM_BufferPool *const bm)
The major function is to returns the number of Pages that have been written to disk from the time
BufferPool is been initialized.
 
*******************************************************


5. bool *getDirtyFlags (BM_BufferPool *const bm)
The major function is to loops throught the PageFrames in BufferPool and returns the boolean value after completely checking which frame in the BufferPool is Dirty.
 
*******************************************************

6. markDirty (BM_BufferPool *const bm, BM_PageHandle *const page)
The major function is to loops through the PageFrames in BufferPool and checks if the Page within page frame is same as page to be modified by client then mark page as dirty.

*******************************************************

7. unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)
The major function is to loops through the PageFrames in BufferPool and finds the page to be unPinned
as well as decreasing  its Fixed count by 1.

*******************************************************

8. pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
The major function is to pins the Page with pageNum in PageFrame of BufferPool. It also load the page from disk file to PageFrame. It is a main function which calls upon FIFO algorithmor LRU algorithm based on the strategy requested if there are no empty page frames.
*******************************************************

9. shutdownBufferPool(BM_BufferPool *const bm)
The major function is to first call the forceFlushPool(bm).
This function then proceeds to shuts down a BufferPool while looping through all the 
PageFrames in BufferPool to check if a page is marked dirty, if so then they have to be 
written back to file on disk. It also frees up all the memory allocated to PageFrames.

*******************************************************

10. forceFlushPool(BM_BufferPool *const bm)
The major function is to loops through all the PageFrame in BufferPool and checks if a page is dirty.
It then checks if the fixedcount for the page is 0 and if so then it writes the page content
 to the file on disk.

*******************************************************

11. forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)
The major function is to write the content of current page having dirtybit, to file on disk.

*******************************************************

12. PageNumber *getFrameContents (BM_BufferPool *const bm)
The major function is to returns the pagenumber of page stored in page frame.
*******************************************************

13. locateNode(BM_BufferPool *const bm,BM_PageHandle *const page,const PageNumber pageNum )
The major function is to locates the page requested in PageFrame of BufferPool as per the pageNum 
value passed to it and returns page pointer if found or returns NULL.


*******************************************************

14. locateNodeinMemory(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
The major function is to calls the locateNode(bm,page, pageNum) function to find the requested page
in PageFrame of BufferPool. If the page is found then fixedcount of page increases and page 
data gets copied to Page in PageFrame. If the Page is not found then it return NULL.

*******************************************************

15. modifyPageHead(BM_BufferPool *const bm,pageFrame *node)
The major function is to loops through the Pages in PageFrame of BufferPool which needs to be replaced and update the pointers within page frames accordingly.

*******************************************************

16. updatePage(BM_BufferPool *const bm,BM_PageHandle *const page,pageFrame *node,const PageNumber pageNum)
The major function is to calls various Storage Manager functions from storage_mgr.c
like openPageFile, ensureCapacity, writeBlock and readBlock.
Function is responsible to replace the page and read the requested page to replaced page frame.
*******************************************************

17. fifo (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
The major function is to implements the FIFO algorithm by calling the 
locateNodeinMemory(bm,page,pageNum) function to find if the PageFrame
already has the page requested by the client. if not then it checks for an empty page frame and loads page into empty page frame. if no page frames are empty then it locates and replaces the page which has come first in page frame having fixed count =0. 

*******************************************************

18. LRU (BM_BufferPool *const bm, BM_PageHandle *const page,const PageNumber pageNum)
The major function is to implements the LRU algorithm by calling the 
locateNodeinMemory(bm,page,pageNum) function to find if the PageFrame
already has the page requested by the client. 
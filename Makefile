all:
	 gcc -o test_assign2 test_assign2_1.c buffer_mgr.c buffer_mgr_stat.c storage_mgr.c dberror.c
clean:
	rm -rf ./test_assign2 ./testbuffer.bin ./docs

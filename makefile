build:
	gcc file_processor_singlethreaded.c -o fp_single.exe -Wall -lm
	gcc file_processor_multithreaded.c -o fp_multi.exe -Wall -lm
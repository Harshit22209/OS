Group No: 14

Members:
	Harshit Gupta 2022209
	Abhay Kohli   2022015


**We have done this assignment with bonus(i.e page-by-page allocation of segment is done)**
How to run the programme:

Step 1:run " export LD_LIBRARY_PATH=<dir to bin>:$LD_LIBRARY_PATH "

Step 2:run " make compile "

**To clean run " make clean "

How code works:

1. The program includes a signal handler "sigFaultHandler",to handle segmentation faults.

2. It calculates the page index associated with the error address. If the page doesn't load, it increments the "noPageFaults" counter.

3. When a page fault occurs, it allocates memory for the page using mmap and maps it to the desired virtual memory address.

4. The loader calculates and increments internal fragments to track any unused space within the page.

5. The program maintains an array of "struct page" to keep track of pages.whenever page fault occurs,with some calculation i can easily find out which page is to be loaded

6. At the beginning of the program, the _start method is executed, note that _start method is executed before loading the pages, which typically results in a segmentation fault.

7. When a segmentation fault occurs, the sigFaultHandler function is called, this function calculates which page in the program caused the fault and checks if the page is already loaded. If the page is not loaded, it allocates memory for the page and maps it to the appropriate virtual memory address.

8. The internalfragments variable is incremented to account for any unused space within the page. After handling the segmentation fault, the page is marked as loaded in the pages array.

9. The handling of segmentation faults for other pages is similar to how entry point addresses are handled.

10. After the program execution, statistics are printed to the terminal. This includes information about the number of page faults, page allocations, and internal fragmentation.

Contributions-

	Harshit Gupta-Implemented Smart loader
	Abhay Kohli-Create readme file,helped in getting resources for assignment


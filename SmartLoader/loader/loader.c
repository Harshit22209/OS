
#include "loader.h"
#define __USE_GNU
#include <ucontext.h>
#include <sys/ucontext.h>
Elf32_Ehdr *ehdr;
Elf32_Phdr *phdr;
// It stores the memory required and whether the memory is already allocated or not
struct page
{
  int isLoaded;
  int memsz;
};
// It stores the pages for each program header
struct pageTable
{
 
  int noOfPages;
  struct page *pages;
  
};
struct pageTable *table;

int fd;

int noOfPageFaults = 0;
int noOfPageAllocations = 0;
float internalfragments = 0;
/*
 * release memory and other cleanups
 */
void loader_cleanup()
{
  // free(ehdr);
  free(phdr);
  
  free(table);

}
typedef int (*start)();
/*
 * Load and run the ELF executable file
 */

void sigFaultHandler(int signum, siginfo_t *info, void *context)
{


  uintptr_t fault_address = (uintptr_t)info->si_addr;
  int p_vaddr = 0;
  int p_off = 0;
  int noOfPages = 0;
  struct page *pages = NULL;

  //printf("hello\n");
  //Iterating over program headers to find in which segment the fault occured and accordingly fetch the pageTable and other info
  for (int i = 0; i < ehdr->e_phnum; i++)
  {
    if (fault_address >= phdr[i].p_vaddr && fault_address <= phdr[i].p_vaddr + phdr[i].p_memsz)
    {
      p_off = phdr[i].p_offset;
      p_vaddr = phdr[i].p_vaddr;
      noOfPages = table[i].noOfPages;
      pages = table[i].pages;
      //printf("start:%p mem:%d end:%d fault_addr:%p\n",(void*)p_vaddr,phdr[i].p_memsz,(void*)p_vaddr+phdr[i].p_memsz,fault_address);
    }
  }
  if (pages != NULL)
  {
    noOfPageFaults++;
    
    //Finding the index in the page table where fault occured.
    int idx = (fault_address - p_vaddr) / 4096;
   
    //checking if particular page is already loaded or not
    //if not yet loaded,then we will load it.
    
    if (pages[idx].isLoaded == 0)
    {
      noOfPageAllocations++;
      //allocating a memory of page size 4096B or 4kb at the page where seg fault occured
      void *virtual_mem = mmap((void *)(p_vaddr + idx * 4096), 4096, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      //finding the start index of that table;
      int startl = (p_vaddr + idx * 4096);
      

      if (virtual_mem == MAP_FAILED)
      {
        printf("I am NULL");
      }

      //calculating the offset
      int addr = 4096 * idx;

      if (lseek(fd, p_off + addr, SEEK_SET) == -1)
      {
        perror("Error in segment data");
        close(fd);

        return;
      }
      
      //Read and copy the content from the ELF file to the newly allocated page
      int val = read(fd, virtual_mem, pages[idx].memsz);
      internalfragments += (4096 - pages[idx].memsz) / (float)1024;
      if (val == -1)
      {
        close(fd);
        return;
      }
      //marking that the page is loaded
      pages[idx].isLoaded = 1;
      //printf("\n");
      return;
    }
  }
  printf("other type of Segment fault");
}
void load_and_run_elf(char *exe)
{
  //checking if the file is executable or not
  if (access(exe, X_OK))
  {
    printf("The file is not executable.\n");
    return;
  }
  //opening the input file
  fd = open(exe, O_RDONLY);
  Elf32_Ehdr elf_h;
  ehdr = &elf_h;
  //reading elf header
  int val = read(fd, &elf_h, sizeof(Elf32_Ehdr));
  if (val == -1)
  {
    printf("Error in reading file\n");
    close(fd);
    return;
  }
  //To traverse program header.
  phdr = (Elf32_Phdr *)malloc(elf_h.e_phnum * sizeof(Elf32_Phdr));

  val = read(fd, phdr, elf_h.e_phnum * sizeof(Elf32_Phdr));
  if (val == -1)
  {
    printf("Error in reading programme headers \n");
    close(fd);
    return;
  }
  int totalPages=0;
  //creating pages by dividing each segment into 4kb chunks
  table = (struct pageTable *)malloc(sizeof(struct pageTable) * elf_h.e_phnum);
  for (int i = 0; i < elf_h.e_phnum; i++)
  {
    

      int p_vaddr = phdr[i].p_vaddr;
      int p_off = phdr[i].p_offset;
      
      //Calculating max no of pages needed for a segment 
      int noOfPages = (phdr[i].p_memsz / 4096);
      noOfPages += (phdr[i].p_memsz % 4096 == 0 ? 0 : 1);
      struct page *pages = (struct page *)mmap(NULL, sizeof(struct page) * noOfPages, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
      
      for (int j = 0; j < noOfPages; j++)
      {
        pages[j].isLoaded = 0;
        int idx = j * 4096;

        if (4096 > phdr[i].p_memsz - idx)
        {
          pages[j].memsz = phdr[i].p_memsz - idx;
        }
        else
        {
          pages[j].memsz = 4096;
        }
      }
      table[i].noOfPages = noOfPages;
      table[i].pages = pages; 
      totalPages+=noOfPages;
      //printf("%d: %d\n",i,noOfPages);
  }
  //printf("totalPages: %d\n",totalPages);
  //To handle sigfault
  struct sigaction sa;
  //To give more info like addr where page fault occured
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = sigFaultHandler;
  sigaction(SIGSEGV, &sa, NULL);
  //getting the entry poinmt
  start _start = (start)(ehdr->e_entry);
  
  printf("\n\n#####Output######\n\n");
  printf("User _start return value = %d\n", _start());
  printf("pagefaults: %d \nno Of PageAllocation: %d\ninternal fragments(in kb): %0.2f\n", noOfPageFaults,noOfPageAllocations ,internalfragments);
}

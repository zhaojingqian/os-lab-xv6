// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct kmem {
  struct spinlock lock;
  struct run *freelist;
};

// each cpu have a list
struct kmem kmems[NCPU];

void
kinit()
{
  // initlock(&kmem.lock, "kmem");
  // freerange(end, (void*)PHYSTOP);
  
  //each cpu get same range list

  for(int i=0; i<NCPU; i++) {
    // char name[7] = "kmem";
    // name[5] +=  i
    initlock(&(kmems[i].lock), "kmem");
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  // acquire(&kmem.lock);
  // r->next = kmem.freelist;
  // kmem.freelist = r;
  // release(&kmem.lock);

  //get cpuid now
  push_off();
  int myid = cpuid();
  pop_off();

  acquire(&(kmems[myid].lock));
  r->next = kmems[myid].freelist;
  kmems[myid].freelist = r;
  release(&(kmems[myid].lock));
  
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  // acquire(&kmem.lock);
  // r = kmem.freelist;
  // if(r)
  //   kmem.freelist = r->next;
  // release(&kmem.lock);

  push_off();
  int myid = cpuid();
  pop_off();

  acquire(&(kmems[myid].lock));
  r = kmems[myid].freelist;
  if(r) {
    kmems[myid].freelist = r->next;
    release(&(kmems[myid].lock));
    memset((char*)r, 5, PGSIZE);
    return (void*)r;
  }
  else {
    release(&(kmems[myid].lock));
    for(int i=0; i<NCPU; i++) {
      if(myid == i) continue;
      acquire(&(kmems[i].lock));
      r = kmems[i].freelist;
      if(r) {
        kmems[i].freelist = r->next;
        release(&(kmems[i].lock));
        memset((char*)r, 5, PGSIZE);
        return (void*)r;
      }
      release(&(kmems[i].lock));
    }
  }
  return (void*)r;
}

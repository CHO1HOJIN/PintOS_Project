#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "process.h"
#include "devices/input.h"

static void syscall_handler (struct intr_frame *);
struct lock filesys_lock;
void
syscall_init (void) 
{
  // lock_init(&filesys_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void check_user_vaddr(const void *vaddr) {
  if (!is_user_vaddr(vaddr)) Exit(-1);
  if (!pagedir_get_page(thread_current()->pagedir, vaddr)) Exit(-1);
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  // f->esp points to # of syscall
  // printf("syscall_number: %d\n", *(uint32_t *)(f->esp));
  switch (*(uint32_t *)(f->esp)) {
    case SYS_HALT:
      Halt();
      break;
    case SYS_EXIT:
      check_user_vaddr(f->esp + 4);
      Exit(*(uint32_t *)(f->esp + 4));
      break;
    case SYS_EXEC:
      check_user_vaddr(f->esp + 4);
      f->eax = Exec((const char *)*(uint32_t *)(f->esp + 4));
      break;
    case SYS_WAIT:
      check_user_vaddr(f->esp + 4);
      f->eax = Wait(*(uint32_t *)(f->esp + 4));
      break;
    case SYS_READ:
      check_user_vaddr(f->esp + 4);
      Read(*(uint32_t *)(f->esp + 4), *(uint32_t *)(f->esp + 8), *(uint32_t *)(f->esp + 12));
      break;
    case SYS_WRITE:
      f->eax = Write(*(uint32_t *)(f->esp + 4), *(uint32_t *)(f->esp + 8), *(uint32_t *)(f->esp + 12));
      break;
    case SYS_FIBONACCI:
      f->eax = fibonacci(*(uint32_t *)(f->esp + 4));
      break;
    case SYS_MAX_OF_FOUR_INT:
      f->eax = max_of_four_int(*(uint32_t *)(f->esp + 4), *(uint32_t *)(f->esp + 8), *(uint32_t *)(f->esp + 12), *(uint32_t *)(f->esp + 16));
      break;
    default:
      break;
  }
  // printf ("system call!\n");
  // thread_exit ();
}

/// 3.3.4 System Calls (p.29)

/// 1) halt: terminates pintos by calling shutdown_power_off()
void Halt(){
  shutdown_power_off();
}

/// 2) exit: terminates the current user program, returning status to the kernel
/// success: 0, error: nonzero
/// Refer to 3.3.2,  print exit message "process_name: exit(status)"
int Exit(int status){
  printf("%s: exit(%d)\n", thread_name(), status);
  thread_current()->exit_status = status;
  thread_exit();
}

/// 3) exec: runs the executable whose name is given in cmd_line
/// return pid or -1 if the program cannot load or run for any reason
int Exec(const char *cmd_line){
  return process_execute(cmd_line);
}

/// 4) wait: waits for a child process pid and retrieves the child's exit status
/// return the status that was passed to exit
int Wait (int pid){
  return process_wait(pid);
}

/// 5) read: reads size bytes from the file open as fd into buffer
/// return: # of bytes actually read (STDIN = 0), -1 if error
int Read (int fd, void *buffer, unsigned size){
  //fd = 0: reads from keyboard using input_getc()
  unsigned i;
  if (fd == 0) {
    for (i = 0; i < size; i++){
      ((uint8_t *)buffer)[i] = input_getc();
      if (((char *)buffer)[i] == '\0') break;
    }
    return i;
  }
  return -1;
}

/// 6) write: writes size bytes from buffer to the open file fd
/// return # of bytes actually written, -1 if error
int Write (int fd, const void *buffer, unsigned size){
  //fd = 1: writes to the console
  if (fd == 1) {
    putbuf(buffer, size);
    return size;
  }
  return -1;
}

int fibonacci(int n) {
  if (n == 0) return 0;
  if (n == 1) return 1;
  int f1, f2, f3, i;

  f1 = 0; 
  f2 = 1;
  for (i = 1; i < n; i++) {
    f3 = f1 + f2;
    f1 = f2;
    f2 = f3;
  }
  return f3;
}

int max_of_four_int(int a, int b, int c, int d) {
  int M = a;
  if (b > M) M = b;
  if (c > M) M = c;
  if (d > M) M = d;
  return M;
}

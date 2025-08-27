#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
void check_user_vaddr(const void *vaddr);
void Halt (void);
int Exit (int status);
int Exec (const char *cmd_line);
int Wait (int pid);
int Read (int fd, void *buffer, unsigned size);
int Write (int fd, const void *buffer, unsigned size);
int fibonacci (int n);
int max_of_four_int (int a, int b, int c, int d);

#endif /* userprog/syscall.h */

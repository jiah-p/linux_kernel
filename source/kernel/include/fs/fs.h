#ifndef __FS_H__
#define __FS_H__

int sys_open(const char * name, int flags, ...);

int sys_read(int file, char * ptr, int len);

int sys_wirte(int file, char * ptr, int len);

int sys_lseek(int file, int ptr, int off);

int sys_close(int file);

#endif
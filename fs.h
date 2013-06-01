/*
 * =====================================================================================
 *
 *       Filename:  fs.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/31/2013 11:22:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

#include <klee/klee.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#define __OS_VERIFY_assert(x) klee_assert(x)
#define __OS_VERIFY_assume(x) klee_assume(x)

//#define DEFINE_OS_NONDET(X) \
//	X __OS_VERIFIER_nondet_##X() { X val; return val; }

//DEFINE_OS_NONDET(int)
#define MAX_LENGTH 16

#define DEFINE_MAP_TYPE_N(name, type, n)\
		struct __##name{         \
			int valid;   \
			type slot;   \
		};\
		typedef struct name##_t{\
			struct __##name slots[n]; \
			int size; \
			int refcount; \
		} name;
#define DEFINE_MAP_TYPE(name, type) DEFINE_MAP_TYPE_N(name, type, MAX_LENGTH)
#define MAP_IS_VALID(v, k)   ((v).slots[k].valid)
#define MAP_DELETE(v, k) ((v).slots[k].valid = 0)
#define MAP_GET(v, k) ((v).slots[k].slot)
#define MAP_SET(v, k, _v) do{(v).slots[k].slot = _v; (v).slots[k].valid = 1;}while(0)

struct FDdata{
	int inum;
	int off;
};
DEFINE_MAP_TYPE(FDMap, struct FDdata)

struct VMA{
	int perm;
	unsigned int anon_data;
};
DEFINE_MAP_TYPE(SVaMap, struct VMA)

struct proc{
	FDMap *fd_map;
	SVaMap *va_map;
};

struct inode{
	unsigned int nlink;
	unsigned int size;
	unsigned int data;
};

DEFINE_MAP_TYPE_N(ProcMap, struct proc, 3)
DEFINE_MAP_TYPE(InodeMap, struct inode)

ProcMap procs;

#define DEFINE_MAKE_SYM(type) \
static type *make_sym_##type(const char *prefix){              \
	static int __c = 0;                                    \
	static char namebuf[128];                              \
	sprintf(namebuf, "%s_" #type "_%d", prefix, __c++);    \
	type *p = (type *)malloc(sizeof(FDMap));               \
	klee_make_symbolic(p, sizeof(type), namebuf);          \
	return p;                                              \
}

static unsigned __sym_uint32(const char *prefix) {
	unsigned x;
	static int __c = 1;                              
	static char namebuf[128];
	sprintf(namebuf, "%s_uint_%d", prefix, __c++); 
	klee_make_symbolic(&x, sizeof x, namebuf);
	return x;
}


DEFINE_MAKE_SYM(FDMap)
DEFINE_MAKE_SYM(SVaMap)

int sys_clone(int ppid, int share_vm, int share_fd)
{
	int ret_pid;
	klee_assume(MAP_IS_VALID(procs, ppid));

	struct proc *pp = &MAP_GET(procs, ppid);
	struct proc p;

	if(share_vm){
		p.va_map = pp->va_map;
		p.va_map->refcount++;
	}else{
		SVaMap *va_map = make_sym_SVaMap("sys_clone");
		memcpy(va_map, pp->va_map, sizeof(SVaMap));
		va_map->refcount = 1;
		p.va_map = va_map;
	}

	if(share_fd){
		p.fd_map = pp->fd_map;
		p.fd_map->refcount++;
	}else{
		FDMap *fdmap = make_sym_FDMap("sys_clone");
		memcpy(fdmap, pp->fd_map, sizeof(FDMap));
		fdmap->refcount = 1;
		p.fd_map = fdmap;
	}

	ret_pid = __sym_uint32("sys_clone_ret_pid");
	klee_assume(ret_pid < 3);
	klee_assume(ret_pid >= 0);
	klee_assume(!MAP_IS_VALID(procs, ret_pid));
	MAP_SET(procs, ret_pid, p);
	/* return to child pid to parent */
	return ret_pid;
}

void sys_exit(int pid)
{
	klee_assume(MAP_IS_VALID(procs, pid));
	/* idle never exit */
	klee_assert(pid!=0);
	struct proc *p = &MAP_GET(procs, pid);

	p->fd_map->refcount --;
	if(p->fd_map->refcount == 0)
		free(p->fd_map);
	p->va_map->refcount --;
	if(p->va_map->refcount == 0)
		free(p->va_map);

	MAP_DELETE(procs, pid);
}

int sys_mmap(int pid, int perm, int flags, int va)
{
	klee_assume(MAP_IS_VALID(procs, pid));
	klee_assume(va >= 0);
	klee_assume(va < MAX_LENGTH);
	struct proc *p = &MAP_GET(procs, pid);
	if(!(flags & MAP_FIXED)){
		va = __sym_uint32("sys_mmap_va");
		klee_assume(!MAP_IS_VALID(*(p->va_map), va));
	}
	struct VMA vma;
	vma.perm = perm;
	vma.anon_data =  __sym_uint32("sys_mmap_anon");
	MAP_SET(*(p->va_map), va, vma);
	return va;
}

int sys_munmap(int pid, int va)
{
	klee_assume(MAP_IS_VALID(procs, pid));
	klee_assume(va >= 0);
	klee_assume(va < MAX_LENGTH);
	struct proc *p = &MAP_GET(procs, pid);
	if(!MAP_IS_VALID(*(p->va_map), va))
		return -ENOMEM;
	MAP_DELETE(*(p->va_map), va);
	return 0;
}

void __init_os()
{
	FDMap *fdmap0 = make_sym_FDMap("init");
	SVaMap *vamap0 = make_sym_SVaMap("init");
	klee_make_symbolic(&procs, sizeof(procs), "procs");

	procs.slots[0].valid = 1;
	struct proc *p0 = &procs.slots[0].slot;
	p0->va_map = vamap0;
	p0->fd_map = fdmap0;

	memset(p0->va_map, 0, sizeof(SVaMap));
	memset(p0->fd_map, 0, sizeof(FDMap));

	vamap0->refcount = 1;
	fdmap0->refcount = 1;

	MAP_DELETE(procs, 1);
	MAP_DELETE(procs, 2);
}

void __push_state(int i)
{
}

void __pop_state(int i)
{
}


#if 0
int main()
{
	__init_os();
	int ret = sys_clone(0, 1, 0);
	
	klee_assert(ret > 0);
	klee_assert(MAP_GET(procs, ret).fd_map != MAP_GET(procs, 0).fd_map);
	klee_assert(MAP_GET(procs, ret).va_map == MAP_GET(procs, 0).va_map);

	sys_exit(ret);
	klee_assert(!MAP_IS_VALID(procs, ret));
}
#endif


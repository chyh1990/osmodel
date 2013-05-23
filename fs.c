/*
 * =====================================================================================
 *
 *       Filename:  fs.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/20/2013 02:04:19 PM
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

#define __OS_VERIFY_assert(x) klee_assert(x)
#define __OS_VERIFY_assume(x) klee_assume(x)

#define DEFINE_OS_NONDET(X) \
	X __OS_VERIFIER_nondet_##X() { X val; return val; }

DEFINE_OS_NONDET(int)



#define MAX_LENGTH 32
#define DEFINE_ASSOC_LIST(n, t1, t2) \
	struct assoc_list_elem_##n{ \
		t1 k; \
		t2 v; \
	}; \
	struct assoc_list_##n{ \
		int len;   \
		struct assoc_list_elem_##n d[MAX_LENGTH];\
	};\
	struct assoc_list_elem_##n *find_in_##n(struct assoc_list_##n *l, t1 k){int i;for(i=0;i<l->len;i++) if(l->d[i].k == k) return &l->d[i]; return NULL;} \
	struct assoc_list_elem_##n* add_in_##n(struct assoc_list_##n *l, t1 k, t2 v){struct assoc_list_elem_##n *p = find_in_##n(l, k); \
		if(p) p->v = v; \
		else{p = &l->d[l->len++]; p->k = k;p->v = v;}\
		return p;}\
	void init_##n(struct assoc_list_##n *l){l->len = 0;}\
	int get_index_##n(struct assoc_list_##n *l, struct assoc_list_elem_##n *p){return p - &l->d[0];}
DEFINE_ASSOC_LIST(i2i, int, int)

struct FDdata{
	int inum;
	int off;
};
DEFINE_ASSOC_LIST(i2fddata, int, struct FDdata)

typedef struct assoc_list_i2fddata FDMap;
typedef struct assoc_list_i2i SVaMap;

struct proc{
	FDMap fd_map;
	SVaMap va_map;
};

struct inode{
	unsigned int nlink;
	unsigned int size;
#define MAX_DATA_LENGTH 32
	char data[MAX_DATA_LENGTH];
};
DEFINE_ASSOC_LIST(i2proc, int, struct proc)
DEFINE_ASSOC_LIST(i2inode, int, struct inode)

typedef struct assoc_list_i2proc ProcMap;
typedef struct assoc_list_i2inode InodeMap;

struct os_system{
	//int time;
	ProcMap procs;
	InodeMap inodes;

	//fs
};

static inline void init_proc(struct proc *p){
	init_i2fddata(&p->fd_map);
	init_i2i(&p->va_map);
}

static struct os_system sys;

int sys_open(int fn, int oflag, int _pid)
{
	struct inode *inode;
	struct assoc_list_elem_i2inode *e = find_in_i2inode(&sys.inodes, fn);
	int inum = fn;
	if(oflag & O_CREAT){
		if(!e){
			struct inode _inode;
			_inode.nlink = 1;
			_inode.size = 0;
			e = add_in_i2inode(&sys.inodes, fn, _inode);
		}else{
			if(oflag & O_EXCL)
				return -EEXIST;
		}
	}
	if(!e)
		return -ENOENT;
	inode = &e->v;
	if(oflag & O_TRUNC){
		if(!(oflag & O_CREAT)){

		}
		inode->size = 0;
	}
	struct FDdata fd_data;
	//XXX
	fd_data.inum = fn;
	fd_data.off = 0;
	FDMap *fdmap = &(find_in_i2proc(&sys.procs, _pid)->v.fd_map);
	struct assoc_list_elem_i2fddata *r = add_in_i2fddata(fdmap, fdmap->len, fd_data);
	return get_index_i2fddata(fdmap, r);
}

int sys_read()
{

}

static void init_os(){
	init_i2proc(&sys.procs);
	init_i2inode(&sys.inodes);
	struct proc idle;
	init_proc(&idle);
	add_in_i2proc(&sys.procs, 0, idle);
	__OS_VERIFY_assert(sys.procs.len == 1);
}

int main()
{
	int fn, oflags;
	int fn1, oflags1;
	klee_make_symbolic(&sys, sizeof(struct os_system), "sys");
	klee_make_symbolic(&fn, sizeof(fn), "fn");
	klee_make_symbolic(&oflags, sizeof(oflags), "oflags");
	klee_make_symbolic(&fn, sizeof(fn), "fn1");
	klee_make_symbolic(&oflags, sizeof(oflags), "oflags1");
	init_os();
	__OS_VERIFY_assume(fn < MAX_LENGTH);
	__OS_VERIFY_assume(fn1 < MAX_LENGTH);
	int ret = sys_open(fn, oflags, 0);
	int ret1 = sys_open(fn1, oflags1, 0);

	return 0;
}


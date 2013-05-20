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

static inline void __OS_VERIFY_assume(int expression) { 
	if (!expression) { ASSUME_LOOP: goto ASSUME_LOOP; } 
	return; 
}

static inline void __OS_VERIFY_assert(int expr){
	if(!expr) { ERROR: goto ERROR; }
	return;
}

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
	static inline int find_in_##n(struct assoc_list_##n *l, t1 k){\
		int i;\
		for(i=0;i<l->len;i++){if(l->d[i].k == k) return i;} \
		return -1;}\
	static inline void add_in_##n(struct assoc_list_##n *l, t1 k, t2 v){\
		int p = find_in_##n(l,k); \
		if(p == -1) {p = l->len;l->len++;} \
		l->d[p].k = k;l->d[p].v = v;}

DEFINE_ASSOC_LIST(i2i, int, int)

typedef struct assoc_list_elem_i2i FDMap;
typedef struct assoc_list_elem_i2i SVaMap;

struct proc{
	FDMap fd_map;
	SVaMap va_map;
};

struct inode{
	int nlink;
	int size;
#define MAX_DATA_LENGTH
	char data[MAX_DATA_LENGTH];
};
DEFINE_ASSOC_LIST(i2proc, int, struct proc)
DEFINE_ASSOC_LIST(i2inode, int, struct inode)

typedef struct assoc_list_elem_i2proc ProcMap;
typedef struct assoc_list_elem_i2inode InodeMap;

struct os_system{
	//int time;
	ProcMap procs;
	InodeMap inodes;
};

int main()
{
	return 0;
}


/*
 * =====================================================================================
 *
 *       Filename:  array.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/20/2013 07:59:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

#include <klee/klee.h>
int a[2];
int foo(){
	a[1] = 3;
}
int main()
{
	klee_make_symbolic(a, sizeof a, "a");
	foo();
	if(a[1] != 4)
		goto ERROR;
	return 0;
ERROR:
	klee_assert(0);
	return -1;
}

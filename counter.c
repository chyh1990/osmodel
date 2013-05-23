/*
 * =====================================================================================
 *
 *       Filename:  counter.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/22/2013 02:10:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

#include <klee/klee.h>
struct state{
	int c;
};
void counter_add(struct state *counter)
{
	counter->c ++;
}
void counter_sub(struct state *counter)
{
	if(counter->c>0)
		counter->c --;
}
int counter_iszero(struct state *counter)
{
	return (counter->c == 0);
}

int main(){
	struct state c1, c2, c3;
	klee_make_symbolic(&c1, sizeof(c1), "c1");
	klee_assume(c1.c >= 0);

	c2 = c1;
	counter_add(&c2);
	counter_sub(&c2);

	c3 = c1;
	counter_sub(&c3);
	counter_add(&c3);

	/* ERROR if not commutable */
	klee_assert(c2.c == c3.c);

	return 0;
}


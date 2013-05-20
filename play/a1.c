/*
 * =====================================================================================
 *
 *       Filename:  a1.c
 *
 *    Description:  i
 *
 *        Version:  1.0
 *        Created:  05/18/2013 12:01:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

static inline void error(){
ERROR:
	goto ERROR;
}

int main(int argc, char *argv[])
{
	if(argc < 2)
		error();
	int s = 1;
	if(sizeof(long) != 4)
		error();
	return 0;
}


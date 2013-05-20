/*
 * =====================================================================================
 *
 *       Filename:  ptr.c
 *
 *    Description: 
 *
 *        Version:  1.0
 *        Created:  05/20/2013 03:14:46 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */
int a = 3;

int main(){
	int *p = &a;
	*p = 5;
	if(a != 5)
		goto ERROR;
	return 0;
ERROR:
	return -1;
}

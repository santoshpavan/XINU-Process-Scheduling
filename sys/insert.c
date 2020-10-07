/* insert.c  -  insert */

#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <lab1.h>

#include <proc.h>

/*------------------------------------------------------------------------
 * insert.c  --  insert an process into a q list in key order
 *------------------------------------------------------------------------
 */
int insert(int proc, int head, int key)
{
	int	next;			/* runs through list		*/
	int	prev;

	next = q[head].qnext;
	//kprintf("pid:%d", proc);
	if (getschedclass() == EXPDISTSCHED) {
		// for RoundRobin for same key
		while (q[next].qkey <= key)	/* tail has maxint as key	*/
			next = q[next].qnext;
		q[proc].qnext = next;
		q[proc].qprev = prev = q[next].qprev;
		q[proc].qkey  = key;
	}/*
	else if (schedclass == LINUXSCHED) {
		int gval = proctab[proc].goodnessvalue;
		while (q[next].qkey < gval)
			next = q[next].qnext;	
		q[proc].qnext = next;
		q[proc].qprev = prev = q[next].qprev;
		q[proc].qkey  = gval;
	}*/
	else {
		// for LINUX and default schedulers
		//kprintf("key(%d):%d - %d\n", proc, key, proctab[key].goodnessvalue);
		while (q[next].qkey < key)	/* tail has maxint as key	*/
			next = q[next].qnext;	
		q[proc].qnext = next;
		q[proc].qprev = prev = q[next].qprev;
		q[proc].qkey  = key;
	}
	q[prev].qnext = proc;
	q[next].qprev = proc;
	return(OK);
}

/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <math.h>
#include <lab1.h>
#include <stdio.h>

#define LAMBDA 0.1

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
int getexpdevid();
void resetepoch();
int getlinuxnextproc();

/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */ 
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	
	int nextprocid;
	optr = &proctab[currpid];
	 
  	if (getschedclass() == EXPDISTSCHED) {
    		/* force context switch */
  		if (optr->pstate == PRCURR) {
  			optr->pstate = PRREADY;
  			insert(currpid,rdyhead,optr->pprio);
  		}
    
    		nextprocid = getexpdevid();
  		/* remove highest priority process at end of ready list */
  		dequeue(nextprocid);
  		nptr = &proctab[ (currpid = nextprocid) ];
  		nptr->pstate = PRCURR;		/* mark it currently running	*/
   	
		#ifdef	RTCLOCK
    			preempt = QUANTUM;		/* reset preemption counter	*/
    		#endif
	}
	else if (getschedclass() == LINUXSCHED) {
		optr->counter -= (prevpreempt - preempt);
		
		if (optr->counter <= 0) {
			optr->counter = 0;
			optr->goodnessvalue = 0;
		}
		else
			optr->goodnessvalue -= (prevpreempt - preempt); //ignoring changes in priority
		
		// checking if current process is yielding
		if (optr->pstate != PRCURR) {
			// check for new process in q
			if (q[rdyhead].qnext == rdytail) {
				nextprocid = NULLPROC;
				resetepoch();
			}
			else {
				// context switch
				nextprocid = getlinuxnextproc();
			}
		}
		else {
			if (optr->goodnessvalue == 0) {
				  // context switch -- making it yield
				  nextprocid = getlinuxnextproc();	
  				optr->pstate = PRREADY;
  				insert(currpid,rdyhead,optr->goodnessvalue);
			}
			else {
				// continue current process
				#ifdef RTCLOCK
					preempt = proctab[currpid].counter;
					prevpreempt = preempt;
				#endif
				return(OK);
			}
		}
  		/* remove highest priority process at end of ready list */
  		dequeue(nextprocid);
  		nptr = &proctab[ (currpid = nextprocid) ];
  		nptr->pstate = PRCURR;		/* mark it currently running	*/
		  #ifdef	RTCLOCK
    			preempt = proctab[currpid].counter;	/* reset preemption counter */
			    prevpreempt = preempt;
 		  #endif
	}
  	else {
       // default scheduler
 		  if ( ( optr->pstate == PRCURR) && (lastkey(rdytail)<optr->pprio)) {
			  return(OK);
   		}
  	
  		/* force context switch */
  		if (optr->pstate == PRCURR) {
  			optr->pstate = PRREADY;
  			insert(currpid,rdyhead,optr->pprio);
  		}
  
  		/* remove highest priority process at end of ready list */
  		nptr = &proctab[ (currpid = getlast(rdytail)) ];
  		nptr->pstate = PRCURR;		/* mark it currently running	*/

		  #ifdef	RTCLOCK
    			preempt = QUANTUM;		/* reset preemption counter	*/
  		#endif
	  }	
   
  	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
  	/* The OLD process returns here when resumed. */
  	return OK;
}

int getexpdevid() {
	int randomval = (int)expdev(LAMBDA);
	int nextprocid = rdyhead;
	do {
		nextprocid = q[nextprocid].qnext;
	} while (nextprocid != q[rdytail].qprev && randomval >= q[nextprocid].qkey);
  
	if (nextprocid == rdytail) {
		return NULLPROC;
	}
	return nextprocid;
}

void resetepoch() {
	// assign quantum values to all the processes
	int i = 0;
	for (; i < NPROC; i++) {
		if (proctab[i].pstate != PRFREE) {
			if (proctab[i].counter == proctab[i].timequantum) { 
				// untouched
				proctab[i].counter = proctab[i].pprio;
				proctab[i].goodnessvalue = proctab[i].counter * 2;
			}
			else if (proctab[i].counter != 0) {
				// unused
				proctab[i].counter = (proctab[i].counter/2) + proctab[i].pprio;
				proctab[i].goodnessvalue = proctab[i].counter + proctab[i].pprio;
			}
			else {
				//used
				proctab[i].counter = proctab[i].timequantum;
				proctab[i].goodnessvalue = proctab[i].counter + proctab[i].pprio;
			}
			proctab[i].birthstatus = THISPROC;
		}
	}
	// resetting the ready q
	int ptr = q[rdyhead].qnext;
	while (ptr != rdytail) {
		int ptrnext = q[ptr].qnext;
		if (q[ptr].qkey == 0 && ptr != NULLPROC) {
			dequeue(ptr);
			insert(ptr, rdyhead, proctab[ptr].goodnessvalue);
		}
		ptr = ptrnext;
	}
}

int getlinuxnextproc() {
	// traverse q from last to first to find proc with goodnessvalue!=0
	int nextprocid = q[rdytail].qprev;
	while (nextprocid != rdyhead && (proctab[nextprocid].counter == 0 || proctab[nextprocid].birthstatus == NEXTPROC)) {
		nextprocid = q[nextprocid].qprev;
	}
	if (nextprocid == rdyhead) {
		// no viable processes
		nextprocid = NULLPROC;
		resetepoch();
	}
	return nextprocid;
}

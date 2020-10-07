/* lab1.h - log, pow, expdev */

#ifndef _LAB1_H_
#define _LAB1_H_

// Scheduling constants
#define EXPDISTSCHED 1
#define LINUXSCHED 2

/* Implementation in schedclass.c */

//extern int schedclass;

void setschedclass (int sched_class);
int getschedclass();

#endif

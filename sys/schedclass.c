#include <lab1.h>
#include <stdio.h>

int schedclass = 0;

void setschedclass (int sched_class) {
  schedclass = sched_class;
}

int getschedclass() {
  return schedclass;
}
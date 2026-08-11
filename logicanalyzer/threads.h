#ifndef LA_THREADS
#define LA_THREADS

// threads.c
void *LAReadThread(void *vlaw);
//void *LAWindowUpdateLoop(void *vlaw);
void LAWindowUpdateLoop(LAWindow *law);
int LAWindowUpdateLoopConnector(void *vlaw);

#endif //LA_THREADS

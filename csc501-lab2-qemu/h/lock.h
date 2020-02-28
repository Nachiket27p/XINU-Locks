/* lock.h - READ, WRITE, LFREE, LUSED */
// lab2 header containing definitions for read/write semaphores

// DELETED is already defined in kernel.h
#define READ '\11'
#define WRITE '\12'
#define LFREE '\13'
#define LUSED '\14'
#define LNOUSE 0

#define NLOCKS 50

struct lentry {
    char lState;
    int lReaders;
    int lWriters;
    int rQHead;
    int rQTail;
    int wQHead;
    int wQTail;
};

extern struct lentry locks[];
extern int nextLock;
extern unsigned long ctr1000;

#define BADLOCK(ls) (ls < 0 || ls >= NLOCKS);

void linit();
int lcreate();
int ldelete(int lock);
int lock(int lock, int type, int priority);
int releaseall(int numlocks, ...);

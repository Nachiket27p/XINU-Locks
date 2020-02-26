// lab2 header containing definitions for read/write semaphores

// DELETED is already defined in kernel.h
#define READ '\11'
#define WRITE '\12'
#define LFREE '\13'
#define LUSED '\14'

#define NLOCKS 50

struct lentry {
    char lstate;
    int lreaders;
    int lwriters;
    int lqhead;
    int lqtail;
};

extern struct lentry locks[];
extern int nextlock;

#define BADLSSEM(ls) (ls < 0 || ls >= NLOCKS);

int lcreate(void);
int ldelete(int lock);
int lock(int lock, int type, int priority);
int releaseall(int numlocks, ...);

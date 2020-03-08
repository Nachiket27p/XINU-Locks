sl
ls
tar xzvf csc501-lab2-qemu.tgz 
git inti
git init
git remote add https://github.com/Nachiket27p/CSC501-Lab2.git
git remote add origin https://github.com/Nachiket27p/CSC501-Lab2.git
git add -A
git commmit -m "initial commit"
git commit -m "initial commit"
git config --global user.email "nachiket27p@gmail.com"
git config --global user.name "Nachiket27p"
git commit -m "initial commit"
git push origin master
touch .gitignore
vi .gitignore 
exit
git add -A
git push origin master
exit
ls
cd csc501-lab2-qemu
cd sys/
grep --help
grep * sysinit
grep sysinit *
grep nextsem *
cd ..
cd h/
grep nextsem *
grep screate *
cd ..
cd sys/
grep screate *
cd csc501-lab2-qemu
cd h/
grep deleted *
grep DELETED *
grep READ *
grep locks *
cd ..
cd sys/
grep locks *
cd
git add -A
git commit -m "created and implemented lock.h updated initialize.c"
git push origin master
cd
git add -A
git commit -m "moved initialization code from initialize.c to linit.c"
git push origin master
cd csc501-lab2-qemu/h/
grep Bool *
grep noneempty *
ls
cd ..
cd sys/
grep nonempty *
cd ..
cd h/
grep nonempty *
cd ..
cd sys/
grep getlast *
grep TRUE *
cd ..
cd h/
grep TRUE *
grep FALSE *
cd ..
cd sys/
grep RESCHNO *
cd ..
cd h/
grep RESCHNO *
grep semcnt *
cd ..
cd sys/
grep semcnt *
grep sreset *
cd ..
cd h/
grep sreset *
cd ..
cd compile/
grep sreset *
cd ..
cd sys/
grep wait *
CD
cd
git add -A
git commit -m "Implemented lock.c - still incomplete"
git push origin master
cd
git add -A
git commit -m "started releaseall.c implementation"
git push origin master
cd csc501-lab2-qemu/h/
grep DELETED *
grep FREE *
cd ..
cd sys/
grep FREE *
cd ..
cd h
cd
git add -A
git commit -m "changed lock policy on lock.c"
git push origin master
cd
git add -A
git commit -m "implemented releaseall.c - still need to finalize"
git push origin master
cd csc501-lab2-qemu/h/
grep ctr1000 *
cd ..
cd sys/
grep ctr1000 *
cd..
clear
git add -A
cd
git add -A
git commit -m "partially implemented lock timing structure"
git push origin master
cd csc501-lab2-qemu
cd compile/
make depend
make
make depend
make
make depend
make
make depend && make
make clean
clear
make depend && make && make run
q
make depend && make && make run
q
make depend && make && make run
cd ..
cd sys/
cd ..
cd h/
grep SYSERR *
cd ..
cd compile/
make depend && make && make run
git add -A
git commit -m "XINU OS not creating main process"
git push origin master
cd csc501-lab2-qemu/compile/
make depend && make && make run
cd ..
cd sys/
grep "qent q" *
grep qent *
grep NQENT *
cd ..
cd h/
grep NQENT *
cd ..
cd compile/
make depend && make && make run
cd ..
cd sys/
grep NQENT *
cd ..
cd h/
grep NQENT *
cd ..
cd compile/
make depend && make && make run
git add -A
cd
git add -A
git commit -m "Fixed XINU by increasing q[] size to accomidate extra spots for locks"
git push origin master
cd csc501-lab2-qemu/com
cd ..
cd compile/
make depend && make && make run
make clean
make depend
cd ..
cd h/
grep nextqueue *
cd ..
cd sys/
frep nextqueue *
grep nextqueue *
cd
git add -A
git commit -m "tested variable arguments in releaseall.c- WORKING"
cd csc501-lab2-qemu/compile/
make clean
cd
git commit -m "tested variable arguments in releaseall.c- WORKING"
git push origin master
cd csc501-lab2-qemu/sys/
grep disable *
cd csc501-lab2-qemu/compile/
make depend && make && make run
cd ..
cd lib/
ls
cd libxc
ls
grep pow *
grep math *
cd ..
cd comp
cd ..
cd compile/
make depend && make && make run
cd ..
cd h/
grep unsigned *
cd ..
cd compile/
make depend && make && make run
cd ..
cd h/
grep ULL *
grep "unsigned long long" *
cd ..
cd csc501-lab2-qemu/compile/
make depend && make && make run
cd
git add -A
git commit -m "declared extra variables in lentry struct and pcb struct for priority inheritance"
git push origin master
cd csc501-lab2-qemu/compile/
make depend && make && make run
cd
git add -A
git commit -m "reimplemented how process keeps track of its locks and how locks track processes"
git push origin master
cd csc501-lab2-qemu/compile/
make depend && make && make run
cd ..
cd sys/
grep ? *
cd ..
cd compile/
make depend && make && make run
make clean
cd
git add -A
git commit -m "fixed compilation error and moved typedef into systypes.h"
git push origin master
cd csc501-lab2-qemu/compile/
make depend && make && make run
exit

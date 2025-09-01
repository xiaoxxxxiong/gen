# gen
Timetabling Software Based on Genetic Algorithm - C based without GUI
You may need a linux system or windows with wsl2, please install a mysql or mariadb (6.0.0 or higher); gcc, mysql.h, pthread.h are also needed.

Follow the steps below, you can run the program:
1.recover cup_bak.sql to a mysql/mariadb instance, the default database name is 'cup'
2.cd sql/tbl, vim dbx.sql, notice the uri in this file, change the path to the actual path on your computer.
3.login mysql, run in db: source 'your dbx.sql path'
4.run in db: truncate table ctn, truncate table sss, truncate table rst
5.run in db: call proc_get(1, 4, @ret, @msg);
6.run in sh: make DBG=O3 CNS=y, it will generate a program with Optimization Level - O3, CNS=y means console app
7.run in sh: ./run_cns
8.open a new terminal B, run in sh: curl -X POST -H "Content-Type: text/plain" -d 'usr:xiong chaowen,pwd:202cb962ac59075b964b07152d234b70,opt:0' http://127.0.0.1:6179
9.back to terminal A, now you can see the program in running

The first column is Population Iteration Count， the second column is the Highest Fitness Score in the Population, 0 is perfect.
use curl -X POST -H "Content-Type: text/plain" -d 'usr:xiong chaowen,pwd:202cb962ac59075b964b07152d234b70,opt:1' http://127.0.0.1:6179 to show the score
you can change settings in table cnf.

Tables:
cld.sql - calendar
cls.sql - class
cnf.sql - configuration
ctn.sql - container
dbx.sql - dabase batch file
inf.sql - basic information
rom.sql - class room
rst.sql - result
rtp.sql - room type
sft.sql - constraint
sss.sql - *
sub.sql - subject
tch.sql - teacher
tsk.sql - task
usr.sql - user
xlg.sql - error log

Procedures:
proc_cld.sql - calendar things
proc_get.sql - get all the data program needed
proc_sch.sql - read result like 'call proc_sch(1, 1, 38, @ret, @msg);'
proc_sft.sql - constraint things
proc_sim.sql - special constraint things

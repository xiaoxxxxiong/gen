/**** create database ****/
use mysql;
create database if not exists ubuntu_work_bak/workplace default charset utf8 collate utf8_general_ci;
use cup;
/**** base table ****/
source /home/aix/ubuntu_work_bak/workplace/sql/tbl/usr.sql;		/* user */
source /home/aix/ubuntu_work_bak/workplace/sql/tbl/xlg.sql;		/* exception log */
source /home/aix/ubuntu_work_bak/workplace/sql/tbl/cld.sql;		/* calendar */
source /home/aix/ubuntu_work_bak/workplace/sql/tbl/cnf.sql;		/* configuration */

source /home/aix/ubuntu_work_bak/workplace/sql/tbl/cls.sql;		/* class */
source /home/aix/ubuntu_work_bak/workplace/sql/tbl/tch.sql;		/* teacher */
source /home/aix/ubuntu_work_bak/workplace/sql/tbl/sub.sql;		/* subject */
source /home/aix/ubuntu_work_bak/workplace/sql/tbl/rtp.sql;		/* room type */
source /home/aix/ubuntu_work_bak/workplace/sql/tbl/rom.sql;		/* room */

source /home/aix/ubuntu_work_bak/workplace/sql/tbl/sss.sql;		/* replace temporary table in procedure proc_get */
source /home/aix/ubuntu_work_bak/workplace/sql/tbl/ctn.sql;		/* container */
source /home/aix/ubuntu_work_bak/workplace/sql/tbl/rst.sql;		/* result */

source /home/aix/ubuntu_work_bak/workplace/sql/tbl/inf.sql;		/* teaching information */
source /home/aix/ubuntu_work_bak/workplace/sql/tbl/sft.sql;		/* soft constraint */

source /home/aix/ubuntu_work_bak/workplace/sql/tbl/tsk.sql;		/* task queue */
/**** procedure ****/
source /home/aix/ubuntu_work_bak/workplace/sql/prc/proc_cld.sql;	/* school calendar */
source /home/aix/ubuntu_work_bak/workplace/sql/prc/proc_get.sql;	/* get configuration & indexes & information */
source /home/aix/ubuntu_work_bak/workplace/sql/prc/proc_sch.sql;	/* get class schedual */
source /home/aix/ubuntu_work_bak/workplace/sql/prc/proc_sft.sql;	/* get soft constraint */
source /home/aix/ubuntu_work_bak/workplace/sql/prc/proc_sim.sql;	/* get simultaneity constraint */

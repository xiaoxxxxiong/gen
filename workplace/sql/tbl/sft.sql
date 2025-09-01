delimiter //
/** soft constraint **/
create table if not exists sft (
	udt int unsigned not null,
	idt smallint unsigned not null,
	/* cts set */
	cls varchar(512) not null default '',	/* cid list - must be ordered by asc */
	tls varchar(512) not null default '',	/* tid list - must be ordered by asc */
	sls varchar(512) not null default '',	/* sid list - must be ordered by asc */
	/* dmb set */
	dmb varchar(1024) not null,		/* time list */
	dtp bit(1) not null,			/* 0:every 1:total  */
	/* compare hit counts of blocks */
	/* 0-3:general 4-7:special */
	opt bit(4) not null,	/* 0:> 1:< 2:= 3:<> 4:> 5:< 6:= 7:<> */
	num tinyint unsigned not null,
	/* constraint weight level */
	lvl tinyint unsigned not null default 4 check(lvl < 5),/* 0 - 4; 0 is bigger */
	/* is stop use */
	stp bit(1) not null default 0,
	primary key (udt, idt)
) //
delimiter ;

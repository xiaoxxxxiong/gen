delimiter //
/** class **/
create table if not exists cls(
	udt int unsigned not null,
	idt tinyint unsigned not null,
	nme varchar(32) not null check(length(nme) >= 6),
	sct tinyint unsigned not null default 60,
	typ bit not null default 0,
	dnu tinyint unsigned not null default 0 check(dnu <= 8),
	mnu tinyint unsigned not null default 0 check(mnu <= 8),
	anu tinyint unsigned not null default 0 check(anu <= 8),
	enu tinyint unsigned not null default 0 check(enu <= 8),
	nnn varchar(512) null,
	primary key(udt, idt)
) //
delimiter ;

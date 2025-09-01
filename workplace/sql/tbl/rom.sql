delimiter //
/** room **/
create table if not exists rom (
	udt int unsigned not null,
	idt tinyint unsigned not null,
	nme varchar(32) not null check(length(nme) >= 6),
	typ tinyint unsigned not null,
	own tinyint unsigned null,
	cap tinyint unsigned not null default 60,
	primary key(udt, idt)
) //
delimiter ;

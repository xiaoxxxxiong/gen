delimiter //
/** subject **/
create table if not exists sub (
	udt int unsigned not null,
	idt tinyint unsigned not null,
	nme varchar(32) not null check(length(nme) >= 6),
	rtp tinyint unsigned null,
	primary key(udt, idt)
) //
delimiter ;

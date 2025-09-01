delimiter //
/** room type **/
create table if not exists rtp (
	udt int unsigned not null,
	idt tinyint unsigned not null,
	nme varchar(32) not null check(length(nme) >= 6),
	primary key(udt, idt)
) //
delimiter ;

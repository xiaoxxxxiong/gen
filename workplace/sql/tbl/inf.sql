delimiter //
create table if not exists inf (
	udt int unsigned not null,
	cid tinyint unsigned not null,
	tid tinyint unsigned not null,
	sid tinyint unsigned not null,
	scr tinyint unsigned not null,
	cmb tinyint unsigned null,
	primary key (udt, cid, tid, sid)
) //
delimiter ;

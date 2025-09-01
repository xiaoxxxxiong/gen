delimiter //
/** calendar **/
create table if not exists cld (
	udt int unsigned not null,
	ydx tinyint unsigned not null,
	mdx tinyint unsigned not null,
	ddx tinyint unsigned not null,
	dow tinyint unsigned not null,
	woy tinyint unsigned not null,
	ifu bit not null default 0,
	primary key (udt, ydx, mdx, ddx)
) //
delimiter ;

delimiter //
/** configuration **/
create table if not exists cnf (
	udt int unsigned not null,
	nme varchar(32) not null,
	val varchar(32) not null,
	dsc varchar(128) null,
	primary key (udt, nme)
) //
delimiter ;

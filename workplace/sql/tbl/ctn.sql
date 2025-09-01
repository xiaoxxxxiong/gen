delimiter //
/** container **/
create table if not exists ctn (
	udt int unsigned not null,
        rnu int unsigned not null,
        cid tinyint unsigned not null,
        tid tinyint unsigned not null,
        sid tinyint unsigned not null,
        rid tinyint unsigned not null,
        day tinyint unsigned not null,
        moa tinyint unsigned not null,
        blk tinyint unsigned not null,
	key (udt)
) //
delimiter ;

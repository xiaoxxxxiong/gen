delimiter //
/** task queue **/
create table if not exists tsk (
	ord int unsigned not null primary key,
	udt int unsigned not null unique,
	stt bit(2) not null	/* 0:ready 1:running 2:finished 3:error */
) //
delimiter ;

delimiter //
/** table sss in procedure proc_get **/
create table if not exists sss (
	udt int unsigned not null,
	/** class id **/
	cid tinyint  unsigned not null,
	/** teacher id **/
	tid tinyint  unsigned not null,
	/** subject id **/
	sid tinyint  unsigned not null,
	/** score **/
	scr smallint unsigned not null,
	/** room count in list **/
	rct tinyint  unsigned not null,
	/** room list **/
	rid varchar(256) not null,
	/** combined class **/
	cmb tinyint  unsigned null
) //
delimiter ;

delimiter //
/** result **/
create table if not exists rst (
	udt int unsigned not null,
	/** unique in one user **/
	idt int unsigned not null,
	/** class id **/
	cid int unsigned not null,
	/** teacher id **/
	tid int unsigned not null,
	/** subject id **/
	sid int unsigned not null,
	/** room id **/
	rid int unsigned not null,

	/** index of week days **/
	day smallint unsigned not null,
	/* dawn or morning or afternoon or evening 1-4 */
	moa tinyint unsigned not null,
	/** index of day blocks **/
	blk tinyint unsigned not null,
	primary key (udt, idt)
) engine = memory //
delimiter ;

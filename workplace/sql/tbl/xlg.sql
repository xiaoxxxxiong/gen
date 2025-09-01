delimiter //
/** exception log **/
create table if not exists xlg (
	udt int unsigned not null,
        /** procedure name **/
        prc varchar(16) not null,
        /** exception content **/
        ctt varchar(512),
        /** date & time **/
        dtt datetime not null default now(),
        key idx_xlg (dtt)
) //
delimiter ;

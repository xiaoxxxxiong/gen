delimiter //
/** user **/
create table if not exists usr (
	idt int unsigned not null primary key auto_increment,
	nme varchar(32) not null check(length(nme) >= 6),
	tel varchar(14) not null check(length(tel) = 11 or length(tel) = 14),
	pwd varchar(32) not null check(length(pwd) = 32),
	rsa varchar(256) not null,	/** rsa-key; note:another key was saved in client **/
	rdm varchar(32) not null,	/** last random from client **/
	dsc varchar(128) null
) //
delimiter ;

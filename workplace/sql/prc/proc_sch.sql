/** clendar **/
delimiter //
drop procedure if exists proc_sch;
create procedure proc_sch (
	in  _udt int unsigned,
	in  _cid smallint unsigned,
	in  _woy tinyint unsigned,	/* week index - the nature week index */
	out _ret bit,
	out _msg varchar(200)
)
_exit_:begin
        declare hasSqlException bit default 0;
        declare exit handler for sqlexception
        begin
                get diagnostics condition 1 _msg = message_text;
                set hasSqlException = 1;
                rollback;
                insert into xlg (udt, prc, ctt) values (_udt, 'proc_sch', _msg);
                set _ret = 1;
                set _msg = 'exception, please contact administrator';
        end;
        start transaction;

        if _udt is null or _udt < 1 then
                rollback;
                set _ret = 1;
                set _msg = 'user id is null';
                leave _exit_;
        elseif not exists (select 1 from usr where idt = _udt limit 1) then
                rollback;
                set _ret = 1;
                set _msg = 'specified user id is not exists in system';
                leave _exit_;
	elseif _cid is null or _cid < 1 then
		rollback;
		set _ret = 1;
		set _msg = 'need class index';
		leave _exit_;
	elseif _woy is null or _woy < 1 then
		rollback;
		set _ret = 1;
		set _msg = 'need week index of year';
		leave _exit_;
	else
	begin
		declare dnu_ tinyint unsigned;
		declare mnu_ tinyint unsigned;
		declare anu_ tinyint unsigned;
		declare enu_ tinyint unsigned;
		declare txt_ varchar(32);
		declare dow_ varchar(32);
		declare moa_ varchar(32);
		declare blk_ tinyint unsigned;
		select dnu, mnu, anu, enu into dnu_, mnu_, anu_, enu_ from cls where udt = _udt and idt = _cid;
		create temporary table sch (
			blk varchar(32)	default '',
			mon varchar(32)	default '',
			tue varchar(32)	default '',
			wed varchar(32)	default '',
			thu varchar(32)	default '',
			fri varchar(32)	default '',
			sat varchar(32)	default '',
			sun varchar(32)	default ''
		) engine = memory default charset = utf8;
		while dnu_ > 0 do
			insert into sch (blk) values (concat('a', dnu_));
			set dnu_ = dnu_ - 1;
		end while;
		while mnu_ > 0 do
			insert into sch (blk) values (concat('b', mnu_));
			set mnu_ = mnu_ - 1;
		end while;
		while anu_ > 0 do
			insert into sch (blk) values (concat('c', anu_));
			set anu_ = anu_ - 1;
		end while;
		while enu_ > 0 do
			insert into sch (blk) values (concat('d', enu_));
			set enu_ = enu_ - 1;
		end while;

		set @day := 0;
		create temporary table tmp (
			select case when a.tid = 0 then '' else concat(lpad(a.tid, 3, ' '),' ', lpad(a.sid, 3, ' '), ' ', lpad(a.rid, 3, ' ')) end txt, b.dow, a.moa, a.blk, 0 flg from
			(select * from rst where udt = _udt and cid = _cid) a inner join
			(select *, @day := @day + 1 day from cld where udt = _udt and conv(ifu,2,10) = 1) b on a.day = b.day
			where b.woy = _woy
		);

		while exists (select 1 from tmp where flg = 0 limit 1) do
			select	(case dow 
				 	when 1 then 'mon'
					when 2 then 'tue'
					when 3 then 'wed'
					when 4 then 'thu'
					when 5 then 'fri'
					when 6 then 'sat'
					else 'sun' end
				) dow, 
				(case moa
					when 1 then 'a'
					when 2 then 'b'
					when 3 then 'c'
					else 'd' end
				) moa, 
				blk, txt
			into dow_, moa_, blk_, txt_
			from tmp where flg = 0 limit 1;
			set @sql = 'update sch set ';
			set @sql = concat(@sql, dow_, ' = ''', txt_, '''');
			set @sql = concat(@sql, ' where blk = ''', moa_, blk_, '''');
			prepare stmt from @sql;
			execute stmt;
			deallocate prepare stmt;
			update tmp set flg = 1 where flg = 0 limit 1;
		end while;
		select * from sch order by blk asc;

		drop table tmp,sch;
	end;
	end if;

	commit;
end //
delimiter ;

/** get configuration & indexes & infomation **/
delimiter //
drop procedure if exists proc_get;
create procedure proc_get (
	in  _udt int unsigned,	/* user id */
	in  _opt bit(4),	/* 0:get bxy */
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
                insert into xlg (udt, prc, ctt) values (_udt, 'proc_get', _msg);
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
	elseif _opt is null then
		rollback;
		set _ret = 1;
		set _msg = 'operation type is null';
		leave _exit_;
	else
		if not exists (select 1 from ctn where udt = _udt limit 1) then 
		begin
		/**************************************************** create ctn if not exists ****************************************************/
		/***** variable *****/
		declare cid_ tinyint unsigned;
		declare tid_ tinyint unsigned;
		declare sid_ tinyint unsigned;
		declare rid_ tinyint unsigned;
		declare scr_ tinyint unsigned;
		/***** step 1 - expand data row by day & moa & blk; not include nnn specified data; fill class owned room *****/
		set @day := 0;
		create temporary table tmp (
			cid tinyint unsigned not null,
			tid tinyint unsigned not null,
			sid tinyint unsigned not null,
			rid tinyint unsigned not null,
			day tinyint unsigned not null,
			moa tinyint unsigned not null,
			blk tinyint unsigned not null
		) engine = memory default charset = utf8;
		insert into tmp (cid, tid, sid, rid, day, moa, blk)
		select m.cid, 0 tid, 0 sid, ifnull(q.rid, 0) rid, o.day, m.moa, n.blk from (
			select idt cid, 1 moa, dnu blk from cls where udt = _udt union select idt cid, 2 moa, mnu blk from cls where udt = _udt union
			select idt cid, 3 moa, anu blk from cls where udt = _udt union select idt cid, 4 moa, enu blk from cls where udt = _udt
		) m inner join (
			select 1 as blk union select 2 as blk union select 3 as blk union select 4 as blk union
			select 5 as blk union select 6 as blk union select 7 as blk union select 8 as blk
		) n on m.blk >= n.blk inner join (
			select @day := @day + 1 day from cld where udt = _udt and conv(ifu,2,10) = 1
		) o on 1 = 1 inner join (
			select idt cid, ifnull(nnn, '') nnn from cls where udt = _udt
		) p on m.cid = p.cid left join (
			select idt rid, own cid from rom where udt = _udt and own is not null
		) q on m.cid = q.cid
		and !find_in_set(concat('d', o.day), p.nnn)
		and !find_in_set(concat('m', m.moa), p.nnn)
		and !find_in_set(concat('b', n.blk), p.nnn)
		and !find_in_set(concat('d', o.day, 'm', m.moa), p.nnn)
		and !find_in_set(concat('d', o.day, 'b', n.blk), p.nnn)
		and !find_in_set(concat('m', m.moa, 'b', n.blk), p.nnn)
		and !find_in_set(concat('d', o.day, 'm', m.moa, 'b', n.blk), p.nnn);
		/***** step 2 - fill tid & sid by inf in tmp *****/
		create temporary table jnf (
			cid tinyint unsigned not null,
			tid tinyint unsigned not null,
			sid tinyint unsigned not null,
			scr tinyint unsigned not null
		) engine = memory default charset = utf8;
		insert into jnf (cid, tid, sid, scr) select cid, tid, sid, scr from inf where udt = _udt;
		/* update tmp, set tid & sid & rid */
		while exists (select 1 from jnf limit 1) do
			select cid, tid, sid, scr into cid_, tid_, sid_, scr_ from jnf limit 1;
			delete from jnf where cid = cid_ and tid = tid_ and sid = sid_ limit 1;
			while scr_ > 0 do
				set scr_ = scr_ - 1;
				update tmp set tid = tid_, sid = sid_ where cid = cid_ and tid = 0 limit 1;
			end while;
		end while;
		drop table jnf;
		update tmp set rid = 0 where tid = 0;
		/***** step 3 - generate row number by specified order; order cid->tid->sid asc *****/
		delete from ctn where udt = _udt;
		set @rnu := -1;
		insert into ctn(rnu, udt, cid, tid, sid, rid, day, moa, blk)
		select @rnu := @rnu + 1, _udt, cid, tid, sid, rid, day, moa, blk from tmp
		order by cid asc, tid asc, sid asc;
		/***** step 4 - reindex row number group by cid *****/
		update ctn a inner join (select cid, min(rnu) rnu from ctn where udt = _udt group by cid) b on a.cid = b.cid set a.rnu = a.rnu - b.rnu where a.udt = _udt;
		drop table tmp;
		/***** step 5 - fill rid in other conditions *****/
		create temporary table rrr (
			select x.cid, x.tid, x.sid, x.cmb, x.scr, ifnull(y.sct, x.sct) sct from
			(select a.cid, a.tid, a.sid, a.cmb, a.scr, b.sct from inf a inner join cls b on a.cid = b.idt where a.udt = _udt and b.udt = _udt) x left join
			(	select a.cmb, sum(b.sct) sct from
				(select cid, cmb from inf where udt = _udt and cmb is not null) a inner join
				(select * from cls where udt = _udt) b on a.cid = b.idt group by cmb
			) y on x.cmb = y.cmb
		);
		delete from sss where udt = _udt;
		insert into sss (udt, cid, tid, sid, scr, rct, rid, cmb)
		select _udt, z.cid, z.tid, z.sid, z.scr, z.rct, z.rid, z.cmb from
		(	select x.*, count(0) rct, group_concat(y.idt order by y.cap asc, y.idt asc separator '_') rid from
			(	select a.* from
				(select * from rrr where 		cmb is 		null		) a inner join
				(select * from sub where udt = _udt and rtp is		null		) b on a.sid = b.idt left  join
				(select * from rom where udt = _udt and own is not	null and typ = 1) c on a.cid = c.own where c.own is null
			) x inner join
			(select * from rom where udt = _udt and own is		null and typ = 1) y on x.sct <= y.cap
			group by x.cid, x.tid, x.sid, x.cmb, x.scr, x.sct
		) z;
		insert into sss (udt, cid, tid, sid, scr, rct, rid, cmb)
		select _udt, z.cid, z.tid, z.sid, z.scr, z.rct, z.rid, z.cmb from
		(	select a.*, count(0) rct, group_concat(c.idt order by c.cap asc, c.idt asc separator '_') rid from
			(select * from rrr where		 cmb is not	null		) a inner join
			(select * from sub where udt = _udt and  rtp is		null		) b on a.sid = b.idt inner join
			(select * from rom where udt = _udt and  own is		null and typ = 1) c on a.sct <= c.cap
			group by a.cid, a.tid, a.sid, a.cmb, a.scr, a.sct
		) z;
		insert into sss (udt, cid, tid, sid, scr, rct, rid, cmb)
		select _udt, z.cid, z.tid, z.sid, z.scr, z.rct, z.rid, z.cmb from
		(	select a.*, count(0) rct, group_concat(c.idt order by c.cap asc, c.idt asc separator '_') rid from
			(select * from rrr							) a inner join
			(select * from sub where udt = _udt and rtp is	not	null		) b on a.sid = b.idt inner join
			(select * from rom where udt = _udt and own is		null and typ > 1) c on a.sct <= c.cap and b.rtp = c.typ
			group by a.cid, a.tid, a.sid, a.cmb, a.scr, a.sct
		) z;
		update ctn a inner join sss b on a.udt = b.udt and a.cid = b.cid and a.tid = b.tid and a.sid = b.sid
		set a.rid = ifnull(substring_index(b.rid, '_', 1), 0) where a.udt = _udt;
		drop table rrr;
		end;
		end if;
		/**************************************************** something to do with ctn ****************************************************/
		/* configuration */
		if _opt = 0 then
			select nme, val from cnf where udt = _udt and nme not in ('rdy', 'res')
			union
			select 'cct' nme, count(0) val from cls where udt = _udt
			union
			select 'mdl' nme, max(day) - min(day) + 1 val from ctn where udt = _udt
			union
			select 'mml' nme, max(moa) - min(moa) + 1 val from ctn where udt = _udt
			union
			select 'mbl' nme, max(blk) - min(blk) + 1 val from ctn where udt = _udt
			union
			select 'mdy' nme, max(day) val from ctn where udt = _udt
			union
			select 'mma' nme, max(moa) val from ctn where udt = _udt
			union
			select 'mbk' nme, max(blk) val from ctn where udt = _udt;
		/* chromesome */
		elseif _opt = 1 then
			select a.cid, a.cnt - b.scr bgn, a.cnt, a.txt from
			(select cid, count(0) cnt, group_concat(concat(day, ':', moa, ':', blk, ':', rid) order by rnu asc separator '/') txt from ctn where udt = _udt group by cid) a inner join
			(select cid, sum(scr) scr from inf where udt = _udt group by cid) b on a.cid = b.cid;
		/* teacher-class indexes, teachers who teaches more than one class */
		elseif _opt = 2 then
			select x.tid, x.cnt, y.ldy, y.idy, y.lma, y.ima, y.lbk, y.ibk, x.txt from
			(	select	a.tid, count(0) cnt,
					group_concat(concat(a.cid, ':', a.bgn, ':', a.cnt) order by a.cid asc separator '/') txt
				from (select cid, tid, min(rnu) bgn, count(0) cnt from ctn where udt = _udt and tid > 0 group by cid, tid) a
				group by a.tid having count(0) > 1
			) x inner join
			(	select	a.tid, 
					max(b.mdy) - min(b.idy) + 1 ldy, 
					min(b.idy) idy, 
					max(b.mma) - min(b.ima) + 1 lma, 
					min(b.ima) ima, 
					max(b.mbk) - min(b.ibk) + 1 lbk, 
					min(b.ibk) ibk 
				from
			 	(select cid, tid from inf where udt = _udt group by cid, tid) a inner join
			 	(select cid, max(day) mdy, min(day) idy, max(moa) mma, min(moa) ima, max(blk) mbk, min(blk) ibk from ctn where udt = _udt group by cid) b on a.cid = b.cid
				group by tid
			) y on x.tid = y.tid;
		/* teacher-combined-class indexes */
		elseif _opt = 3 then
			select u.tid, u.cnt, v.ldy, v.idy, v.lma, v.ima, v.lbk, v.ibk, u.txt from
			(	select x.tid, count(0) cnt, group_concat(concat(x.scr, '.', cnt, '.', x.txt)) txt from (
					select a.tid, a.sid, a.scr, count(0) cnt, group_concat(concat(a.cid, ':', b.bgn) order by a.cid asc separator '/') txt from
					(select cid, tid, sid, scr, cmb from inf where udt = _udt and cmb is not null) a inner join
					(select cid, tid, sid, min(rnu) bgn from ctn where udt = _udt group by cid, tid, sid) b on a.cid = b.cid and a.tid = b.tid and a.sid = b.sid
					group by a.tid, a.sid, a.scr, a.cmb
				) x group by x.tid
			) u inner join 
			(	select	a.tid, 
					max(b.mdy) - min(b.idy) + 1 ldy, 
					min(b.idy) idy, 
					max(b.mma) - min(b.ima) + 1 lma, 
					min(b.ima) ima, 
					max(b.mbk) - min(b.ibk) + 1 lbk, 
					min(b.ibk) ibk 
				from
				(select cid, tid from inf where udt = _udt and cmb is not null group by cid, tid, cmb) a inner join
			 	(select cid, max(day) mdy, min(day) idy, max(moa) mma, min(moa) ima, max(blk) mbk, min(blk) ibk from ctn where udt = _udt group by cid) b on a.cid = b.cid
				group by tid
			) v on u.tid = v.tid;
		elseif _opt = 4 then
			delete from rst where udt = _udt;
			set @idt := 0;
			insert into rst (cid, tid, sid, rid, day, moa, blk, udt, idt)
			select cid, tid, sid, rid, 0, 0, 0, _udt, @idt := @idt + 1 from ctn where udt = _udt order by cid asc, tid asc, sid asc;
		/* get saved result for check */
		elseif _opt = 5 then
			select a.cid, a.cnt - b.scr bgn, a.cnt, a.txt from
			(select cid, count(0) cnt, group_concat(concat(day, ':', moa, ':', blk, ':', rid) order by idt asc separator '/') txt from rst where udt = _udt group by cid) a inner join
			(select cid, sum(scr) scr from inf where udt = _udt group by cid) b on a.cid = b.cid;
		/* room indexes */
		elseif _opt in (6, 7) then
		begin
			declare cid_ tinyint  unsigned;
			declare nnd_ smallint unsigned;
			declare grp_ smallint unsigned;
			declare rid_ varchar(256);
			create temporary table rcd (
				cid tinyint  unsigned not null,
				bgn smallint unsigned not null,
				nnd smallint unsigned not null,
				rct tinyint  unsigned not null,
				rid varchar(256) not null,
				grp tinyint  unsigned not null
			) engine = memory default charset = utf8;
			insert into rcd (cid, bgn, nnd, rct, rid, grp) select a.cid, b.bgn, a.scr + bgn nnd, a.rct, a.rid, 0 grp
			from (select * from sss where udt = _udt) a inner join 
			(select cid, tid, sid, min(rnu) bgn from ctn where udt = _udt and tid > 0 group by cid, tid, sid) b on a.cid = b.cid and a.tid = b.tid and a.sid = b.sid
			order by a.cid asc, b.bgn asc;

			set grp_ = 1;
			select cid, nnd, rid into cid_, nnd_, rid_ from rcd where grp = 0 limit 1;
			update rcd set grp = grp_ where cid = cid_ and nnd = nnd_ limit 1;
			while exists (select 1 from rcd where grp = 0 limit 1) do
				if exists (select 1 from rcd where grp = 0 and cid = cid_ and bgn = nnd_ and rid = rid_ limit 1) then
					select nnd into nnd_ from rcd where grp = 0 and cid = cid_ and bgn = nnd_ and rid = rid_ limit 1;
				else
					set grp_ = grp_ + 1;
					select cid, nnd, rid into cid_, nnd_, rid_ from rcd where grp = 0 limit 1;
				end if;
				update rcd set grp = grp_ where cid = cid_ and nnd = nnd_ limit 1;
			end while;
			/* get class-begin&count-room index for mutation */
			if _opt = 6 then
				select cid, min(bgn) bgn, max(nnd) - min(bgn) cnt, rct, rid from rcd where rct > 1 group by cid, rct, rid, grp order by cid asc, bgn asc;
			/* get class-begin&count-room index for hard constraint */
			else
				select  y.rid, count(0) cnt, 
					max(z.mdy) - min(z.idy) + 1 ldy, min(z.idy) idy, 
					max(z.mma) - min(z.ima) + 1 lma, min(z.ima) ima, 
					max(z.mbk) - min(z.ibk) + 1 lbk, min(z.ibk) ibk, 
					group_concat(y.cid, ':', y.cnt, ':', y.txt) txt
				from(	select x.rid, x.cid, count(0) cnt, group_concat(concat(x.bgn, '_', x.cnt) order by x.bgn asc separator '/') txt from
					(	select a.cid, a.bgn, a.cnt, substring_index(substring_index(a.rid, '_', b.help_topic_id), '_', -1) rid from
						(select cid, min(bgn) bgn, max(nnd) - min(bgn) cnt, rid from rcd group by cid, rct, rid, grp) a
						inner join mysql.help_topic b on b.help_topic_id <= (length(a.rid) - length(replace(a.rid, '_', '')) + 1)
					) x group by x.rid, x.cid
				) y inner join
				(select cid, max(day) mdy, min(day) idy, max(moa) mma, min(moa) ima, max(blk) mbk, min(blk) ibk from ctn where udt = _udt group by cid) z on y.cid = z.cid
				group by y.rid having count(0) > 1;
			end if;
			drop table rcd;
		end;
		elseif _opt = 8 then
			select  a.rct, a.rid, count(0) cnt, a.scr, 
				max(c.mdy) - min(c.idy) + 1 ldy, min(c.idy) idy, 
				max(c.mma) - min(c.ima) + 1 lma, min(c.ima) ima, 
				max(c.mbk) - min(c.ibk) + 1 lbk, min(c.ibk) ibk,
				group_concat(a.cid, ':', b.bgn) txt
			from (select * from sss where udt = _udt and cmb is not null) a inner join
			(select cid, tid, sid, min(rnu) bgn from ctn where udt = _udt group by cid, tid, sid) b on a.cid = b.cid and a.tid = b.tid and a.sid = b.sid inner join
			(select cid, max(day) mdy, min(day) idy, max(moa) mma, min(moa) ima, max(blk) mbk, min(blk) ibk from ctn where udt = _udt group by cid) c on a.cid = c.cid
			group by a.cmb, a.scr, a.rct, a.rid;
		/* general index - used by soft constraint */
		elseif _opt in (9, 10, 11, 12, 13, 14) then /* 9:cts 10:cst 11:tcs 12:tsc 13:sct 14:stc */
		begin
			declare anm_ varchar(4);
			declare bnm_ varchar(4);
			declare cnm_ varchar(4);
			if _opt = 9 then
				set anm_ = 'cid';
				set bnm_ = 'tid';
				set cnm_ = 'sid';
			elseif _opt = 10 then
				set anm_ = 'cid';
				set bnm_ = 'sid';
				set cnm_ = 'tid';
			elseif _opt = 11 then
				set anm_ = 'tid';
				set bnm_ = 'cid';
				set cnm_ = 'sid';
			elseif _opt = 12 then
				set anm_ = 'tid';
				set bnm_ = 'sid';
				set cnm_ = 'cid';
			elseif _opt = 13 then
				set anm_ = 'sid';
				set bnm_ = 'cid';
				set cnm_ = 'tid';
			elseif _opt = 14 then
				set anm_ = 'sid';
				set bnm_ = 'tid';
				set cnm_ = 'cid';
			end if;
			set @sql = concat('
				select y.', anm_, ', count(0) cnt, sum(y.scr) scr, group_concat(concat(y.', bnm_, ', ''_'', y.cnt, ''_'', y.scr, ''_'', y.txt) order by y.', bnm_, ' asc) txt from (
					select x.', anm_, ', x.', bnm_, ', count(0) cnt, sum(x.cnt) scr, group_concat(concat(x.', cnm_, ', '':'', x.bgn, '':'', x.cnt, '':'', ifnull(x.cmb, 0)) order by x.', cnm_, ' asc separator ''/'') txt from
					(	select a.*, b.cmb from (select cid, tid, sid, min(rnu) bgn, count(0) cnt from ctn where udt = ', _udt, ' and tid > 0 group by cid, tid, sid) a
						left join (select * from inf where udt = ', _udt, ') b on a.cid = b.cid and a.tid = b.tid and a.sid = b.sid
					) x group by x.', anm_, ', x.', bnm_, '
				) y group by y.', anm_
			);
			prepare stmt from @sql;
			execute stmt;
			deallocate prepare stmt;
		end;
		elseif _opt = 15 then
		begin

		end;
		end if;
	end if;

	commit;
end //
delimiter ;

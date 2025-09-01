/** special constraint **/
delimiter //
drop procedure if exists proc_sim;
create procedure proc_sim (
	in  _udt int,
	out _ret bit,
	out _msg varchar(200)
)
_exit_:begin
        declare hasSqlException bit default 0;
        declare continue handler for sqlexception
        begin
                get diagnostics condition 1 _msg = message_text;
                set hasSqlException = 1;
                rollback;
                insert into xlg (prc, ctt) values ('proc_sim', _msg);
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
	else
	begin
	select	idt,
		typ,
		case typ
		when 0 then cct
		when 1 then cct
		when 2 then tct
		when 3 then tct
		when 4 then sct
		else sct end uct,
		case typ
		when 0 then tct
		when 1 then sct
		when 2 then cct
		when 3 then sct
		when 4 then cct
		else tct end vct,
		case typ
		when 0 then sct
		when 1 then tct
		when 2 then sct
		when 3 then cct
		when 4 then tct
		else cct end wct,
		case typ
		when 0 then cls
		when 1 then cls
		when 2 then tls
		when 3 then tls
		when 4 then sls
		else sls end uls,
		case typ
		when 0 then tls
		when 1 then sls
		when 2 then cls
		when 3 then sls
		when 4 then cls
		else tls end vls,
		case typ
		when 0 then sls
		when 1 then tls
		when 2 then sls
		when 3 then cls
		when 4 then tls
		else cls end wls,
		dct,
		conv(dtp,2,10) dtp,
		conv(opt,2,10) opt,
		num,
		lvl,
		dmb
	from
	(
	select	*,
		case cls when '' then 0 else length(cls) - length(replace(cls, ',', '')) + 1 end cct,
		case tls when '' then 0 else length(tls) - length(replace(tls, ',', '')) + 1 end tct,
		case sls when '' then 0 else length(sls) - length(replace(sls, ',', '')) + 1 end sct,
		case dmb when '' then 0 else length(dmb) - length(replace(dmb, ',', '')) + 1 end dct,
		case	case
			when length(cls) = 0 and length(tls) = 0 and length(sls) = 0
			then 'cts'
			when length(cls) = 0 and length(tls) = 0 and length(sls) > 0
			then 'sct'
			when length(cls) = 0 and length(tls) > 0 and length(sls) = 0
			then 'tcs'
			when length(cls) = 0 and length(tls) > 0 and length(sls) > 0
			then 'tsc'
			when length(cls) > 0 and length(tls) = 0 and length(sls) = 0
			then 'cts'
			when length(cls) > 0 and length(tls) = 0 and length(sls) > 0
			then 'cst'
			when length(cls) > 0 and length(tls) > 0 and length(sls) = 0
			then 'cts'
			else 'cts' end
		when 'cts' then 0
		when 'cst' then 1
		when 'tcs' then 2
		when 'tsc' then 3
		when 'sct' then 4
		else 5 end typ
	from sft where udt = _udt and stp = 0 and opt > 3 and opt < 8
	) a;
	end;
	end if;

	commit;
end //
delimiter ;

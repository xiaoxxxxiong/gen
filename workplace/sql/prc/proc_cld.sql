/** clendar **/
delimiter //
drop procedure if exists proc_cld;
CREATE PROCEDURE proc_cld(
        in  _udt int unsigned,
        in  _opt bit(2),
        in  _bdt date,
        in  _ndt date,
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
                insert into xlg (udt, prc, ctt) values (_udt, 'proc_cld', _msg);
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
        elseif _opt = 0 and (_bdt is null or _ndt is null or date(_bdt) is null or date(_ndt) is null) then
                rollback;
                set _ret = 1;
                set _msg = 'begin | end date is null or in wrong format';
                leave _exit_;
        elseif _opt = 0 and (datediff(_ndt, _bdt) < 1 or datediff(_ndt, _bdt) > 255) then
                rollback;
                set _ret = 1;
                set _msg = 'days between begin date and end date are too small or large [1, 255]';
                leave _exit_;
        elseif _opt = 0 then
                truncate table cld;
                insert into cld (udt, ydx, mdx, ddx, dow, woy, ifu)
                select  _udt,
                        year(date_add(_bdt, interval cast(help_topic_id as signed integer) day)) - 2000 ydx,
                        month(date_add(_bdt, interval cast(help_topic_id as signed integer) day)) mdx,
                        day(date_add(_bdt, interval cast(help_topic_id as signed integer) day)) ddx,
                        case dayofweek(date_add(_bdt, interval cast(help_topic_id as signed integer) day)) - 1
                        when 0 then 7 else dayofweek(date_add(_bdt, interval cast(help_topic_id as signed integer) day)) - 1 end dow,
                        weekofyear(date_add(_bdt, interval cast(help_topic_id as signed integer) day)) woy,
                        case dayofweek(date_add(_bdt, interval cast(help_topic_id as signed integer) day)) when 1 then 0 when 7 then 0 else 1 end ifu
                from mysql.help_topic
                where help_topic_id < datediff(_ndt + 1, _bdt)
                order by help_topic_id asc;
        elseif _opt = 1 then
                set @sql = 'update cld set ifu = !ifu where udt = ';
                set @sql = concat(@sql, _udt);
                if _bdt is not null then
                        set @sql = concat(@sql, ' and date(concat(ydx + 2000, ''-'', mdx, ''-'', ddx)) >= ''', _bdt, '''');
                end if;
                if _ndt is not null then
                        set @sql = concat(@sql, ' and date(concat(ydx + 2000, ''-'', mdx, ''-'', ddx)) <= ''', _ndt, '''');
                end if;
                prepare stmt from @sql;
                execute stmt;
                deallocate prepare stmt;
        elseif _opt = 2 then
                set @day := 0;
                select @day := @day + 1 day, a.* from (select ydx, mdx, ddx, dow, woy from cld where conv(ifu,2,10) = 1 and udt = _udt order by ydx asc, mdx asc, ddx asc) a;
        else
                select ydx, mdx, ddx, dow, woy, conv(ifu,2,10) ifu from cld where udt = _udt order by ydx asc, mdx asc, ddx asc;
        end if;

        commit;

end //
delimiter ;

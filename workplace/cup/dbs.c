#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dbs.h"
/*** convert number to string ***/
char * utoa(unsigned int num){
        char * str = (char *)malloc(sizeof(char) * 11);
        char * h = str;
        unsigned int len = 0, lft = 0;
        do{
                * str = num % 10 + 48;
                str++;
                len++;
        }
        while((num = num / 10) > 0);
        * str = '\0';
        str = h;
        char tmp;
        for(; lft < len - 1; lft++,len--){
                tmp = * (str + lft);
                * (str + lft) = * (str + len - 1);
                * (str + len - 1) = tmp;
        }
        return h;
}
/*** get configuration ***/
bxy_t * get_bxy (con_t * con, unsigned int udt){
        MYSQL * obj = (MYSQL *)malloc(sizeof(MYSQL));
        MYSQL_RES * res;
        MYSQL_ROW row;

        char sql[100];
        strcpy(sql, "call proc_get(");
	strcat(sql, utoa(udt));
	strcat(sql, ", 0, @ret, @msg)");
        int ret;
        //connnect
        mysql_init(obj);
        obj = mysql_real_connect(obj, con->ser, con->usr, con->pwd, con->dbn, con->prt, NULL, CLIENT_FOUND_ROWS);
        if(obj == NULL){
                strcpy(con->msg, "access error");
                return NULL;
        }//execute query & store result
        ret = mysql_query(obj, sql);
        if(ret != 0) {
                strcpy(con->msg, "query error");
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        if(res == NULL){
                strcpy(con->msg, "result error:get_bxy");
                mysql_close(obj);
                return NULL;
        }
        //field number & row number
        bxy_t * p = NULL, * bxy_ = NULL;
        bxy_ = p = (bxy_t *)malloc(sizeof(bxy_t) * 1);

        //set rows to struct
        while(1){
                row = mysql_fetch_row(res);
                if(row == NULL)
                        break;
                else {
			if(strcmp(row[0], "bnu") == 0)
				p->bnu = atoi(row[1]);
			else if(strcmp(row[0], "gen") == 0)
				p->gen = atoi(row[1]);
			else if(strcmp(row[0], "cct") == 0)
				p->cct = (unsigned char)atoi(row[1]);
			else if(strcmp(row[0], "mdl") == 0)
				p->mdl = (unsigned char)atoi(row[1]);
			else if(strcmp(row[0], "mml") == 0)
				p->mml = (unsigned char)atoi(row[1]);
			else if(strcmp(row[0], "mbl") == 0)
				p->mbl = (unsigned char)atoi(row[1]);
			else if(strcmp(row[0], "mdy") == 0)
				p->mdy = (unsigned char)atoi(row[1]);
			else if(strcmp(row[0], "mma") == 0)
				p->mma = (unsigned char)atoi(row[1]);
			else if(strcmp(row[0], "mbk") == 0)
				p->mbk = (unsigned char)atoi(row[1]);
			else {};
                }
        }

        mysql_free_result(res);
        int xxx = mysql_next_result(obj);
        if(xxx > 0){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        ret = mysql_query(obj, "select @ret, @msg");
        if(ret == 1){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        row = mysql_fetch_row(res);
        if((* row) != NULL && row[0] != NULL){
                if(atoi(row[0]) == 1){
                        strcpy(con->msg, "procedure returned err:");
                        strcpy(con->msg, row[1]);
                        mysql_close(obj);
                        return NULL;
                        }
        }

        //close connection & free memory
        mysql_free_result(res);
        mysql_close(obj);
        free(obj);

        return bxy_;
}
/*** get genomosome ***/
bsp_t ** get_chr (con_t * con, bxy_t * bxy, unsigned int udt){
        MYSQL * obj = (MYSQL *)malloc(sizeof(MYSQL));
        MYSQL_RES * res;
        MYSQL_ROW row;

        char sql[100];
	if(bxy->chk == 0){
	        strcpy(sql, "call proc_get(");
		strcat(sql, utoa(udt));
		strcat(sql, ", 1, @ret, @msg)");
	}
	else{
	        strcpy(sql, "call proc_get(");
		strcat(sql, utoa(udt));
		strcat(sql, ", 5, @ret, @msg)");
	}
        int ret;
        unsigned int rnu;/* row number */
        //connnect
        mysql_init(obj);
        obj = mysql_real_connect(obj, con->ser, con->usr, con->pwd, con->dbn, con->prt, NULL, CLIENT_FOUND_ROWS);
        if(obj == NULL){
                strcpy(con->msg, "access error");
                return NULL;
        }//execute query & store result
        ret = mysql_query(obj, sql);
        if(ret != 0) {
                strcpy(con->msg, "query error");
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        if(res == NULL){
                strcpy(con->msg, "result error:get_chr");
                mysql_close(obj);
                return NULL;
        }
        //field number & row number
        rnu = mysql_num_rows(res);
	bsp_t ** p = NULL, ** bsp_ = NULL;
	bsp_ = p = (bsp_t **)malloc(sizeof(bsp_t *) * rnu);
	unsigned short * cbc = bxy->cbc = (unsigned short *)malloc(sizeof(unsigned short) * rnu);

        //set rows to struct
        while(1){
                row = mysql_fetch_row(res);
                if(row == NULL)
                        break;
                else {
			* cbc = (unsigned short)atoi(row[2]);
			bsp_t * bsp = * p = (bsp_t *)malloc(sizeof(bsp_t) * (* cbc));
			char * str = row[3], * atr = NULL;
			while(str != NULL){
				atr = strsep(&str, "/");
				bsp->day = (unsigned char)(atoi(strsep(&atr, ":")));
				bsp->moa = (unsigned char)(atoi(strsep(&atr, ":")));
				bsp->blk = (unsigned char)(atoi(strsep(&atr, ":")));
				bsp->rid = (unsigned char)(atoi(strsep(&atr, ":")));
				bsp++;
			}
			cbc++;
                	p++;
                }
        }

        mysql_free_result(res);
        int xxx = mysql_next_result(obj);
        if(xxx > 0){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        ret = mysql_query(obj, "select @ret, @msg");
        if(ret == 1){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        row = mysql_fetch_row(res);
        if((* row) != NULL && row[0] != NULL){
                if(atoi(row[0]) == 1){
                        strcpy(con->msg, "procedure returned err:");
                        strcpy(con->msg, row[1]);
                        mysql_close(obj);
                        return NULL;
                        }
        }

        //close connection & free memory
        mysql_free_result(res);
        mysql_close(obj);
        free(obj);

        return bsp_;
}
/*** get tci ***/
stc_t * get_tci (con_t * con, unsigned int udt){
        MYSQL * obj = (MYSQL *)malloc(sizeof(MYSQL));
        MYSQL_RES * res;
        MYSQL_ROW row;

        char sql[100];
        strcpy(sql, "call proc_get(");
	strcat(sql, utoa(udt));
	strcat(sql, ", 2, @ret, @msg)");
        int ret;
        unsigned int rnu;/* row number */
        //connnect
        mysql_init(obj);
        obj = mysql_real_connect(obj, con->ser, con->usr, con->pwd, con->dbn, con->prt, NULL, CLIENT_FOUND_ROWS);
        if(obj == NULL){
                strcpy(con->msg, "access error");
                return NULL;
        }//execute query & store result
        ret = mysql_query(obj, sql);
        if(ret != 0) {
                strcpy(con->msg, "query error");
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        if(res == NULL){
                strcpy(con->msg, "result error:get_bxy");
                mysql_close(obj);
                return NULL;
        }
        //field number & row number
        rnu = mysql_num_rows(res);
        stc_t * stc_ = (stc_t * )malloc(sizeof(stc_t));
        tci_t * p = NULL, * tci_ = NULL;
        tci_ = p = (tci_t *)malloc(sizeof(tci_t) * rnu);

        //set rows to struct
        while(1){
                row = mysql_fetch_row(res);
                if(row == NULL)
                        break;
                else {
			p->tid = (unsigned char)atoi(row[0]);
			p->cnt = (unsigned char)atoi(row[1]);
			p->ldy = (unsigned char)atoi(row[2]);
			p->idy = (unsigned char)atoi(row[3]);
			p->lma = (unsigned char)atoi(row[4]);
			p->ima = (unsigned char)atoi(row[5]);
			p->lbk = (unsigned char)atoi(row[6]);
			p->ibk = (unsigned char)atoi(row[7]);
			cbg_t * cbg = p->cbg = (cbg_t *)malloc(sizeof(cbg_t) * p->cnt);
			char * str = row[8], * atr = NULL;
			while(str != NULL){
				atr = strsep(&str, "/");
				cbg->cid = (unsigned char)(atoi(strsep(&atr, ":")));
				cbg->bgn = (unsigned short)(atoi(strsep(&atr, ":")));
				cbg->cnt = (unsigned short)(atoi(strsep(&atr, ":")));
				cbg++;
			}
                	p++;
                }
        }

        stc_->bsc = tci_;
        stc_->cnt = rnu;

        mysql_free_result(res);
        int xxx = mysql_next_result(obj);
        if(xxx > 0){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        ret = mysql_query(obj, "select @ret, @msg");
        if(ret == 1){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        row = mysql_fetch_row(res);
        if((* row) != NULL && row[0] != NULL){
                if(atoi(row[0]) == 1){
                        strcpy(con->msg, "procedure returned err:");
                        strcpy(con->msg, row[1]);
                        mysql_close(obj);
                        return NULL;
                        }
        }

        //close connection & free memory
        mysql_free_result(res);
        mysql_close(obj);
        free(obj);

        return stc_;
}
/*** get tcc ***/
stc_t * get_tcc (con_t * con, unsigned int udt){
        MYSQL * obj = (MYSQL *)malloc(sizeof(MYSQL));
        MYSQL_RES * res;
        MYSQL_ROW row;

        char sql[100];
        strcpy(sql, "call proc_get(");
	strcat(sql, utoa(udt));
	strcat(sql, ", 3, @ret, @msg)");
        int ret;
        unsigned int rnu;/* row number */
        //connnect
        mysql_init(obj);
        obj = mysql_real_connect(obj, con->ser, con->usr, con->pwd, con->dbn, con->prt, NULL, CLIENT_FOUND_ROWS);
        if(obj == NULL){
                strcpy(con->msg, "access error");
                return NULL;
        }//execute query & store result
        ret = mysql_query(obj, sql);
        if(ret != 0) {
                strcpy(con->msg, "query error");
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        if(res == NULL){
                strcpy(con->msg, "result error:get_bxy");
                mysql_close(obj);
                return NULL;
        }
        //field number & row number
        rnu = mysql_num_rows(res);
        stc_t * stc_ = (stc_t * )malloc(sizeof(stc_t));
        tcc_t * p = NULL, * tcc_ = NULL;
        tcc_ = p = (tcc_t *)malloc(sizeof(tcc_t) * rnu);

        //set rows to struct
        while(1){
                row = mysql_fetch_row(res);
                if(row == NULL)
                        break;
                else {
			p->tid = (unsigned char)atoi(row[0]);
			p->cnt = (unsigned char)atoi(row[1]);
			p->ldy = (unsigned char)atoi(row[2]);
			p->idy = (unsigned char)atoi(row[3]);
			p->lma = (unsigned char)atoi(row[4]);
			p->ima = (unsigned char)atoi(row[5]);
			p->lbk = (unsigned char)atoi(row[6]);
			p->ibk = (unsigned char)atoi(row[7]);
			cmb_t * cmb = p->cmb = (cmb_t *)malloc(sizeof(cmb_t) * p->cnt);
			char * str = row[8], * atr = NULL, * btr = NULL, * ctr = NULL;
			while(str != NULL){
				atr = strsep(&str, ",");
				cmb->scr = (unsigned short)(atoi(strsep(&atr, ".")));
				cmb->cnt = (unsigned char)(atoi(strsep(&atr, ".")));
				
				cab_t * cab = cmb->cab = (cab_t *)malloc(sizeof(cab_t) * cmb->cnt);
				btr = strsep(&atr, ".");
				while(btr != NULL){
					ctr = strsep(&btr, "/");
					cab->cid = (unsigned char)(atoi(strsep(&ctr, ":")));
					cab->bgn = (unsigned short)(atoi(strsep(&ctr, ":")));
					cab++;
				}

				cmb++;
			}
                	p++;
                }
        }

        stc_->bsc = tcc_;
        stc_->cnt = rnu;

        mysql_free_result(res);
        int xxx = mysql_next_result(obj);
        if(xxx > 0){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        ret = mysql_query(obj, "select @ret, @msg");
        if(ret == 1){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        row = mysql_fetch_row(res);
        if((* row) != NULL && row[0] != NULL){
                if(atoi(row[0]) == 1){
                        strcpy(con->msg, "procedure returned err:");
                        strcpy(con->msg, row[1]);
                        mysql_close(obj);
                        return NULL;
                        }
        }

        //close connection & free memory
        mysql_free_result(res);
        mysql_close(obj);
        free(obj);

        return stc_;
}
/*** set result ***/
unsigned int set_res (con_t * con, bsp_t bsp, unsigned int udt, unsigned int idt){
        MYSQL * obj = (MYSQL *)malloc(sizeof(MYSQL));
        MYSQL_RES * res;

        char sql[100];
        strcpy(sql, "update rst set day='");
	strcat(sql, utoa(bsp.day));
	strcat(sql, "',moa='");
	strcat(sql, utoa(bsp.moa));
	strcat(sql, "',blk='");
	strcat(sql, utoa(bsp.blk));
	strcat(sql, "',rid='");
	strcat(sql, utoa(bsp.rid));
	strcat(sql, "' where udt = ");
	strcat(sql, utoa(udt));
	strcat(sql, " and idt = ");
	strcat(sql, utoa(idt));
        int ret;
        //connnect
        mysql_init(obj);
        obj = mysql_real_connect(obj, con->ser, con->usr, con->pwd, con->dbn, con->prt, NULL, CLIENT_FOUND_ROWS);
        if(obj == NULL){
                strcpy(con->msg, "access error");
                return 1;
        }//execute query & store result
        ret = mysql_query(obj, sql);
        if(ret != 0) {
                strcpy(con->msg, "query error");
                mysql_close(obj);
                return 1;
        }
        res = mysql_store_result(obj);
        if(res == NULL){
                strcpy(con->msg, "result error:set_res");
                mysql_close(obj);
                return 1;
        }

        //close connection & free memory
        mysql_free_result(res);
        mysql_close(obj);
        free(obj);

        return 0;
}
/*** get room index ***/
stc_t * get_rid (con_t * con, unsigned int udt){
        MYSQL * obj = (MYSQL *)malloc(sizeof(MYSQL));
        MYSQL_RES * res;
        MYSQL_ROW row;

        char sql[100];
        strcpy(sql, "call proc_get(");
	strcat(sql, utoa(udt));
	strcat(sql, ", 6, @ret, @msg)");
        int ret;
        unsigned int rnu;/* row number */
        //connnect
        mysql_init(obj);
        obj = mysql_real_connect(obj, con->ser, con->usr, con->pwd, con->dbn, con->prt, NULL, CLIENT_FOUND_ROWS);
        if(obj == NULL){
                strcpy(con->msg, "access error");
                return NULL;
        }//execute query & store result
        ret = mysql_query(obj, sql);
        if(ret != 0) {
                strcpy(con->msg, "query error");
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        if(res == NULL){
                strcpy(con->msg, "result error:get_bxy");
                mysql_close(obj);
                return NULL;
        }
        //field number & row number
        rnu = mysql_num_rows(res);
        stc_t * stc_ = (stc_t * )malloc(sizeof(stc_t));
        rid_t * p = NULL, * rid_ = NULL;
        rid_ = p = (rid_t *)malloc(sizeof(rid_t) * rnu);

        //set rows to struct
        while(1){
                row = mysql_fetch_row(res);
                if(row == NULL)
                        break;
                else {
			p->cid = (unsigned short)atoi(row[0]);
			p->bgn = (unsigned short)atoi(row[1]);
			p->cnt = (unsigned short)atoi(row[2]);
			p->rct = (unsigned short)atoi(row[3]);
			unsigned short * rid = p->rid = (unsigned short *)malloc(sizeof(unsigned short) * p->rct);
			char * str = row[4];
			while(str != NULL){
				* rid = (unsigned short)(atoi(strsep(&str, "_")));
				rid++;
			}
                	p++;
                }
        }

        stc_->bsc = rid_;
        stc_->cnt = rnu;

        mysql_free_result(res);
        int xxx = mysql_next_result(obj);
        if(xxx > 0){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        ret = mysql_query(obj, "select @ret, @msg");
        if(ret == 1){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        row = mysql_fetch_row(res);
        if((* row) != NULL && row[0] != NULL){
                if(atoi(row[0]) == 1){
                        strcpy(con->msg, "procedure returned err:");
                        strcpy(con->msg, row[1]);
                        mysql_close(obj);
                        return NULL;
                        }
        }

        //close connection & free memory
        mysql_free_result(res);
        mysql_close(obj);
        free(obj);

        return stc_;
}
/*** get room_class index ***/
stc_t * get_rci (con_t * con, unsigned int udt){
        MYSQL * obj = (MYSQL *)malloc(sizeof(MYSQL));
        MYSQL_RES * res;
        MYSQL_ROW row;

        char sql[100];
        strcpy(sql, "call proc_get(");
	strcat(sql, utoa(udt));
	strcat(sql, ", 7, @ret, @msg)");
        int ret;
        unsigned int rnu;/* row number */
        //connnect
        mysql_init(obj);
        obj = mysql_real_connect(obj, con->ser, con->usr, con->pwd, con->dbn, con->prt, NULL, CLIENT_FOUND_ROWS);
        if(obj == NULL){
                strcpy(con->msg, "access error");
                return NULL;
        }//execute query & store result
        ret = mysql_query(obj, sql);
        if(ret != 0) {
                strcpy(con->msg, "query error");
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        if(res == NULL){
                strcpy(con->msg, "result error:get_rci");
                mysql_close(obj);
                return NULL;
        }
        //field number & row number
        rnu = mysql_num_rows(res);
        stc_t * stc_ = (stc_t * )malloc(sizeof(stc_t));
	rci_t * p = NULL, * rci_ = NULL;
	rci_ = p = (rci_t *)malloc(sizeof(rci_t) * rnu);

        //set rows to struct
        while(1){
                row = mysql_fetch_row(res);
                if(row == NULL)
                        break;
                else {
			p->rid = (unsigned short)atoi(row[0]);
			p->cnt = (unsigned short)atoi(row[1]);
			p->ldy = (unsigned short)atoi(row[2]);
			p->idy = (unsigned short)atoi(row[3]);
			p->lma = (unsigned char)atoi(row[4]);
			p->ima = (unsigned char)atoi(row[5]);
			p->lbk = (unsigned char)atoi(row[6]);
			p->ibk = (unsigned char)atoi(row[7]);
			ccc_t * ccc = p->ccc = (ccc_t *)malloc(sizeof(ccc_t) * p->cnt);
			char * str = row[8], * atr = NULL, * btr = NULL;
			while(str != NULL){
				atr = strsep(&str, ",");
				ccc->cid = (unsigned short)(atoi(strsep(&atr, ":")));
				ccc->cnt = (unsigned short)(atoi(strsep(&atr, ":")));
				bac_t * bac = ccc->bac = (bac_t *)malloc(sizeof(bac_t) * ccc->cnt);
				while(atr != NULL){
					btr = strsep(&atr, "/");
					bac->bgn = (unsigned short)(atoi(strsep(&btr, "_")));
					bac->cnt = (unsigned short)(atoi(strsep(&btr, "_")));
					bac++;
				}
				ccc++;
			}
                	p++;
                }
        }

        stc_->bsc = rci_;
        stc_->cnt = rnu;

        mysql_free_result(res);
        int xxx = mysql_next_result(obj);
        if(xxx > 0){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        ret = mysql_query(obj, "select @ret, @msg");
        if(ret == 1){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        row = mysql_fetch_row(res);
        if((* row) != NULL && row[0] != NULL){
                if(atoi(row[0]) == 1){
                        strcpy(con->msg, "procedure returned err:");
                        strcpy(con->msg, row[1]);
                        mysql_close(obj);
                        return NULL;
                        }
        }

        //close connection & free memory
        mysql_free_result(res);
        mysql_close(obj);
        free(obj);

        return stc_;
}
/*** get room_combined_class index ***/
stc_t * get_rcc (con_t * con, unsigned int udt){
        MYSQL * obj = (MYSQL *)malloc(sizeof(MYSQL));
        MYSQL_RES * res;
        MYSQL_ROW row;

        char sql[100];
        strcpy(sql, "call proc_get(");
	strcat(sql, utoa(udt));
	strcat(sql, ", 8, @ret, @msg)");
        int ret;
        unsigned int rnu;/* row number */
        //connnect
        mysql_init(obj);
        obj = mysql_real_connect(obj, con->ser, con->usr, con->pwd, con->dbn, con->prt, NULL, CLIENT_FOUND_ROWS);
        if(obj == NULL){
                strcpy(con->msg, "access error");
                return NULL;
        }//execute query & store result
        ret = mysql_query(obj, sql);
        if(ret != 0) {
                strcpy(con->msg, "query error");
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        if(res == NULL){
                strcpy(con->msg, "result error:get_rcc");
                mysql_close(obj);
                return NULL;
        }
        //field number & row number
        rnu = mysql_num_rows(res);
        stc_t * stc_ = (stc_t * )malloc(sizeof(stc_t));
	rcc_t * p = NULL, * rcc_ = NULL;
	rcc_ = p = (rcc_t *)malloc(sizeof(rcc_t) * rnu);

        //set rows to struct
        while(1){
                row = mysql_fetch_row(res);
                if(row == NULL)
                        break;
                else {
			p->rct = (unsigned char)atoi(row[0]);
			unsigned short * rid = p->rid = (unsigned short *)malloc(sizeof(unsigned short) * p->rct);
			char * tst = row[1];
			while(tst != NULL){
				* rid = (unsigned char)(atoi(strsep(&tst, "_")));
				rid++;
			}
			p->cnt = (unsigned char)atoi(row[2]);
			p->scr = (unsigned char)atoi(row[3]);

			p->ldy = (unsigned short)atoi(row[4]);
			p->idy = (unsigned short)atoi(row[5]);
			p->lma = (unsigned char)atoi(row[6]);
			p->ima = (unsigned char)atoi(row[7]);
			p->lbk = (unsigned char)atoi(row[8]);
			p->ibk = (unsigned char)atoi(row[9]);
			cab_t * cab = p->cab = (cab_t *)malloc(sizeof(cab_t) * p->cnt);
			char * str = row[10], * atr = NULL;
			while(str != NULL){
				atr = strsep(&str, ",");
				cab->cid = (unsigned short)(atoi(strsep(&atr, ":")));
				cab->bgn = (unsigned short)(atoi(strsep(&atr, ":")));
				cab++;
			}
                	p++;
                }
        }

        stc_->bsc = rcc_;
        stc_->cnt = rnu;

        mysql_free_result(res);
        int xxx = mysql_next_result(obj);
        if(xxx > 0){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        ret = mysql_query(obj, "select @ret, @msg");
        if(ret == 1){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        row = mysql_fetch_row(res);
        if((* row) != NULL && row[0] != NULL){
                if(atoi(row[0]) == 1){
                        strcpy(con->msg, "procedure returned err:");
                        strcpy(con->msg, row[1]);
                        mysql_close(obj);
                        return NULL;
                        }
        }

        //close connection & free memory
        mysql_free_result(res);
        mysql_close(obj);
        free(obj);

        return stc_;
}
/*** get general index - used for soft constraint ***/
stc_t * get_xls (con_t * con, unsigned char opt, unsigned int udt){
        MYSQL * obj = (MYSQL *)malloc(sizeof(MYSQL));
        MYSQL_RES * res;
        MYSQL_ROW row;

        char sql[100];
        strcpy(sql, "call proc_get(");
	strcat(sql, utoa(udt));
	strcat(sql, ", ");
	strcat(sql, utoa(opt));
	strcat(sql, ", @ret, @msg)");
        int ret;
        unsigned int rnu;/* row number */
        //connnect
        mysql_init(obj);
        obj = mysql_real_connect(obj, con->ser, con->usr, con->pwd, con->dbn, con->prt, NULL, CLIENT_FOUND_ROWS);
        if(obj == NULL){
                strcpy(con->msg, "access error");
                return NULL;
        }//execute query & store result
        ret = mysql_query(obj, sql);
        if(ret != 0) {
                strcpy(con->msg, "query error");
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        if(res == NULL){
                strcpy(con->msg, "result error:get_rcc");
                mysql_close(obj);
                return NULL;
        }
        //field number & row number
        rnu = mysql_num_rows(res);
        stc_t * stc_ = (stc_t * )malloc(sizeof(stc_t));
	xls_t * p = NULL, * xls_ = NULL;
	xls_ = p = (xls_t *)malloc(sizeof(xls_t) * rnu);

        //set rows to struct
        while(1){
                row = mysql_fetch_row(res);
                if(row == NULL)
                        break;
                else {
			p->xid = (unsigned short)atoi(row[0]);
			p->cnt = (unsigned short)atoi(row[1]);
			p->scr = (unsigned short)atoi(row[2]);
			yls_t * yls = p->yls = (yls_t *)malloc(sizeof(yls_t) * p->cnt);
			char * str = row[3], * atr = NULL, * btr = NULL, * ctr = NULL;
			while(str != NULL){
				atr = strsep(&str, ",");
				yls->yid = (unsigned short)(atoi(strsep(&atr, "_")));
				yls->cnt = (unsigned short)(atoi(strsep(&atr, "_")));
				yls->scr = (unsigned short)(atoi(strsep(&atr, "_")));

				zls_t * zls = yls->zls = (zls_t *)malloc(sizeof(zls_t) * yls->cnt);
				btr = strsep(&atr, "_");
				while(btr != NULL){
					ctr = strsep(&btr, "/");
					zls->zid = (unsigned short)(atoi(strsep(&ctr, ":")));
					zls->bgn = (unsigned short)(atoi(strsep(&ctr, ":")));
					zls->cnt = (unsigned short)(atoi(strsep(&ctr, ":")));
					zls->cmb = (unsigned char)(atoi(strsep(&ctr, ":")));
					zls++;
				}
				yls++;
			}
                	p++;
                }
        }

        stc_->bsc = xls_;
        stc_->cnt = rnu;

        mysql_free_result(res);
        int xxx = mysql_next_result(obj);
        if(xxx > 0){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        ret = mysql_query(obj, "select @ret, @msg");
        if(ret == 1){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        row = mysql_fetch_row(res);
        if((* row) != NULL && row[0] != NULL){
                if(atoi(row[0]) == 1){
                        strcpy(con->msg, "procedure returned err:");
                        strcpy(con->msg, row[1]);
                        mysql_close(obj);
                        return NULL;
                        }
        }

        //close connection & free memory
        mysql_free_result(res);
        mysql_close(obj);
        free(obj);

        return stc_;
}
/*** get soft constraint ***/
stc_t * get_sft (con_t * con, unsigned int udt){
        MYSQL * obj = (MYSQL *)malloc(sizeof(MYSQL));
        MYSQL_RES * res;
        MYSQL_ROW row;

        char sql[100];
        strcpy(sql, "call proc_sft(");
	strcat(sql, utoa(udt));
	strcat(sql, ", @ret, @msg)");
        int ret;
        unsigned int rnu;/* row number */
        //connnect
        mysql_init(obj);
        obj = mysql_real_connect(obj, con->ser, con->usr, con->pwd, con->dbn, con->prt, NULL, CLIENT_FOUND_ROWS);
        if(obj == NULL){
                strcpy(con->msg, "access error");
                return NULL;
        }//execute query & store result
        ret = mysql_query(obj, sql);
        if(ret != 0) {
                strcpy(con->msg, "query error");
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        if(res == NULL){
                strcpy(con->msg, "result error:get_sft");
                mysql_close(obj);
                return NULL;
        }
        //field number & row number
        rnu = mysql_num_rows(res);
        stc_t * stc_ = (stc_t * )malloc(sizeof(stc_t));
        sft_t * p = NULL, * sft_ = NULL;
        sft_ = p = (sft_t *)malloc(sizeof(sft_t) * rnu);
	unsigned int len;

        //set rows to struct
        while(1){
                row = mysql_fetch_row(res);
                if(row == NULL)
                        break;
                else {
			char * str = NULL, * atr = NULL;
			p->idt = atoi(row[0]);
			p->typ = atoi(row[1]);
			p->uct = atoi(row[2]);
			p->vct = atoi(row[3]);
			p->wct = atoi(row[4]);
			len = strlen(row[5]) + 1;
			p->stu = (char *)malloc(sizeof(char) * len);
			strcpy(p->stu, row[5]);//uls str, used for print not satisfied constraints
			if(p->uct > 0){
				str = row[5];
				unsigned short * uls = p->uls = (unsigned short *)malloc(sizeof(unsigned short) * p->uct);
				while(str != NULL){
					atr = strsep(&str, ",");
					* uls = atoi(atr);
					uls++;
				}
			}else p->uls = NULL;
			len = strlen(row[6]) + 1;
			p->stv = (char *)malloc(sizeof(char) * len);
			strcpy(p->stv, row[6]);//vls str, used for print not satisfied constraints
			if(p->vct > 0){
				str = row[6];
				unsigned short * vls = p->vls = (unsigned short *)malloc(sizeof(unsigned short) * p->vct);
				while(str != NULL){
					atr = strsep(&str, ",");
					* vls = atoi(atr);
					vls++;
				}
			}else p->vls = NULL;
			len = strlen(row[7]) + 1;
			p->stw = (char *)malloc(sizeof(char) * len);
			strcpy(p->stw, row[7]);//wls str, used for print not satisfied constraints
			if(p->wct > 0){
				str = row[7];
				unsigned short * wls = p->wls = (unsigned short *)malloc(sizeof(unsigned short) * p->wct);
				while(str != NULL){
					atr = strsep(&str, ",");
					* wls = atoi(atr);
					wls++;
				}
			}else p->wls = NULL;
			p->dct = atoi(row[8]);
			p->dtp = atoi(row[9]);
			p->opt = atoi(row[10]);
			p->num = atoi(row[11]);
			p->lvl = atoi(row[12]);
			len = strlen(row[13]) + 1;
			p->str = (char *)malloc(sizeof(char) * len);
			strcpy(p->str, row[13]);//dmb str, used for print not satisfied constraints
			if(p->dct > 0){
				str = row[13];
				bsp_t * dmb = p->dmb = (bsp_t *)malloc(sizeof(bsp_t) * p->dct);
				while(str != NULL){
					atr = strsep(&str, ",");
					dmb->day = atoi(strsep(&atr, ":"));
					dmb->moa = atoi(strsep(&atr, ":"));
					dmb->blk = atoi(strsep(&atr, ":"));
					dmb++;
				}
			}else p->dmb = NULL;
                	p++;
                }
        }

        stc_->bsc = sft_;
        stc_->cnt = rnu;

        mysql_free_result(res);
        int xxx = mysql_next_result(obj);
        if(xxx > 0){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        ret = mysql_query(obj, "select @ret, @msg");
        if(ret == 1){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        row = mysql_fetch_row(res);
        if((* row) != NULL && row[0] != NULL){
                if(atoi(row[0]) == 1){
                        strcpy(con->msg, "procedure returned err:");
                        strcpy(con->msg, row[1]);
                        mysql_close(obj);
                        return NULL;
                        }
        }

        //close connection & free memory
        mysql_free_result(res);
        mysql_close(obj);
        free(obj);

        return stc_;
}
/*** get simultaneity constraint ***/
stc_t * get_sim (con_t * con, unsigned int udt){
        MYSQL * obj = (MYSQL *)malloc(sizeof(MYSQL));
        MYSQL_RES * res;
        MYSQL_ROW row;

        char sql[100];
        strcpy(sql, "call proc_sim(");
	strcat(sql, utoa(udt));
	strcat(sql, ", @ret, @msg)");
        int ret;
        unsigned int rnu;/* row number */
        //connnect
        mysql_init(obj);
        obj = mysql_real_connect(obj, con->ser, con->usr, con->pwd, con->dbn, con->prt, NULL, CLIENT_FOUND_ROWS);
        if(obj == NULL){
                strcpy(con->msg, "access error");
                return NULL;
        }//execute query & store result
        ret = mysql_query(obj, sql);
        if(ret != 0) {
                strcpy(con->msg, "query error");
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        if(res == NULL){
                strcpy(con->msg, "result error:get_sim");
                mysql_close(obj);
                return NULL;
        }
        //field number & row number
        rnu = mysql_num_rows(res);
        stc_t * stc_ = (stc_t * )malloc(sizeof(stc_t));
        sft_t * p = NULL, * sft_ = NULL;
        sft_ = p = (sft_t *)malloc(sizeof(sft_t) * rnu);
	unsigned int len;

        //set rows to struct
        while(1){
                row = mysql_fetch_row(res);
                if(row == NULL)
                        break;
                else {
			char * str = NULL, * atr = NULL;
			p->idt = atoi(row[0]);
			p->typ = atoi(row[1]);
			p->uct = atoi(row[2]);
			p->vct = atoi(row[3]);
			p->wct = atoi(row[4]);
			len = strlen(row[5]) + 1;
			p->stu = (char *)malloc(sizeof(char) * len);
			strcpy(p->stu, row[5]);//uls str, used for print not satisfied constraints
			if(p->uct > 0){
				str = row[5];
				unsigned short * uls = p->uls = (unsigned short *)malloc(sizeof(unsigned short) * p->uct);
				while(str != NULL){
					atr = strsep(&str, ",");
					* uls = atoi(atr);
					uls++;
				}
			}else p->uls = NULL;
			len = strlen(row[6]) + 1;
			p->stv = (char *)malloc(sizeof(char) * len);
			strcpy(p->stv, row[6]);//vls str, used for print not satisfied constraints
			if(p->vct > 0){
				str = row[6];
				unsigned short * vls = p->vls = (unsigned short *)malloc(sizeof(unsigned short) * p->vct);
				while(str != NULL){
					atr = strsep(&str, ",");
					* vls = atoi(atr);
					vls++;
				}
			}else p->vls = NULL;
			len = strlen(row[7]) + 1;
			p->stw = (char *)malloc(sizeof(char) * len);
			strcpy(p->stw, row[7]);//wls str, used for print not satisfied constraints
			if(p->wct > 0){
				str = row[7];
				unsigned short * wls = p->wls = (unsigned short *)malloc(sizeof(unsigned short) * p->wct);
				while(str != NULL){
					atr = strsep(&str, ",");
					* wls = atoi(atr);
					wls++;
				}
			}else p->wls = NULL;
			p->dct = atoi(row[8]);
			p->dtp = atoi(row[9]);
			p->opt = atoi(row[10]);
			p->num = atoi(row[11]);
			p->lvl = atoi(row[12]);
			len = strlen(row[13]) + 1;
			p->str = (char *)malloc(sizeof(char) * len);
			strcpy(p->str, row[13]);//dmb str, used for print not satisfied constraints
			if(p->dct > 0){
				str = row[13];
				bsp_t * dmb = p->dmb = (bsp_t *)malloc(sizeof(bsp_t) * p->dct);
				while(str != NULL){
					atr = strsep(&str, ",");
					dmb->day = atoi(strsep(&atr, ":"));
					dmb->moa = atoi(strsep(&atr, ":"));
					dmb->blk = atoi(strsep(&atr, ":"));
					dmb++;
				}
			}else p->dmb = NULL;
                	p++;
                }
        }

        stc_->bsc = sft_;
        stc_->cnt = rnu;

        mysql_free_result(res);
        int xxx = mysql_next_result(obj);
        if(xxx > 0){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        ret = mysql_query(obj, "select @ret, @msg");
        if(ret == 1){
                strcpy(con->msg, mysql_error(obj));
                mysql_close(obj);
                return NULL;
        }
        res = mysql_store_result(obj);
        row = mysql_fetch_row(res);
        if((* row) != NULL && row[0] != NULL){
                if(atoi(row[0]) == 1){
                        strcpy(con->msg, "procedure returned err:");
                        strcpy(con->msg, row[1]);
                        mysql_close(obj);
                        return NULL;
                        }
        }

        //close connection & free memory
        mysql_free_result(res);
        mysql_close(obj);
        free(obj);

        return stc_;
}
/*** get usr ***/
unsigned int get_usr (con_t * con, char * ton, char * pwd, unsigned int * udt){
        MYSQL * obj = (MYSQL *)malloc(sizeof(MYSQL));
        MYSQL_RES * res;
        MYSQL_ROW row;

        char sql[500];
        strcpy(sql, "select ifnull(b.stt, 4) stt, a.idt from (select idt from usr where (tel ='");
	strcat(sql, ton);
	strcat(sql, "' or nme='");
	strcat(sql, ton);
	strcat(sql, "') and pwd = '");
	strcat(sql, pwd);
	strcat(sql, "') a left join tsk b on a.idt = b.udt");
        int ret;
	unsigned int flg = 2;
        unsigned int rnu;/* row number */
        //connnect
        mysql_init(obj);
        obj = mysql_real_connect(obj, con->ser, con->usr, con->pwd, con->dbn, con->prt, NULL, CLIENT_FOUND_ROWS);
        if(obj == NULL){
                strcpy(con->msg, "access error");
                return 1;
        }//execute query & store result
        ret = mysql_query(obj, sql);
        if(ret != 0) {
                strcpy(con->msg, "query error");
                mysql_close(obj);
                return 1;
        }
        res = mysql_store_result(obj);
        if(res == NULL){
                strcpy(con->msg, "result error:get_usr");
                mysql_close(obj);
                return 1;
        }
        //field number & row number
        rnu = mysql_num_rows(res);
	if(rnu > 0){
                row = mysql_fetch_row(res);
                if(strcmp(row[0], "4") == 0)
                        flg = 3;//there is no info in task table - udt
		else if(strcmp(row[0], "1") == 0)
			flg = 4;
		else
			flg = atoi(row[0]) + 100;
		* udt = atoi(row[1]);
	}

        //close connection & free memory
        mysql_free_result(res);
        mysql_close(obj);
        free(obj);

        return flg;
}
unsigned int set_stt(con_t * con, unsigned char stt, unsigned int udt){
        MYSQL * obj = (MYSQL *)malloc(sizeof(MYSQL));
        MYSQL_RES * res;

        char sql[100];
        strcpy(sql, "update tsk set stt=");
	strcat(sql, utoa(stt));
	strcat(sql, " where udt = ");
	strcat(sql, utoa(udt));
        int ret;
        //connnect
        mysql_init(obj);
        obj = mysql_real_connect(obj, con->ser, con->usr, con->pwd, con->dbn, con->prt, NULL, CLIENT_FOUND_ROWS);
        if(obj == NULL){
                strcpy(con->msg, "access error");
                return 1;
        }
	//execute query & store result
        ret = mysql_query(obj, sql);
        if(ret != 0) {
                strcpy(con->msg, "query error");
                mysql_close(obj);
                return 1;
        }
        res = mysql_store_result(obj);
        if(res == NULL){
                strcpy(con->msg, "result error:set_stt");
                mysql_close(obj);
                return 1;
        }

        //close connection & free memory
        mysql_free_result(res);
        mysql_close(obj);
        free(obj);

        return 0;
}

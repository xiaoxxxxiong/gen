#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <errno.h>

#include "cup/stc.h"
#include "cup/dbs.h"
#include "cup/mcr.h"

#define _VER_ "002"
#define _SUB_ "002"
#define _MDF_ "001"
#define _DAT_ __DATE__
#ifdef _DBG_
#define _GET_TIME_OF_DAY_(p0,p1) (gettimeofday(p0,p1));
#define _TIME_ (1000000 * (b.tv_sec - a.tv_sec) + b.tv_usec - a.tv_usec);
#else
#define _GET_TIME_OF_DAY_(p0,p1) /**/
#define _TIME_ /**/
#endif
#define MAX 4096
//special constraint dynamic link object
fun dyn;
void sed(){
	//init seed
	srand(time(NULL));
}
unsigned int rnd(unsigned int min, unsigned int max){
	//return random value [max value: max - 1]
	return rand() % (max - min) + min;
}
void cnx(con_t ** con){
	FILE * fpn = NULL;
	if((fpn = fopen("./dbs.conf" , "r")) == NULL)
	{
		printf("can not open dbs.conf!\n");
		exit(EXIT_FAILURE);
	}
	unsigned int max = 1024;
	char buf[1024], * bug, * key, * val;
	* con = (con_t *)malloc(sizeof(con_t));
        (* con)->ser = (char *)malloc(sizeof(char) * 33);
        (* con)->usr = (char *)malloc(sizeof(char) * 33);
        (* con)->pwd = (char *)malloc(sizeof(char) * 33);
        (* con)->dbn = (char *)malloc(sizeof(char) * 33);
        (* con)->prt = 3306;
        (* con)->msg = (char *)malloc(sizeof(char) * 33);
	while(!feof(fpn))
	{
		if(fgets(buf, max, fpn)){
			bug = buf;

			while((* bug) != '\0'){
				if((* bug) == '\n')
					* bug = '\0';
				bug++;
			}

			key = strtok(buf, ":");
			val = strtok(NULL, ":");

			if(strcmp(key, "ser") == 0)
	       	 		strcpy((* con)->ser, val);
			else if(strcmp(key, "prt") == 0)
				(* con)->prt = atoi(val);
			else if(strcmp(key, "usr") == 0)
				strcpy((* con)->usr, val);
			else if(strcmp(key, "pwd") == 0)
				strcpy((* con)->pwd, val);
			else if(strcmp(key, "dbn") == 0)
				strcpy((* con)->dbn, val);
		}
	}
	fclose(fpn);
	strcpy((* con)->msg, "");
}
void grx(bxy_t * bxy, bsp_t ** chr, bsp_t **** grp){
	unsigned char bnu = bxy->bnu;
	unsigned char cct = bxy->cct;
	unsigned short * cbc = bxy->cbc;
	bsp_t *** p = * grp = (bsp_t ***)malloc(sizeof(bsp_t **) * bnu);
	bsp_t ** q = NULL;
	bsp_t * r = NULL;
	bsp_t * s = NULL;
	for(int i = 0; i < bnu; i++){
		q = * (p + i) = (bsp_t **)malloc(sizeof(bsp_t *) * cct);
		for(int j = 0; j < cct; j++){
			r = * (q + j) = (bsp_t *)malloc(sizeof(bsp_t) * cbc[j]);
			s = * (chr + j);
			for(int k = 0; k < cbc[j]; k++,r++,s++)
				* r = * s;
		}
	}
}
void ref(con_t * con, bxy_t ** bxy, unsigned char * cpu, unsigned char chk, unsigned int udt){
	//get bxy
	bxy_t * vvv = * bxy = get_bxy(con, udt);
	//
	vvv->spn = (pthread_spinlock_t *)malloc(sizeof(pthread_spinlock_t));
	vvv->bar = (pthread_barrier_t *)malloc(sizeof(pthread_barrier_t));
	//set check mode
	vvv->chk = chk;
	//get chr
	bsp_t ** chr = get_chr(con, vvv, udt);
	//generate group
	bsp_t *** grp = NULL, *** grq = NULL;
	grx(vvv, chr, &grp);
	grx(vvv, chr, &grq);
	//get tci
	stc_t * tci = get_tci(con, udt);
	//get tcc
	stc_t * tcc = get_tcc(con, udt);
	//get rid
	stc_t * rid = get_rid(con, udt);
	//get rci
	stc_t * rci = get_rci(con, udt);
	//get rcc
	stc_t * rcc = get_rcc(con, udt);
	//get general index for soft constraint, cts/cst/tcs/tsc/sct/stc
	stc_t * cts = get_xls(con, 9,  udt);
	stc_t * cst = get_xls(con, 10, udt);
	stc_t * tcs = get_xls(con, 11, udt);
	stc_t * tsc = get_xls(con, 12, udt);
	stc_t * sct = get_xls(con, 13, udt);
	stc_t * stc = get_xls(con, 14, udt);
	//get sft
	stc_t * sft = get_sft(con, udt);
	//get sim 
	stc_t * sim = get_sim(con, udt);
	//set to bxy
	vvv->grp = grp;
	vvv->grq = grq;
	vvv->tci = tci;
	vvv->tcc = tcc;
	vvv->rid = rid;
	vvv->rci = rci;
	vvv->rcc = rcc;
	vvv->cts = cts;
	vvv->cst = cst;
	vvv->tcs = tcs;
	vvv->tsc = tsc;
	vvv->sct = sct;
	vvv->stc = stc;
	vvv->sft = sft;
	vvv->sim = sim;
	vvv->xpb = 0.27183;
	unsigned int len = 0;
	for(int i = 0; i < vvv->cct; i++)
		len += * (vvv->cbc + i);
	vvv->ypb = 1.27183 / len;
	vvv->scr = (unsigned int *)calloc(sizeof(unsigned int), vvv->bnu);

	* cpu = sysconf(_SC_NPROCESSORS_ONLN);
	if(* cpu >= vvv->bnu)
		* cpu = vvv->bnu;
	unsigned char * cgb = (unsigned char *)malloc(sizeof(unsigned char) * (* cpu));
	unsigned char * cgc = (unsigned char *)malloc(sizeof(unsigned char) * (* cpu));
	unsigned char sum = 0, avg = vvv->bnu / (* cpu), lft = vvv->bnu % (* cpu);
	for(int i = 0; i < (* cpu); i++){
		* (cgc + i) = avg + (lft > i);
		* (cgb + i) = sum;
		sum += * (cgc + i);
	}
	//change index
	bxy_t * tmp = (bxy_t *)malloc(sizeof(bxy_t) * (* cpu));
	bxy_t * bak = tmp;
	for(int i = 0; i < (* cpu); i++,tmp++,cgb++,cgc++){
		* tmp = * vvv;
		tmp->cgb = * cgb;
		tmp->cgc = * cgc;
		tmp->grp += * cgb;
		tmp->grq += * cgb;
		tmp->scr += * cgb;
		tmp->hit = 0;
	}
	free(vvv);
	* bxy = bak;
	free(cgb - (* cpu));
	free(cgc - (* cpu));
}
void * mut(void * arg){
	bxy_t * bxy = (bxy_t *)arg;
	bsp_t *** grp = bxy->grp, * cls = NULL, tmp;
	rid_t * rid = (rid_t *)bxy->rid->bsc;
	rid_t * bak = rid;
	unsigned char cct = bxy->cct, cgc = bxy->cgc, crt = bxy->rid->cnt;
	unsigned short srd, sre, cbc;
	double ypb = bxy->ypb, psb;
	for(int k = 0; k < cgc; k++,grp++){
		for(int i = 0; i < cct; i++){
			cbc = * (bxy->cbc + i);
			psb = ypb * cbc * rnd(0, 21) / 10;
			if(rnd(0, 10000) < psb * 10000){
				cls = * ((* grp) + i);
				srd = rnd(0, cbc);
				sre = rnd(0, cbc);
				tmp = * (cls + srd);
				(cls + srd)->day = (cls + sre)->day;
				(cls + srd)->moa = (cls + sre)->moa;
				(cls + srd)->blk = (cls + sre)->blk;
				(cls + sre)->day = tmp.day;
				(cls + sre)->moa = tmp.moa;
				(cls + sre)->blk = tmp.blk;
			}
		}
		rid = bak;
		for(int i = 0; i < crt; i++,rid++){
			psb = ypb * rid->cnt * rnd(0, 21) / 10;
			if(rnd(0, 10000) < psb * 10000){
				cls = * ((* grp) + rid->cid - 1);
				srd = rnd(rid->bgn, rid->bgn + rid->cnt);
				(cls + srd)->rid = * (rid->rid + rnd(0, rid->rct));
			}
		}
	}
	return NULL;
}
void * crx(void * arg){
	bxy_t * bxy = (bxy_t *)arg;
	bsp_t *** grp = bxy->grp, ** chr = NULL, ** chs = NULL, * tmp = NULL;
	unsigned char cct = bxy->cct, cgb = bxy->cgb, cgc = bxy->cgc, srd;
	float xpb = bxy->xpb, psb;
	for(int k = 0; k < cgc; k++,grp++){
		psb = cct * rnd(0, 201) / 100 * xpb;
		CRX:
		if(rnd(0, 1000) < psb * 1000){
			srd = rnd(0, bxy->bnu);
			chr = * grp;
			chs = * (grp - k - cgb + srd);
			srd = rnd(0, cct);
			//lock two class, make exchange to atomic operation
			//why not mutex lock?
			//because operation under spinlock is easy and short
			//mutex will block the thread, and wake up thread also takes time
			pthread_spin_lock(bxy->spn);
			tmp = * (chr + srd);
			* (chr + srd) = * (chs + srd);
			* (chs + srd) = tmp;
			//unlock
			pthread_spin_unlock(bxy->spn);
			if(--psb > 0)//if expected cross times > 1
				goto CRX;
		}
	}
	return NULL;
}
void * (* spc())(void * arg){
	void * handle = NULL;
    	char * error = NULL;
	const char * LIB_PATH = "./cup/libdyn.so";
        handle = dlopen(LIB_PATH, RTLD_LAZY);
        if (!handle) {
		fprintf(stderr, "spc():%s\n", dlerror());
		return NULL;
	}
	fun func = dlsym(handle, "spc");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "spc():%s\n", error);
		return NULL;
	} else
		return func;
	return NULL;
}
void * sft(void * arg){
	bxy_t * bxy = (bxy_t *)arg;
	bsp_t *** grp = bxy->grp;
	bsp_t ** chr = NULL, * cls = NULL, * clt = NULL, * dmb = NULL;
	unsigned int nct = bxy->cts->cnt;
	unsigned int ncs = bxy->cst->cnt;
	unsigned int ntc = bxy->tcs->cnt;
	unsigned int nts = bxy->tsc->cnt;
	unsigned int nsc = bxy->sct->cnt;
	unsigned int nst = bxy->stc->cnt;
	unsigned int xct, snu, num, * scr = bxy->scr;
	unsigned char cgc = bxy->cgc, isc;	//C:0 T:1 S:2
	unsigned short * uls = NULL, * vls = NULL, * wls = NULL, uct, vct, wct;
	xls_t * cts = (xls_t *)bxy->cts->bsc;
	xls_t * cst = (xls_t *)bxy->cst->bsc;
	xls_t * tcs = (xls_t *)bxy->tcs->bsc;
	xls_t * tsc = (xls_t *)bxy->tsc->bsc;
	xls_t * sct = (xls_t *)bxy->sct->bsc;
	xls_t * stc = (xls_t *)bxy->stc->bsc;
	xls_t * xls = NULL;
	yls_t * yls = NULL;
	zls_t * zls = NULL;
	sft_t * sft = (sft_t *)bxy->sft->bsc;

	//group
	for(int i = 0; i < cgc; i++,grp++,scr++){
		* scr = 0;
		chr = * grp;
		sft = (sft_t *)bxy->sft->bsc;
		snu = bxy->sft->cnt;
		for(int j = 0; j < snu; j++,sft++){
			dmb = sft->dmb;
			switch(sft->typ){
				case 0:	xls = cts;
					xct = nct;
					isc = 0;
					break;
				case 1:	xls = cst;
					xct = ncs;
					isc = 0;
					break;
				case 2:	xls = tcs;
					xct = ntc;
					isc = 1;
					break;
				case 3:	xls = tsc;
					xct = nts;
					isc = 2;
					break;
				case 4:	xls = sct;
					xct = nsc;
					isc = 1;
					break;
				case 5:	xls = stc;
					xct = nst;
					isc = 2;
					break;
				default:xls = cts;
					xct = nct;
					isc = 0;
					break;
			}
			num = 0;
			unsigned int arr[sft->dct];
			for(int x = 0; x < sft->dct; x++)
				arr[x] = 0;
			//0
			uct = sft->uct;
			uls = sft->uls;
			for(int x = 0; x < xct; x++,xls++){
				if(sft->uct == 0 || * uls == xls->xid){
					if(isc == 0) clt = * (chr + xls->xid - 1);
					yls = xls->yls;
					//1
					vct = sft->vct;
					vls = sft->vls;
					for(int y = 0; y < xls->cnt; y++,yls++){
						if(sft->vct == 0 || * vls == yls->yid){
							if(isc == 1) clt = * (chr + yls->yid - 1);
							zls = yls->zls;
							//2
							wct = sft->wct;
							wls = sft->wls;
							for(int z = 0; z < yls->cnt; z++,zls++){
								if(sft->wct == 0 || * wls == zls->zid){
									if(isc == 2) clt = * (chr + zls->zid - 1);
									cls = clt + zls->bgn;
									//3
									// - ****************** compare ******************
									for(int u = 0; u < zls->cnt; u++,cls++){
										dmb = sft->dmb;
										for(int v = 0; v < sft->dct; v++,dmb++)
											if(	(dmb->day == 0 || dmb->day == cls->day) &&
												(dmb->moa == 0 || dmb->moa == cls->moa) &&
												(dmb->blk == 0 || dmb->blk == cls->blk)
											){
												arr[v]++;
												//the elements of dmb list should be independent to each other
												//for example: 11:0:0,11:2:0 will cause problems, because the cls->day:11
												//matchs 11:0:0 first, always. so, 11:2:0 will never be used/matched.
												break;//IMPORTANT!!!
											}
									}
									// - ****************** compare ******************
									//3
									if(sft->wct){
										if(--wct == 0)
											break;
										wls++;
									}
								}
							}
							//2
							if(sft->vct){
								if(--vct == 0)
									break;
								vls++;
							}
						}
					}
					//1
					if(sft->uct){
						if(--uct == 0)
							break;
						uls++;
					}
				}
			}
			if(sft->dtp == 0){
				for(int x = 0; x < sft->dct; x++){
					if(sft->opt == 0 && arr[x] <= sft->num){
						* scr += (5 - sft->lvl) * (sft->num - arr[x] + 1);
						_INF_
					}else if(sft->opt == 1 && arr[x] >= sft->num){
						* scr += (5 - sft->lvl) * (arr[x] - sft->num + 1);
						_INF_
					}else if(sft->opt == 2 && arr[x] != sft->num){
						* scr += (5 - sft->lvl) * abs(arr[x] - sft->num);
						_INF_
					}else if(sft->opt == 3 && arr[x] == sft->num){
						* scr += (5 - sft->lvl);
						_INF_
					}
				}		
			}else{
				for(int x = 0; x < sft->dct; x++)
					num += arr[x];
				if(sft->opt == 0 && num <= sft->num){
					* scr += (5 - sft->lvl) * (sft->num - num + 1);
					_INF_
				}else if(sft->opt == 1 && num >= sft->num){
					* scr += (5 - sft->lvl) * (num - sft->num + 1);
					_INF_
				}else if(sft->opt == 2 && num != sft->num){
					* scr += (5 - sft->lvl) * abs(num - sft->num);
					_INF_
				}else if(sft->opt == 3 && num == sft->num){
					* scr += (5 - sft->lvl);
					_INF_
				}
			}
			//0
		}
	}
	return NULL;
}
void * hrd(void * arg){
	bxy_t * bxy = (bxy_t *)arg;
	bsp_t *** grp = bxy->grp;
	bsp_t ** chr = NULL, * cls = NULL;
	tci_t * tci = NULL;
	cbg_t * cbg = NULL;
	tcc_t * tcc = NULL;
	cmb_t * cmb = NULL;
	cab_t * cab = NULL;
	rci_t * rci = NULL;
	rcc_t * rcc = NULL;
	ccc_t * ccc = NULL;
	bac_t * bac = NULL;
	unsigned int   cnt = bxy->tci->cnt, cnu = bxy->tcc->cnt, rct = bxy->rci->cnt, mct = bxy->rcc->cnt, * scr = bxy->scr;
	unsigned char  cgc = bxy->cgc;
	unsigned char  mdl = bxy->mdl, idy, ldy;
	unsigned char  mml = bxy->mml, ima, lma;
	unsigned char  mbl = bxy->mbl, ibk, lbk;
	unsigned char  ctn[mml][mbl][mdl];
	unsigned short * rid = NULL;
	for(int i = 0; i < mml; i++)
		for(int j = 0; j < mbl; j++)
			for(int k = 0; k < mdl; k++)
				ctn[i][j][k] = 0;
	for(int i = 0; i < cgc; i++,grp++,scr++){
		chr = * grp;
		//teacher
		//general time can not be same
		tci = (tci_t *)bxy->tci->bsc;
		for(int j = 0; j < cnt; j++,tci++){
			cbg = tci->cbg;
			ima = tci->ima;
			ibk = tci->ibk;
			idy = tci->idy;
			lma = tci->lma;
			lbk = tci->lbk;
			ldy = tci->ldy;
			for(int k = 0; k < tci->cnt; k++,cbg++){
				cls = (* (chr + cbg->cid - 1)) + cbg->bgn;
				for(int l = 0; l < cbg->cnt; l++,cls++)
					ctn[cls->moa - ima][cls->blk - ibk][cls->day - idy]++;
			}
			for(int x = 0; x < lma; x++){
				for(int y = 0; y < lbk; y++){
					for(int z = 0; z < ldy; z++){
						if(ctn[x][y][z] > 1)
							* scr += ctn[x][y][z] * (ctn[x][y][z] - 1) * 5;
						ctn[x][y][z] = 0;
					}
				}
			}
		}
		//combined class
		//combined time should be same
		tcc = (tcc_t *)bxy->tcc->bsc;
		for(int j = 0; j < cnu; j++,tcc++){
			cmb = tcc->cmb;
			ima = tcc->ima;
			ibk = tcc->ibk;
			idy = tcc->idy;
			lma = tcc->lma;
			lbk = tcc->lbk;
			ldy = tcc->ldy;
			for(int k = 0; k < tcc->cnt; k++,cmb++){
				cab = cmb->cab;
				* scr += cmb->cnt * (cmb->cnt - 1) * cmb->scr * 5;
				for(int l = 0; l < cmb->cnt; l++,cab++){
					cls = (* (chr + cab->cid - 1)) + cab->bgn;
					for(int m = 0; m < cmb->scr; m++,cls++)
						ctn[cls->moa - ima][cls->blk - ibk][cls->day - idy]++;
				}
			}
			for(int x = 0; x < lma; x++){
				for(int y = 0; y < lbk; y++){
					for(int z = 0; z < ldy; z++){
						if(ctn[x][y][z] > 1)
							* scr -= ctn[x][y][z] * (ctn[x][y][z] - 1) * 10;
						ctn[x][y][z] = 0;
					}
				}
			}
		}
#ifdef _DBG_
		_PRT_(" - tch can't be occupied by two classes at the same time,except combined classes - \033[7;31;40m%9u\033[0m\n", * scr)
#else
		_PRT_("-1:%u,", * scr)
#endif
		//room
		//general room can not be same
		rci = (rci_t *)bxy->rci->bsc;
		for(int j = 0; j < rct; j++,rci++){
			ccc = rci->ccc;
			ima = rci->ima;
			ibk = rci->ibk;
			idy = rci->idy;
			lma = rci->lma;
			lbk = rci->lbk;
			ldy = rci->ldy;
			for(int k = 0; k < rci->cnt; k++,ccc++){
				bac = ccc->bac;
				for(int l = 0; l < ccc->cnt; l++,bac++){
					cls = (* (chr + ccc->cid - 1)) + bac->bgn;
					for(int m = 0; m < bac->cnt; m++,cls++)
						if(cls->rid == rci->rid)
							ctn[cls->moa - ima][cls->blk - ibk][cls->day - idy]++;
				}
			}
			for(int x = 0; x < lma; x++){
				for(int y = 0; y < lbk; y++){
					for(int z = 0; z < ldy; z++){
						if(ctn[x][y][z] > 1)
							* scr += ctn[x][y][z] * (ctn[x][y][z] - 1) * 5;
						ctn[x][y][z] = 0;
					}
				}
			}
		}
		//combined class
		//combined room should be same
		rcc = (rcc_t *)bxy->rcc->bsc;
		for(int j = 0; j < mct; j++,rcc++){
			rid = rcc->rid;
			ima = rcc->ima;
			ibk = rcc->ibk;
			idy = rcc->idy;
			lma = rcc->lma;
			lbk = rcc->lbk;
			ldy = rcc->ldy;
			* scr += rcc->cnt * (rcc->cnt - 1) * rcc->scr * 5;
			for(int k = 0; k < rcc->rct; k++,rid++){
				cab = rcc->cab;
				for(int l = 0; l < rcc->cnt; l++,cab++){
					cls = (* (chr + cab->cid - 1)) + cab->bgn;
					for(int m = 0; m < rcc->scr; m++,cls++)
						if(cls->rid == * rid)
							ctn[cls->moa - ima][cls->blk - ibk][cls->day - idy]++;
				}
				for(int x = 0; x < lma; x++){
					for(int y = 0; y < lbk; y++){
						for(int z = 0; z < ldy; z++){
							if(ctn[x][y][z] > 1)
								* scr -= ctn[x][y][z] * (ctn[x][y][z] - 1) * 10;
							ctn[x][y][z] = 0;
						}
					}
				}
			}
		}
#ifdef _DBG_
		_PRT_(" - rom can't be occupied by two classes at the same time,except combined classes - \033[7;31;40m%9u\033[0m\n", * scr)
#else
		_PRT_("-2:%u", * scr)
#endif
		//if hit score
		if((* scr) <= 0){
			bxy->hit = bxy->cgb + i + 1;
			break;
		}
	}
	return NULL;
}
void * gen(void * arg){
	bxy_t * bxy = (bxy_t *)arg;
	unsigned char bnu = bxy->bnu, cct = bxy->cct, cgb = bxy->cgb, cgc = bxy->cgc;
	unsigned short * cbc = NULL;
	unsigned int * scr = bxy->scr - cgb, fst, snd, srd, sre;
	bsp_t *** grp = bxy->grp - cgb, *** grq = bxy->grq, *** tmp = NULL;
	bsp_t ** chr = NULL, ** chs = NULL;
	bsp_t * cls = NULL, * clt = NULL;

	for(int k = 0; k < cgc; k++,grq++){
		srd = rnd(0, bnu);
		sre = rnd(0, bnu);
		fst = * (scr + srd);
		snd = * (scr + sre);
		if(fst > snd)
			srd = sre;
		chr = * (grp + srd);
		chs = * grq;
		cbc = bxy->cbc;
		for(int i = 0; i < cct; i++,chr++,chs++,cbc++){
			cls = * chr;
			clt = * chs;
			for(int j = 0; j < * cbc; j++,cls++,clt++)
				* clt = * cls;
		}
	}
	//wait for all threads
	//pthread_barrier_wait(bxy->bar);//do we really need this?
	tmp = bxy->grp;
	bxy->grp = bxy->grq;
	bxy->grq = tmp;
	return NULL;
}
void * cbm(void * arg){
	gen(arg);//generate new group
	mut(arg);//mutation
	crx(arg);//cross
	/* 
	 - without barrier, hrd in thread 1 may execute before crx in thread 2.
	 - for example:
	 - if hrd gives hit a positive value, eg., hit = 13 before crx.
	 - it means we find the chromesome we want, when the crx is going.
	 - we get the right chromesome, but finally it changed by crx, so we get
	 - the wrong result finally.
	*/
	pthread_barrier_wait(((bxy_t *)arg)->bar);
	/*
	 - please note that, any others constraint must executed before hard constraint and after soft constraint in one cycle
	 - so, as you see, dyn is stand above hrd and below sft
	 - the first constraint should clear score every cycle, like '* scr = 0'; as this case, sft did in clude this sentence
	 - do not forget to remove '* scr = 0' in other constraints
	*/
	sft(arg);//soft constraint
	dyn(arg);//special constraint
	hrd(arg);//hard constraint
	//pthread_barrier_wait(&bar);//do we really need this ????
	return NULL;
}
void tdb(con_t * con, bxy_t * bxy, unsigned char hit, unsigned char udt){
	bsp_t ** chr = * (bxy->grp + hit);
	bsp_t * cls = NULL;
	unsigned char cct = bxy->cct;
	unsigned short * cbc = bxy->cbc;
	unsigned int idt = 1;
	for(int i = 0; i < cct; i++,cbc++,chr++){
		cls = * chr;
		for(int j = 0; j < * cbc; j++,cls++)
			set_res(con, * cls, udt, idt++);
	}
	//set status fineshed
	set_stt(con, 2, udt);
}
void del(bxy_t * bxy){
	bsp_t *** grp = bxy->grp, *** grq = bxy->grq;
	bsp_t ** chr, ** chs = NULL;
	bsp_t * cls = NULL, * clt = NULL;
	for(int i = 0; i < bxy->bnu; i++){
		chr = * (grp + i);
		chs = * (grq + i);
		for(int j = 0; j < bxy->cct; j++){
			cls = * (chr + j);
			clt = * (chs + j);
			free(cls);
			free(clt);
		}
		free(chr);
		free(chs);
	}
	free(bxy->grp);	//grp
	free(bxy->grq);	//grq

	tci_t * tci = (tci_t *)bxy->tci->bsc;
	for(int i = 0; i < bxy->tci->cnt; i++)
		free((tci + i)->cbg);
	free(tci);
	free(bxy->tci);	//tci
	
	tcc_t * tcc = (tcc_t *)bxy->tcc->bsc;
	cmb_t * cmb = NULL;
	for(int i = 0; i < bxy->tcc->cnt; i++){
		cmb = (tcc + i)->cmb;
		for(int j = 0; j < (tcc + i)->cnt; j++)
			free((cmb + j)->cab);
		free(cmb);
	}
	free(tcc);
	free(bxy->tcc);	//tcc

	rid_t * rid = (rid_t *)bxy->rid->bsc;
	for(int i = 0; i < bxy->rid->cnt; i++)
		free((rid + i)->rid);
	free(rid);
	free(bxy->rid);	//rid

	rci_t * rci = (rci_t *)bxy->rci->bsc;
	ccc_t * ccc = NULL;
	for(int i = 0; i < bxy->rci->cnt; i++){
		ccc = (rci + i)->ccc;
		for(int j = 0; j < (rci + i)->cnt; j++)
			free((ccc + j)->bac);
		free(ccc);
	}
	free(rci);
	free(bxy->rci);	//rci

	rcc_t * rcc = (rcc_t *)bxy->rcc->bsc;
	for(int i = 0; i < bxy->rcc->cnt; i++)
		free(rcc->cab);
	free(rcc);
	free(bxy->rcc);	//rcc

	xls_t * cts = (xls_t *)bxy->cts->bsc;
	yls_t * yls = NULL;
	for(int i = 0; i < bxy->cts->cnt; i++){
		yls = (cts + i)->yls;
		for(int j = 0; j < (cts + i)->cnt; j++)
			free((yls + j)->zls);
		free(yls);
	}
	free(cts);
	free(bxy->cts);	//cts

	xls_t * cst = (xls_t *)bxy->cst->bsc;
	for(int i = 0; i < bxy->cst->cnt; i++){
		yls = (cst + i)->yls;
		for(int j = 0; j < (cst + i)->cnt; j++)
			free((yls + j)->zls);
		free(yls);
	}
	free(cst);
	free(bxy->cst);	//cst

	xls_t * tcs = (xls_t *)bxy->tcs->bsc;
	for(int i = 0; i < bxy->tcs->cnt; i++){
		yls = (tcs + i)->yls;
		for(int j = 0; j < (tcs + i)->cnt; j++)
			free((yls + j)->zls);
		free(yls);
	}
	free(tcs);
	free(bxy->tcs);	//tcs

	xls_t * tsc = (xls_t *)bxy->tsc->bsc;
	for(int i = 0; i < bxy->tsc->cnt; i++){
		yls = (tsc + i)->yls;
		for(int j = 0; j < (tsc + i)->cnt; j++)
			free((yls + j)->zls);
		free(yls);
	}
	free(tsc);
	free(bxy->tsc);	//tsc

	xls_t * sct = (xls_t *)bxy->sct->bsc;
	for(int i = 0; i < bxy->sct->cnt; i++){
		yls = (sct + i)->yls;
		for(int j = 0; j < (sct + i)->cnt; j++)
			free((yls + j)->zls);
		free(yls);
	}
	free(sct);
	free(bxy->sct);	//sct

	xls_t * stc = (xls_t *)bxy->stc->bsc;
	for(int i = 0; i < bxy->stc->cnt; i++){
		yls = (stc + i)->yls;
		for(int j = 0; j < (stc + i)->cnt; j++)
			free((yls + j)->zls);
		free(yls);
	}
	free(stc);
	free(bxy->stc);	//stc

	sft_t * sft = (sft_t *)bxy->sft->bsc;
	for(int i = 0; i < bxy->sft->cnt; i++){
		free((sft + i)->dmb);
		free((sft + i)->uls);
		free((sft + i)->vls);
		free((sft + i)->wls);
		free((sft + i)->stu);
		free((sft + i)->stv);
		free((sft + i)->stw);
		free((sft + i)->str);
	}
	free(sft);
	free(bxy->sft);	//sft

	free(bxy->cbc);	//cbc
	free(bxy->scr);	//scr
	free(bxy);	//bxy
}
/** core **/
void * cor(int arc, char * arv[]){
	//arv
	int gen = 0, ret = 2, udt = 1;
	unsigned char chk = 0;
	if(arc > 1){
		gen = atoi(arv[1]);
		if(gen < 1 || gen > 20000)
			gen = 0;
	}
	gen *= 100;
	if(arc > 2){
		ret = atoi(arv[2]);
		if(ret < 1 || ret > 20)
			ret = 1;
	}
	ret *= 100;
	if(arc > 3){
		if(atoi(arv[3]) == 1)
			chk = 1;
		else if(atoi(arv[3]) == 2)
			chk = 2;
		else if(atoi(arv[3]) == 3){
#ifdef _DBG_
			printf("\033[0;33;40mCONSOLE SysV VER%s.%s.%s / %s\033[0m\n", _VER_,_SUB_,_MDF_,_DAT_);
#endif
			return NULL;
		}
	}
	if(arc > 4){
		udt = atoi(arv[4]);
		if(udt < 1)
			udt = 1;
	}
	//variable
	con_t * con = NULL;
	bxy_t * bxy = NULL;
	unsigned char cpu = 0;
	unsigned int * scr = NULL, max, i;
	unsigned int hit = 0;
	//mysql connection
	cnx(&con);
	//configurations & indexes
	ref(con, &bxy, &cpu, chk, udt);
	//get special constraint function
	dyn = spc();
	//run
	if(chk == 1){
		_THD_
		unsigned char cgd = bxy->cgc;
		bxy->cgc = 1;
		bxy->ctt = (char *)malloc(MAX);
		strcpy(bxy->ctt, "");
		sft((void *)bxy);
		dyn((void *)bxy);
		hrd((void *)bxy);
		char * ctt = (char *)malloc(MAX);
#ifdef _DBG_
		strcpy(ctt, "the final score is ");
		strcat(ctt, utoa(* bxy->scr));
		strcat(ctt, " - for console mode");
#else
		strcpy(ctt, bxy->ctt);
#endif
		bxy->cgc = cgd;
		free(bxy->ctt);
		del(bxy);
		return (void *)ctt;
	}
	//init random seed
	sed();
#ifdef _DBG_
	struct timeval a, b;
	unsigned long int mtm = 0;
#endif
	pthread_t pid[cpu];
	pthread_barrier_init(bxy->bar, NULL, cpu);
	pthread_spin_init(bxy->spn, PTHREAD_PROCESS_PRIVATE);
	gen = gen > 0 ? gen : bxy->gen;
	for(i = 0; i < gen; i++){
		//cbm
		///////////////////////////////////////////////////////////////
		_GET_TIME_OF_DAY_(&a,0)
		for(int j = 0; j < cpu; j++) pthread_create(&pid[j], NULL, cbm, (void *)(bxy + j));
		for(int j = 0; j < cpu; j++) pthread_join(pid[j], NULL);
		_GET_TIME_OF_DAY_(&b,0)
#ifdef _DBG_
		mtm += _TIME_
#endif
		//if hit score
		///////////////////////////////////////////////////////////////
		for(int j = 0; j < cpu; j++){
			if((bxy + j)->hit > 0){//hit score
				hit = (bxy + j)->hit;
				max = * (bxy->scr + hit - 1);
				break;
			}
		}
		if(hit) break;
		//find max score
		///////////////////////////////////////////////////////////////
#ifdef _DBG_
		if((i + 1) % ret == 0){
			scr = bxy->scr;
			max = * scr++;
			for(int j = 1; j < bxy->bnu; j++,scr++)
				if(max > * scr)
					max = * scr;
			printf("%7d %8u\n", i + 1, max);
		}
#endif
		//re-init seed per ktimes
		///////////////////////////////////////////////////////////////
		if((i + 1) % 4096 == 0)
			sed();
	}
	pthread_barrier_destroy(bxy->bar);
	pthread_spin_destroy(bxy->spn);
	if(!hit){
		scr = bxy->scr;
		max = * scr++;
		for(int i = 1; i < bxy->bnu; i++,scr++)
			if(max >  * scr){
				max = * scr;
				hit = i;
			}
		hit++;
	}
#ifdef _DBG_
	printf("esp:%lfs avg:%lfs/ktims\n", 1.0 * mtm / 1000000, 1.0 * mtm / 1000 / i);
	printf("gen:%u hit:%hu scr:%u\n", i, hit - 1, max);
	//final testing for result data by hard constraint
	unsigned int k;
	for(k = 0; k < cpu; k++)
		if(hit - 1 >= (bxy + k)->cgb && hit - 1 < (bxy + k)->cgb + (bxy + k)->cgc)
			break;
	(bxy + k)->hit = 0;
	hrd((void *)(bxy + k));
	if((bxy + k)->hit){
		printf("checking constraint ........ / \033[1;32;40mOK!!!\033[0m\n");
		tdb(con, bxy, hit - 1, udt);
		printf("writing into database ...... / \033[1;32;40mOK!!!\033[0m\n");
	}else{
		printf("checking constraint ........ / \033[1;31;40mFAILED!!!\033[0m\ndo you want it saved(N/y)?:");
		char c;
		ETR:
		if(scanf("%c", &c)){
			if(c == 10)//if press enter key
				goto ETR;
			else if(c == 'y' || c == 'Y'){
				tdb(con, bxy, hit - 1, udt);
				printf("\033[1;32;40mSAVED!!!\033[0m\n");
			}
			else{
				set_stt(con, 0, udt);
				printf("\033[1;31;40mABORT!!!\033[0m\n");
			}
		}
	}
#else
	tdb(con, bxy, hit - 1, udt);
#endif

	//free memory
	del(bxy);
	//return
	return NULL;
}
/** core package **/
void * box(void * arg){
	arg_t * prm = (arg_t *)arg;
	return cor(prm->arc, prm->arv);
} 
/** socket tcp **/
void * tcp(void * arg){
	pthread_t pid;
	con_t * con = (con_t *)arg;
	int sfd, cfd, len;
	struct sockaddr_in ser;
	char buf[MAX], * hdr = NULL, * bdy = NULL, * res = NULL, * kvp = NULL, * key = NULL, * val = NULL;
	char usr[64], pwd[64], opt = 0, rus = 1;
	int arc = 5;
	char * arv[5];
	unsigned int stt = 0, udt = 0;
	arv[0] = (char *)malloc(10);
	arv[1] = (char *)malloc(10);
	arv[2] = (char *)malloc(10);
	arv[3] = (char *)malloc(10);
	arv[4] = (char *)malloc(64);
	res = (char *)malloc(sizeof(char) * MAX);
	if((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
		printf("create socket error:%s(errno:%d)\n", strerror(errno),errno);
		return NULL;
	}
	memset(&ser, 0, sizeof(ser));
	ser.sin_family = AF_INET;
	ser.sin_addr.s_addr = htonl(INADDR_ANY);
	ser.sin_port = htons(6179);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&rus, sizeof(rus));
	if(bind(sfd, (struct sockaddr *)&ser, sizeof(ser)) == -1){
		printf("bind socket error:%s(errno:%d)\n", strerror(errno),errno);
		return NULL;
	}
	if(listen(sfd, 10) == -1){
		printf("listen socket error:%s(errno:%d)\n", strerror(errno),errno);
		return NULL;
	}
	while(1){
		if((cfd = accept(sfd, (struct sockaddr *)NULL, NULL)) == -1){
			printf("accept socket error:%s(errno:%d)", strerror(errno),errno);
			continue;
		}
		len = recv(cfd, buf, MAX, 0);
		buf[len] = '\0';
		if(!(buf[0] == 'P' && buf[1] == 'O' && buf[2] == 'S' && buf[3] == 'T')){
			strcpy(res, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n-- POST ONLY, GET WAS NOT SUPPORTED! --\n");
			send(cfd, res, strlen(res), 0);
		}else{
			//find head
			hdr = strstr(buf, "Content-Length");
			if (hdr == NULL)
				continue;
			//find body 
			bdy = strstr(buf, "\r\n\r\n") + 4;
			if (bdy == NULL)
				continue;
			else{
				//decode body params
				while(bdy != NULL){
					kvp = strsep(&bdy, ",");
					key = strsep(&kvp, ":");
					val = strsep(&kvp, ":");
					if(strcmp(key, "usr") == 0)
						strcpy(usr, val);
					else if(strcmp(key, "pwd") == 0)
						strcpy(pwd, val);
					else if(strcmp(key, "opt") == 0)
						opt = atoi(val);
				}
				//work & return to client
				strcpy(res, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
				stt = get_usr(con, usr, pwd, &udt);
				if(stt == 2)
					strcat(res, "err:illegal user.");
				else if(stt == 3 && opt == 0)
					strcat(res, "err:you have no informations in task table.");
				else if(stt == 4 && opt == 0)
					strcat(res, "err:you already have a running task, abort.");
				else{
					strcpy(arv[0], "0");
					strcpy(arv[1], "0");
					strcpy(arv[2], "5");
					strcpy(arv[4], utoa(udt));
					if(opt == 0){
						strcpy(arv[3], "0");
						strcat(res, "suc:your schedual task is now running - on general mode.");
					}else if(opt == 1){
						strcpy(arv[3], "1");
					}else if(opt == 2){
						strcpy(arv[3], "2");
						strcat(res, "suc:your schedual task is now running - on preview mode.");
					}else if(opt == 3){
						strcpy(arv[3], "3");
						strcat(res, "suc:");
#ifdef _DBG_		
						strcat(res, "CONSOLE");
#else
						strcat(res, "RELEASE");
#endif
						strcat(res, " SysV VER");
						strcat(res, _VER_);
						strcat(res, ".");
						strcat(res, _SUB_);
						strcat(res, ".");
						strcat(res, _MDF_);
						strcat(res, " / ");
						strcat(res, _DAT_);
					}else{
						strcat(res, "err:operation type not supported.");
						close(cfd);
						continue;
					}
					//
					//
					arg_t prm;
					prm.arc = arc;
					prm.arv[0] = arv[0];
					prm.arv[1] = arv[1];
					prm.arv[2] = arv[2];
					prm.arv[3] = arv[3];
					prm.arv[4] = arv[4];

					if(opt == 1){
						char * ctt = box(&prm);
						strcat(res, ctt);
						free(ctt);
					}else{
						if(opt < 3)
							set_stt(con, 1, udt);
						pthread_create(&pid, NULL, box, (void *)(&prm));
#ifdef _DBG_
						/*
						 * if console mode was set, tcp accept only one task in one time.
						 * otherwise the console output will be mess up.
						 * think about two tasks output console lines at the same time -
						 * now comes the question - which line belongs to which task?
						*/
						pthread_join(pid, NULL);
#else				
						/*
						 * if console mode was not set, then it stands the release version.
						 * we need the program runs as fast as it can, so, let it fly.
						*/
						pthread_detach(pid);
#endif
					}
					//box((void *)(&prm));
					//cor(arc, arv);
				}
				strcat(res, "\n");
				send(cfd, res, strlen(res), 0);
			}
		}
		close(cfd);
	}
	close(sfd);
	free(res);
	free(arv[0]);
	free(arv[1]);
	free(arv[2]);
	free(arv[3]);
	free(arv[4]);
	return NULL;
}
int main(int arc, char * arv[]){
	con_t * con = NULL;
	cnx(&con);
	tcp(con);
	free(con);
	//cor(arc, arv);
}

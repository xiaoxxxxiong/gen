#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stc.h"
#include "mcr.h"

//simultaneity
void * sim(void * arg){
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
	unsigned char mdy = bxy->mdy;
	unsigned char mma = bxy->mma;
	unsigned char mbk = bxy->mbk;
	unsigned char lst[mma][mbk][mdy];
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
	sft_t * sft = (sft_t *)bxy->sim->bsc;

	//group
	for(int i = 0; i < cgc; i++,grp++,scr++){
		chr = * grp;
		sft = (sft_t *)bxy->sim->bsc;
		snu = bxy->sim->cnt;
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
			for(int x = 0; x < mma; x++)
				for(int y = 0; y < mbk; y++)
					for(int z = 0; z < mdy; z++)
						lst[x][y][z] = 0;
			
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
												lst[cls->moa - 1][cls->blk - 1][cls->day - 1]++;
												//arr[v]++;
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
			dmb = sft->dmb;
			for(int u = 0; u < sft->dct; u++,dmb++){
				for(int x = 0; x < mma; x++){
					if(dmb->moa == 0 || dmb->moa == x + 1){
						for(int y = 0; y < mbk; y++){
							if(dmb->blk == 0 || dmb->blk == y + 1){
								for(int z = 0; z < mdy; z++){
									if(dmb->day == 0 || dmb->day == z + 1){
										if(	(dmb->day == 0 || dmb->day == z + 1) &&
											(dmb->moa == 0 || dmb->moa == x + 1) &&
											(dmb->blk == 0 || dmb->blk == y + 1)
										)	if(lst[x][y][z] == sft->uct)
												arr[u]++;

										if(dmb->day)break;
									}
								}

								if(dmb->blk)break;
							}
						}
						
						if(dmb->moa)break;
					}
				}
			}
			if(sft->dtp == 0){
				for(int x = 0; x < sft->dct; x++){
					if(sft->opt == 4 && arr[x] <= sft->num){
						* scr += (5 - sft->lvl) * (sft->num - arr[x] + 1);
						_INF_
					}else if(sft->opt == 5 && arr[x] >= sft->num){
						* scr += (5 - sft->lvl) * (arr[x] - sft->num + 1);
						_INF_
					}else if(sft->opt == 6 && arr[x] != sft->num){
						* scr += (5 - sft->lvl) * abs(arr[x] - sft->num);
						_INF_
					}else if(sft->opt == 7 && arr[x] == sft->num){
						* scr += (5 - sft->lvl);
						_INF_
					}
				}		
			}else{
				for(int x = 0; x < sft->dct; x++)
					num += arr[x];
				if(sft->opt == 4 && num <= sft->num){
					* scr += (5 - sft->lvl) * (sft->num - num + 1);
						_INF_
				}else if(sft->opt == 5 && num >= sft->num){
					* scr += (5 - sft->lvl) * (num - sft->num + 1);
						_INF_
				}else if(sft->opt == 6 && num != sft->num){
					* scr += (5 - sft->lvl) * abs(num - sft->num);
						_INF_
				}else if(sft->opt == 7 && num == sft->num){
					* scr += (5 - sft->lvl);
						_INF_
				}
			}
			//0
		}
	}
	return NULL;
}

extern void * spc(void * arg){
	//simultaneity
	if(((bxy_t *)arg)->sim->cnt)
		sim(arg);
	return NULL;
}

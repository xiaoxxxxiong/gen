#ifdef _DBG_
#define _PRT_(fmt,arg...) if(bxy->chk==1)printf(fmt,##arg);
#define _THD_ \
	if(bxy->chk==1)\
		printf(" - typ uls               vls               wls               dtp   opt   num   lvl \033[7;32;40m      scr\033[0m dmb\n");
#define _INF_ \
	if(bxy->chk==1)\
		printf(" - %s %-14.14s%3s %-14.14s%3s %-14.14s%3s %3c %5c %5u %5u \033[7;33;40m%9u\033[0m %-100.100s%s\n",\
			sft->typ==0?"cts":(sft->typ==1?"cst":(sft->typ==2?"tcs":(sft->typ==3?"tsc":(sft->typ==4?"sct":"stc")))),\
			sft->stu,strlen(sft->stu)>14?"...":"",sft->stv,strlen(sft->stv)>14?"...":"",sft->stw,strlen(sft->stw)>14?"...":"",\
			sft->dtp?'A':'P',sft->opt%4==0?'G':(sft->opt%4==1?'L':(sft->opt%4==2?'E':'N')),\
			sft->num,sft->lvl,*scr,sft->str,strlen(sft->str)>100?"...":"");
#else
#define _PRT_(fmt,arg...) if(bxy->chk==1)sprintf(bxy->ctt+strlen(bxy->ctt),fmt,##arg);
#define _THD_ /**/
#define _INF_ if(bxy->chk==1)sprintf(bxy->ctt+strlen(bxy->ctt),"%u:%u,",sft->idt,*scr);
#endif

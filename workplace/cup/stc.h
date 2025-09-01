#ifndef _H_ST
#define _H_ST
//basic structure
typedef struct {
	void * bsc;		//general structure
	unsigned short cnt;	//count of bsc
} stc_t;
//mysql connection
typedef struct {
        char * ser;		//server url
        char * usr;		//user account
        char * pwd;		//password
        char * dbn;		//database name
        unsigned short prt;	//port
	char * msg;		//message
} con_t;
//base pair
typedef struct {
	unsigned char day;	//day index
	unsigned char moa;	//1:dawn 2:morning 3:afternoon 4:evening
	unsigned char blk;	//block index
	unsigned char rid;	//room id
} bsp_t;
//configuration
typedef struct {
	// - check mode -
	unsigned char chk:2;	//0:normal 1:check 2:preview
	// - base info -
	unsigned int gen;	//max generation times
	float xpb;		//cross probability
	double ypb;		//mutation probability
	unsigned char bnu;	//base group number
	unsigned char hit;	//hit chromesome index of group
	unsigned char cgb;	//cpu group begin
	unsigned char cgc;	//cpu group count
	unsigned char cct;	//class count
	unsigned char mdl;	//max day length of chrome
	unsigned char mml;	//max moa length of chrome
	unsigned char mbl;	//max blk length of chrome
	unsigned char mdy;	//max day of chrome
	unsigned char mma;	//max moa of chrome
	unsigned char mbk;	//max blk of chrome
	unsigned short * cbc;	//class base pair count
	unsigned int * scr;	//chromesome score - \LOCATED!!!
	char * ctt;		//check result content - used for checking info return to socket client
	pthread_spinlock_t * spn;//spin lock
	pthread_barrier_t * bar;//barrier
	// - group & mirror - \LOCATED!!! -
	bsp_t *** grp;		//base group
	bsp_t *** grq;		//base group mirror
	// - indexes -
	stc_t * tci;		//teacher-class indexes
	stc_t * tcc;		//teacher-combined-class indexes
	stc_t * rid;		//room index for mutation
	stc_t * rci;		//room_class index for constraint evaluation
	stc_t * rcc;		//room_combined_class index for constraint evaluation
	stc_t * cts;		//general index used for soft constraint
	stc_t * cst;		//general index used for soft constraint
	stc_t * tcs;		//general index used for soft constraint
	stc_t * tsc;		//general index used for soft constraint
	stc_t * sct;		//general index used for soft constraint
	stc_t * stc;		//general index used for soft constraint
	stc_t * sft;		//soft constraint
	stc_t * sim;		//special constraint
} bxy_t;
//class-begin-index-count
typedef struct {
	unsigned char cid;
	unsigned short bgn;
	unsigned short cnt;
} cbg_t;
//teacher->class indexes
typedef struct {
	unsigned char tid;
	unsigned char cnt;
	unsigned char ldy;
	unsigned char idy;	//min day value of chromesome
	unsigned char lma;
	unsigned char ima;	//min moa value of chromesome
	unsigned char lbk;
	unsigned char ibk;	//min blk value of chromesome
	cbg_t * cbg;
} tci_t;
//class & begin index
typedef struct {
	unsigned char cid;
	unsigned short bgn;
} cab_t;
//combined-class
typedef struct {
	unsigned short scr;
	unsigned char cnt;	//class count of this combined-class
	cab_t * cab;
} cmb_t;
//teacher->combined-class indexes
typedef struct {
	unsigned char tid;
	unsigned char cnt;	//combined-class count
	unsigned char ldy;
	unsigned char idy;	//min day value of chromesome
	unsigned char lma;
	unsigned char ima;	//min moa value of chromesome
	unsigned char lbk;
	unsigned char ibk;	//min blk value of chromesome
	cmb_t * cmb;
} tcc_t;
//room index used for mutation
typedef struct {
	unsigned short cid;
	unsigned short bgn;
	unsigned short cnt;
	unsigned short rct;
	unsigned short * rid;
} rid_t;
//room index used for hard constraint
typedef struct {
	unsigned short bgn;
	unsigned short cnt;
} bac_t;
typedef struct {
	unsigned short cid;
	unsigned short cnt;
	bac_t * bac;
} ccc_t;
typedef struct {
	unsigned short rid;
	unsigned short cnt;
	unsigned short ldy;
	unsigned short idy;	//min day value of chromesome
	unsigned char lma;
	unsigned char ima;	//min moa value of chromesome
	unsigned char lbk;
	unsigned char ibk;	//min blk value of chromesome
	ccc_t * ccc;
} rci_t;
typedef struct {
	unsigned char rct;
	unsigned short * rid;
	unsigned char cnt;
	unsigned char scr;
	unsigned short ldy;
	unsigned short idy;	//min day value of chromesome
	unsigned char lma;
	unsigned char ima;	//min moa value of chromesome
	unsigned char lbk;
	unsigned char ibk;	//min blk value of chromesome
	cab_t * cab;
} rcc_t;
//general index - used for soft constraint
typedef struct {
	unsigned short zid;
	unsigned short bgn;
	unsigned short cnt;
	unsigned char cmb;	//0:no 1:yes
} zls_t;
typedef struct {
	unsigned short yid;
	unsigned short cnt;	//count of zct
	unsigned short scr;	//total score of this yid
	zls_t * zls;
} yls_t;
typedef struct {
	unsigned short xid;
	unsigned short cnt;	//count of yct
	unsigned short scr;	//total score of this xid
	yls_t * yls;
} xls_t;
/* soft constraint */
typedef struct {
	bsp_t * dmb;		//time list
	unsigned short * uls;	//uid list
	unsigned short * vls;	//vid list
	unsigned short * wls;	//wid list
	unsigned short uct;	//uid list count
	unsigned short vct;	//vid list count
	unsigned short wct;	//wid list count
	unsigned short dct;	//time list count
	unsigned short num;	//opt num
	unsigned short idt;	//id of this row 2024-01-26
	unsigned char lvl;	//level
	unsigned char dtp:1;	//0:every 1:total
	unsigned char opt:4;	//general 0:> 1:< 2:= 3:<>; special 4:> 5:< 6:= 7:<>
	unsigned char typ:3;	//0:cts 1:cst 2:tcs 3:tsc 4:sct 5:stc
	char * stu;	//uls string - used for print unsatisfied information
	char * stv;	//vls string - used for print unsatisfied information
	char * stw;	//wls string - used for print unsatisfied information
	char * str;	//dmb string - used for print unsatisfied information
} sft_t;
/* dynamic link object function */
typedef void * (* fun)(void * arg);
typedef struct {
	int arc;
	char * arv[5];
} arg_t;
#endif

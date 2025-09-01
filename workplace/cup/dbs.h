#include <mysql/mysql.h>
#include "stc.h"
/*** convert unsigned int to string ***/
char * utoa (unsigned int num);
/*** get configuration ***/
bxy_t * get_bxy (con_t * con, unsigned int udt);
/*** get genomosome ***/
bsp_t ** get_chr (con_t * con, bxy_t * bxy, unsigned int udt);
/*** get tci ***/
stc_t * get_tci (con_t * con, unsigned int udt);
/*** get tcc ***/
stc_t * get_tcc (con_t * con, unsigned int udt);
/*** set result ***/
unsigned int set_res(con_t * con, bsp_t bsp, unsigned int udt, unsigned int idt);
/*** get room index ***/
stc_t * get_rid (con_t * con, unsigned int udt);
/*** get room_class index ***/
stc_t * get_rci (con_t * con, unsigned int udt);
/*** get room_combined_class index ***/
stc_t * get_rcc (con_t * con, unsigned int udt);
/*** get general index - used for soft constraint ***/
stc_t * get_xls (con_t * con, unsigned char opt, unsigned int udt);
/*** get soft constraint ***/
stc_t * get_sft (con_t * con, unsigned int udt);
/*** get special constraint ***/
stc_t * get_sim (con_t * con, unsigned int udt);
/*** check if user exists 0:yes 1:no ***/
unsigned int get_usr (con_t * con, char * ton, char * pwd, unsigned int * udt);
/*** set task status ***/
unsigned int set_stt (con_t * con, unsigned char stt, unsigned int udt);

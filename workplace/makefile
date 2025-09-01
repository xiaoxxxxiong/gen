DBG ?= g
CNS ?= y
PTH ?= /usr/local/run
TGT ?= run
CTT := $(shell cat dbs.conf)
ifeq ($(DBG),g)
	FLG += -g
else ifeq ($(DBG),O0)
	FLG += -O0
else ifeq ($(DBG),O1)
	FLG += -O1
else ifeq ($(DBG),O2)
	FLG += -O2
else ifeq ($(DBG),O3)
	FLG += -O3
else
	FLG += -O3
endif
ifeq ($(CNS),y)
	FLG += -D_DBG_
	TGT = run_cns
endif
$(TGT):cup/libdyn.so cup/dbs.o run.o
	gcc cup/dbs.o run.o -o $(TGT) -lmysqlclient -lpthread $(FLG) -Wall -ldl
run.o:run.c
	gcc -c run.c -o run.o $(FLG) -Wall
cup/dbs.o:cup/dbs.c
	gcc -c cup/dbs.c -o cup/dbs.o $(FLG) -Wall -lpthread
cup/libdyn.so:cup/dyn.c
	gcc -fPIC -shared cup/dyn.c -o cup/libdyn.so -ldl $(FLG) -Wall
#installsql:
#	$(foreach ROW, $(CTT), \
#		echo "$(ROW)" \
#	)
install:
	if [ -f run ];\
	then\
		if [ -d $(PTH) ];\
			then echo " -- $(PTH) exist";\
		else\
		  	mkdir -p $(PTH) $(PTH)/cup;\
		  	cp run $(PTH);\
			cp cup/libdyn.so $(PTH)/cup;\
			cp dbs.conf $(PTH)/dbs.conf;\
			cp run.sh /etc/init.d/run.sh;\
			echo " - installed";\
  		fi;\
	else\
		echo " -- console version will not be installed";\
	fi;
uninstall:
	rm -rf $(PTH);\
	rm -rf /etc/init.d/run.sh
	rm -rf /var/log/run-console.log
	rm -rf /var/log/run-error.log
clean:
	rm -rf cup/*.o cup/*.so *.o *.so run_cns run

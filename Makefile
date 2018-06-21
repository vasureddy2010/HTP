OBJS= main.o scanner.o procSerMsg.o procSockMsg.o serial.o usrintfc.o InvfileQ.o readcfg.o queueProc.o version.o invThread.o Modbus.o pi.o serverinterface.o RTCC.o  mysqlIns.o

CFLAGS += -g  -Werror-implicit-function-declaration `mysql_config --cflags`

smTrkCli_MFM: $(OBJS) 
	$(CC) $(OBJS) -lpthread -lm -o $@ `mysql_config --libs`

clean:
	rm *.o smTrkCli_MFM


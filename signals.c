/* ************************************************************************
*  file: signals.c , trapping of signals from Unix.       Part of DIKUMUD *
*  Usage : Signal Trapping.                                               *
*  Copyright (C) 1990, 1991 - see 'license.doc' for complete information. *
************************************************************************* */

#include <signal.h>
#include <stdio.h>
#include <sys/time.h>

#include "utils.h"

int checkpointing(void);
int shutdown_request(void);
int logsig(void);
int hupsig(void);

void signal_setup(void)
{
	struct itimerval itime;
	struct timeval interval;

	signal(SIGUSR2, shutdown_request);

	/* just to be on the safe side: */

	signal(SIGHUP, hupsig);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, hupsig);
	signal(SIGALRM, logsig);
	signal(SIGTERM, hupsig);

	/* set up the deadlock-protection */

	interval.tv_sec = 900;    /* 15 minutes */
	interval.tv_usec = 0;
	itime.it_interval = interval;
	itime.it_value = interval;
	setitimer(ITIMER_VIRTUAL, &itime, 0);
	signal(SIGVTALRM, checkpointing);
}



int checkpointing(void)
{
	extern int tics;

	if (!tics) /* tics == 0 : 15분 간격으로 불러서 0으로 만듬, 15분 동안 tics이 업데이트 안됐으면 Deadlock으로 판단 */
	{
		log("CHECKPOINT shutdown: tics not updated");
		abort();
	}
	else
		tics = 0;
}




int shutdown_request(void)
{
	extern int shutdown_num;

	log("Received USR2 - shutdown request");
	shutdown_num = 1;
}



/* kick out players etc */
int hupsig(void)
{
	extern int shutdown_num;

	log("Received SIGHUP, SIGINT, or SIGTERM. Shutting down");
	exit(0);   /* something more elegant should perhaps be substituted */
}



int logsig(void)
{
	log("Signal received. Ignoring.");
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include "csapp.h"
#include "csapp.h"

#define MAXARGS 128
#define MAXLINE 1024
#define MAX_BG_JOBS 69105

#ifndef _BUILTINS_H
#define _BUILTINS_H
int builtin_command(int argc, char **argv);
#endif

#ifndef _JOBS_H
#define _JOBS_H
typedef struct Job {
	unsigned jid;
	pid_t pid;
	int completed;
	int stopped;
	int notified;
	struct Job *last;
	struct Job *next;
	char cmdline[MAXLINE];
} Job;

Job *make_job(unsigned jid, pid_t pid, char *cmdline);
void remove_job(Job *job);
Job *job_with_jid(unsigned jid);
Job *job_with_pid(pid_t pid);
int mark_process_status(pid_t pid, int status);
void update_jobs();
void wait_for_job(Job *j);
extern Job *first_job;
#endif

Job *first_job = NULL;
Job *make_job(unsigned jid, pid_t pid, char *cmdline) {
	Job *job = malloc(sizeof(Job));
	if (!job) {
		fprintf(stderr, "unable to allocate memory for new job");
		return NULL;
	}
	job->jid = jid;
	job->pid = pid;
	job->completed = 0;
	job->stopped = 0;
	job->notified = 0;
	job->last = NULL;
	job->next = NULL;
	strcpy(job->cmdline, cmdline);
	if (!first_job) {
		first_job = job;
		return job;
	}
	/* add to list of bg jobs */
	Job *j = first_job;
	while (j->next)
		j = j->next;
	j->next = job;
	job->last = j;
	return job;
}

void remove_job(Job *job) {
	Job *j;
	for (j = first_job; j; j = j->next) {
		if (j->pid == job->pid) {
			if (!j->last && !j->next)
				first_job = NULL;
			else {
				if (!j->last)
					first_job = j->next;
				else 
					j->last->next = j->next;
				if (j->next)
					j->next->last = j->last;
			}
			free(job);
			return;
		}
	}
	fprintf(stderr, "Job %d not in bg job list\n", job->pid);
}

Job *job_with_jid(unsigned jid) {
	Job *j;
	for (j = first_job; j; j = j->next)
		if (j->jid == jid)
			return j;
	return NULL;
}

Job *job_with_pid(pid_t pid) {
	Job *j;
	for (j = first_job; j; j = j->next)
		if (j->pid == pid)
			return j;
	return NULL;
}

int mark_process_status(pid_t pid, int status) {
	Job *j;
	if (pid > 0) {
	/* Update the record for the process. */
		for (j = first_job; j; j = j->next) {
			if (j->pid == pid) {
				if (WIFSTOPPED(status)) {
					printf("job %d stopped\n", pid);
					j->stopped = 1;
				} else {
					j->completed = 1;
					if (WIFSIGNALED(status))
						fprintf(stderr, "Job %d terminated by signal: %s\n", j->pid, strsignal(WTERMSIG(status)));
					else
						printf("job %d exited normally\n", pid);
				}
				return 0;
			}
		}
		fprintf(stderr, "No child process %d\n", pid);
		return -1;
	}
	else if (pid == 0 || errno == ECHILD)
	/* No processes ready to report. */
		return -1;
	else {
	/* Other weird errors.  */
		perror("waitpid");
		return -1;
	}
}

void update_jobs() {
	pid_t pid;
	int status;
	do 
		pid = waitpid(-1, &status, WUNTRACED|WNOHANG);
	while (!mark_process_status(pid, status));
	Job *j;
	for (j = first_job; j; j = j->next) {
		if (j->completed)
			remove_job(j);
		else if (j->stopped && !j->notified)
			j->notified = 1;
	}
}

void eval(char *cmdline);
int parseline(char *buf, int *argc, char **argv);
void sigint_handler(int sig);
static pid_t cpid;

int command_jobs(int argc, char **argv);
int command_bg(int argc, char **argv);
int command_fg(int argc, char **argv);

/* If first arg is a builtin command, run it and return true */
int builtin_command(int argc, char **argv)
{
	if (!strcmp(argv[0], "quit")) /* quit command */
		exit(0);
	if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
		return 1;
	if (!strcmp(argv[0], "jobs"))
		return command_jobs(argc, argv);
	if (!strcmp(argv[0], "bg"))
		return command_bg(argc, argv);
	if (!strcmp(argv[0], "fg"))
		return command_fg(argc, argv);
	return 0;                     /* Not a builtin command */
}

int command_jobs(int argc, char **argv) {
	pid_t pid;
	int status;
	Job *j;
	for (j = first_job; j; j = j->next)
		printf("[%d] %d %s   %s", 
	j->jid, 
	j->pid, 
	j->stopped ? "Stopped" : "Running", 
	j->cmdline);
	return 1;
}

int command_bg(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: bg <job>\n");
		printf("    job can be either a process id or job id prefixed with %%\n");
		printf("Examples:\n");
		printf("    bg 12345  -  Runs job with process id 12345 in the background\n");
		printf("    bg %%5     -  Runs job with job id 5 in the background\n");
		return 1;
	}
	Job *j;
	if (argv[1][0] == '%') {
		unsigned jid = atoi(argv[1]+1);
		j = job_with_jid(jid);
	} else {
		pid_t pid = atoi(argv[1]);
		j = job_with_pid(pid);
	}
	if (!j) {
		fprintf(stderr, "No job with specified jid or pid\n");
		return 1;
	}
	Kill(-j->pid, SIGCONT);
	j->stopped = 0;
	return 1;
}

int command_fg(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: fg <job>\n");
		printf("    job can be either a process id or job id prefixed with %%\n");
		printf("Examples:\n");
		printf("    fg 69105  -  Runs job with process id 69105 in the foreground\n");
		printf("    fg %%3     -  Runs job with job id 3 in the foreground\n");
		return 1;
	}
	Job *j;
	if (argv[1][0] == '%') {
		unsigned jid = atoi(argv[1]+1);
		j = job_with_jid(jid);
		} else {
		pid_t pid = atoi(argv[1]);
		j = job_with_pid(pid);
	}
	if (!j) {
		fprintf(stderr, "No job with specified jid or pid\n");
		return 1;
	}
	Kill(-j->pid, SIGCONT);
	j->stopped = 0;
	// mark job as needing to continue in foreground
	wait_for_job(j);
	return 1;
}

int main()
{
	Signal(SIGINT, SIG_IGN);
	Signal(SIGTSTP, SIG_IGN);
	char cmdline[MAXLINE];
	printf("Parent pid: %d\n", getpid());
	while (1) {
		/* Read */
		printf("<> ");
		Fgets(cmdline, MAXLINE, stdin);
		if (feof(stdin))
			exit(0);
		/* Evaluate */
		eval(cmdline);
		/* Update job statuses */
		update_jobs();
	}
}

void sigint_handler(int sig) {
	if(!kill(-cpid, 0))
		Kill(-cpid, sig);
}

/* eval - Evaluate a command line */
void eval(char *cmdline)
{
	int argc;            /* argument count */
	char *argv[MAXARGS]; /* Argument list execve() */
	char modified_cmdline[MAXLINE];   /* Holds modified command line */
	int bg;              /* Should the job run in bg or fg? */
	static unsigned jid = 0; /* job id */
	strcpy(modified_cmdline, cmdline);
	bg = parseline(modified_cmdline, &argc, argv);
	if (argv[0] == NULL)
		return;  /* Ignore empty lines */
	/* if built in, execute directly */
	if (builtin_command(argc, argv))
		return;
	Signal(SIGINT, sigint_handler);
	Signal(SIGTSTP, sigint_handler);
	jid++;
	if ((cpid = Fork()) == 0) {   /* Child runs user job */
		setpgid(getpid(), 0);
		if (execve(argv[0], argv, environ) < 0) {
			printf("%s: Command not found.\n", argv[0]);
			exit(0);
		}
	} 
	Job *j = make_job(jid, cpid, cmdline); 
	/* Parent waits for foreground job to terminate */
	if (!bg)
		wait_for_job(j);
	else
		printf("[%d] %d   %s", j->jid, j->pid, j->cmdline);
	Signal(SIGINT, SIG_IGN);
	Signal(SIGTSTP, SIG_IGN);
	return;
}

void wait_for_job(Job *j) {
	int status;
	if (waitpid(j->pid, &status, WUNTRACED) > 0) {
		if (WIFSTOPPED(status)) {
			j->stopped = 1;
			printf(" Job %d stopped by signal: %s\n", j->pid, strsignal(WSTOPSIG(status)));
		} else {
			remove_job(j);
			if (WIFSIGNALED(status))
				printf(" Job %d terminated by signal: %s\n", j->pid, strsignal(WTERMSIG(status)));
			else if (!WIFEXITED(status))
				printf(" Job %d did not exit with a valid status\n", j->pid);
		}
	}
	else
		unix_error("waitfg: waitpid error");
}
   
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, int *arg_count, char **argv)
{
	char *delim;         /* Points to first space delimiter */
	int argc;            /* Number of args */
	int bg;              /* Background job? */
	buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
	while (*buf && (*buf == ' ')) /* Ignore leading spaces */
		buf++;
		/* Build the argv list */
	argc = 0;
	while ((delim = strchr(buf, ' '))) {
		argv[argc++] = buf;
		*delim = '\0';
		buf = delim + 1;
		while (*buf && (*buf == ' ')) /* Ignore spaces */
		buf++;
	}
	argv[argc] = NULL;
	if (argc == 0)  /* Ignore blank line */
		return 1;
	/* Should the job run in the background? */
	if ((bg = (*argv[argc-1] == '&')) != 0)
		argv[--argc] = NULL;
	*arg_count = argc;
	return bg;
}

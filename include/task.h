#ifndef __TASK_HEADER_INCLUDED__
#define __TASK_HEADER_INCLUDED__

#include <arch/task.h>
#include <mm.h>

#define TASK_RUNNING   0
#define TASK_READY     1
#define TASK_SLEEPING  2 

#define INIT_WAITQ(name) {(struct task *)&(name),(struct task *)&(name)}
struct waitq {
	struct task *next,*prev;
};

/* Task descriptor */
struct task {
	struct task	*next,*prev;
	uint32_t	state;
	struct thread	t;
	pid_t 		pid;
	int		preempt;

	/* Filesystem info */
	struct inode	*root;
	struct inode	*cwd;
};

/* Enable/disable pre-emption for the current task */
#define get_cpu(x) ((x)->preempt++)
#define put_cpu(x) ((x)->preempt--)

/* Wait-queue manipulation */
void sleep_on(struct waitq *);
void wake_up(struct waitq *);

/* Run-queue manupulation */
void task_to_runq(struct task *);

/* The scheduler */
void sched_init(void);
void sched(void);

#endif /* __TASK_HEADER_INCLUDED__ */
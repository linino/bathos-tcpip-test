/*
 * Copyright (c) dog hunter AG - Zug - CH
 * Public domain
 * Author: Davide Ciminaghi <ciminaghi@gnudd.com> 2016
 */
#include <bathos/bathos.h>
#include <bathos/event.h>
#include <bathos/pipe.h>
#include <bathos/shell.h>
#include <bathos/tcp-server-pipe.h>

/*
 * setup a remote shell
 */
declare_event(shell_connection_accepted);

static struct bathos_pipe *server_pipe;

static struct tcp_server_data sd = {
	.port = 1000,
	.accept_event = &event_name(shell_connection_accepted),
};

static void *test_periodic(void *arg)
{
	return NULL;
}

static int test_init(void *arg)
{
	printf("test init\n");

	server_pipe = pipe_open("tcp-server-main",
				BATHOS_MODE_INPUT|BATHOS_MODE_OUTPUT,
				&sd);
	if (!server_pipe) {
		printf("error opening tcp-server-main\n");
		return -1;
	}
	
	return 0;
}

static void handle_shell_connection_accepted(struct event_handler_data *ed)
{
	struct bathos_pipe *connection_pipe = ed->data;

	if (!connection_pipe) {
		printf("NO CONNECTION PIPE !\n");
		return;
	}
	/* Start the shell */
	trigger_event(&event_name(shell_start), connection_pipe);
}

/* FIXME: CONNECTION CLOSED EVENT */


declare_event_handler(shell_connection_accepted, NULL,
		      handle_shell_connection_accepted, NULL);


static void handle_shell_termination(struct event_handler_data *ed)
{
	struct bathos_pipe *connection_pipe = ed->data;

	if (!connection_pipe) {
		printf("NO CONNECTION PIPE !\n");
		return;
	}
	printf("SHELL TERMINATED, closing pipe %p\n", connection_pipe);
	pipe_close(connection_pipe);
}

/* FIXME: CONNECTION CLOSED EVENT */


declare_event_handler(shell_termination, NULL,
		      handle_shell_termination, NULL);

static struct bathos_task __task t_test = {
	.name = "test", .period = 5 * HZ,
	.job = test_periodic,
	.arg = NULL,
	.init = test_init,
	.release = 3,
};

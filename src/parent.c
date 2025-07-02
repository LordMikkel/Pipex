/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parent.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 03:44:01 by migarrid          #+#    #+#             */
/*   Updated: 2025/07/02 17:15:57 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

int	parent_process(t_pipex *px, char **av)
{
	int	i;
	int	status[2];

	if (pipe(px->pipe_fd) == ERROR)
		exit_error(ERR_PIPE, EXIT_FAILURE, px);
	i = 0;
	while (i < 2)
	{
		px->pid[i] = fork();
		if (px->pid[i] == -1)
			exit_error(ERR_FORK, EXIT_FAILURE, px);
		if (px->pid[i] == 0)
			child_process(px, av[i + px->cmd_start], i);
		i++;
	}
	close_pipes(px);
	waitpid(px->pid[0], &status[0], 0);
	waitpid(px->pid[1], &status[1], 0);
	if (WIFEXITED(status[1]))
		status[1] = WEXITSTATUS(status[1]);
	if (px->outfile < 0)
		status[1] = EXIT_FAILURE;
	return (status[1]);
}

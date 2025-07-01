/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parent.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 03:44:01 by migarrid          #+#    #+#             */
/*   Updated: 2025/07/02 01:16:44 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

int	parent_process(t_pipex *px, char **av)
{
	int	i;
	int	status;

	if (pipe(px->pipe_fd) == ERROR)
		exit_error(ERR_PIPE, EXIT_FAILURE, px);
	i = 0;
	while(i < 2)
	{
		px->pid[i] = fork();
		if (px->pid[i] == -1)
			exit_error(ERR_FORK, EXIT_FAILURE, px);
		if (px->pid[i] == 0)
			child_process(px, av[i + px->cmd_start], i);
		i++;
	}
	close_pipes(px);
	waitpid(px->pid[1], &status, 0);
	if (WIFEXITED(status))
		status = WEXITSTATUS(status);
	if (px->outfile < 0)
		status = EXIT_FAILURE;
	return (status);
}

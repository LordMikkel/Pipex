/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parent_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 03:44:01 by migarrid          #+#    #+#             */
/*   Updated: 2025/07/02 17:58:06 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/pipex_bonus.h"

int	wait_all(t_pipex *px, int childs)
{
	int	i;
	int	status;

	i = 0;
	while (i < childs)
	{
		waitpid(px->pids[i], &status, 0);
		if (i == px->cmd_count - 1)
		{
			if (WIFEXITED(status))
				status = WEXITSTATUS(status);
		}
		i++;
	}
	if (px->outfile < 0)
		status = EXIT_FAILURE;
	return (status);
}

void	create_pipes(t_pipex *px)
{
	int	i;

	i = 0;
	px->pipes_fd = ft_calloc(px->pipes_count, sizeof(int *));
	if (!px->pipes_fd)
		exit_error(ERR_MEM, EXIT_FAILURE, px);
	while (i < px->pipes_count)
	{
		px->pipes_fd[i] = ft_calloc(2, sizeof(int));
		if (!px->pipes_fd[i])
			exit_error(ERR_MEM, EXIT_FAILURE, px);
		if (pipe(px->pipes_fd[i]) == ERROR)
			exit_error(ERR_PIPE, EXIT_FAILURE, px);
		i++;
	}
}

int	parent_process(t_pipex *px, char **av)
{
	int		status;
	int		i;

	i = 0;
	create_pipes(px);
	while (i < px->cmd_count)
	{
		px->pids[i] = fork();
		if (px->pids[i] == -1)
			exit_error(ERR_FORK, EXIT_FAILURE, px);
		if (px->pids[i] == 0)
			child_process(px, av[px->cmd_start + i], i);
		i++;
	}
	close_pipes(px);
	status = wait_all(px, px->cmd_count);
	free(px->pids);
	free_pipes(px);
	return (status);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 03:44:36 by migarrid          #+#    #+#             */
/*   Updated: 2025/07/02 17:30:53 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

void	execute(t_pipex *px, char *args, char **envp)
{
	char	**cmd;
	char	*path;

	cmd = ft_split(args, ' ');
	if (!cmd || !cmd[0] || !cmd[0][0])
	{
		ft_printf_fd(STDERR, ERR_CMD_N);
		ft_free_str_array(cmd);
		exit_error(ERR_MEM, EXIT_CMD, px);
	}
	path = get_path(cmd[0], envp);
	if (!path)
	{
		execve(cmd[0], cmd, envp);
		ft_printf_fd(STDERR, ERR_CMD, cmd[0]);
		ft_free_str_array(cmd);
		exit_error(NULL, EXIT_CMD, px);
	}
	execve(path, cmd, envp);
	if (ERROR_EXECVE)
	{
		free(path);
		ft_free_str_array(cmd);
		exit_error(ERR_EXEC, EXIT_FAILURE, px);
	}
}

static void	setup_stdin(t_pipex *px, int i)
{
	if (i == 0)
	{
		if (px->infile >= 0)
		{
			dup2(px->infile, STDIN_FILENO);
			close(px->infile);
		}
		else
			close(STDIN_FILENO);
	}
	else if (i == 1)
	{
		dup2(px->pipe_fd[0], STDIN_FILENO);
	}
}

static void	setup_stdout(t_pipex *px, int i)
{
	if (i == 0)
	{
		dup2(px->pipe_fd[1], STDOUT_FILENO);
	}
	else if (i == 1)
	{
		if (px->outfile >= 0)
		{
			dup2(px->outfile, STDOUT_FILENO);
			close(px->outfile);
		}
	}
}

void	child_process(t_pipex *px, char *cmd, int i)
{
	setup_stdin(px, i);
	setup_stdout(px, i);
	close_pipes(px);
	execute(px, cmd, px->envp);
}

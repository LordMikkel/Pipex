/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 19:26:13 by migarrid          #+#    #+#             */
/*   Updated: 2025/07/02 01:54:26 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/pipex_bonus.h"

void	init_here_doc(t_pipex *px, int ac, char **av, char **envp)
{
	ft_bzero(px, sizeof(t_pipex));
	px->envp = envp;
	px->here_doc = TRUE;
	px->limiter = av[2];
	px->cmd_start = 3;
	px->infile = -1;
	px->outfile = open_file(av[ac - 1], APPEND);
	px->cmd_count = ac - px->cmd_start - OUTFILE;
	px->pipes_count = px->cmd_count - OUTFILE;
	px->pids = ft_calloc(px->cmd_count, sizeof(int));
	if (!px->pids)
		exit_error(ERR_MEM, EXIT_FAILURE, px);
}

void	init_pipex(t_pipex *px, int ac, char **av, char **envp)
{
	ft_bzero(px, sizeof(t_pipex));
	px->envp = envp;
	px->cmd_start = 2;
	px->infile = open_file(av[1], READ_ONLY);
	px->outfile = open_file(av[ac - 1], TRUNCATE);
	px->cmd_count = ac - px->cmd_start - OUTFILE;
	px->pipes_count = px->cmd_count - OUTFILE;
	px->pids = ft_calloc(px->cmd_count, sizeof(int));
	if (!px->pids)
		exit_error(ERR_MEM, EXIT_FAILURE, px);
}

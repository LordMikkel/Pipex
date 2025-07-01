/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 19:26:13 by migarrid          #+#    #+#             */
/*   Updated: 2025/07/01 21:09:31 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

void	init_pipex(t_pipex *px, int ac, char **av, char **envp)
{
	ft_bzero(px, sizeof(t_pipex));
	px->envp = envp;
	px->cmd[0] = 2;
	px->cmd[1] = 3;
	px->infile = open_file(av[1], READ_ONLY);
	px->outfile = open_file(av[ac - 1], TRUNCATE);
}

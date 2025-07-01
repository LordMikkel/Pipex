/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 15:43:53 by migarrid          #+#    #+#             */
/*   Updated: 2025/07/01 20:26:09 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex_bonus.h"

void	free_pipes(t_pipex *px)
{
	int	i;

	i = 0;
	while (i < px->pipes_count)
	{
		if (px->pipes[i])
			free(px->pipes[i]);
		i++;
	}
	free(px->pipes);
}

void	close_pipes(t_pipex *px)
{
	int	i;

	i = 0;
	while (i < px->pipes_count)
	{
		if (px->pipes[i])
		{
			if (px->pipes[i][0] >= 0)
			{
				close(px->pipes[i][0]);
				px->pipes[i][0] = -1;
			}
			if (px->pipes[i][1] >= 0)
			{
				close(px->pipes[i][1]);
				px->pipes[i][1] = -1;
			}
		}
		i++;
	}
}

void	clean_pipes(t_pipex *px)
{
	if (px->pipes && px->pipes_count)
	{
		close_pipes(px);
		free_pipes(px);
	}
	if (px->pids)
		free(px->pids);
}

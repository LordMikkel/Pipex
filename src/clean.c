/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 15:43:53 by migarrid          #+#    #+#             */
/*   Updated: 2025/07/02 16:53:18 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

void	close_pipes(t_pipex *px)
{
	if (px->pipe_fd[0] >= 0)
	{
		close(px->pipe_fd[0]);
		px->pipe_fd[0] = -1;
	}
	if (px->pipe_fd[1] >= 0)
	{
		close(px->pipe_fd[1]);
		px->pipe_fd[1] = -1;
	}
}

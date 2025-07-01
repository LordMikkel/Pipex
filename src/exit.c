/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 19:26:47 by migarrid          #+#    #+#             */
/*   Updated: 2025/07/02 00:52:07 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

int	exit_error(char *error, int type, t_pipex *px)
{
	if (px && px->pipe_fd)
		close_pipes(px);
	if (error)
		ft_printf_fd(STDERR, error);
	if (errno && type != EXIT_CMD)
		ft_printf_fd(STDERR, ERRNO, strerror(errno));
	exit(type);
}

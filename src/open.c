/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   open.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 22:49:27 by migarrid          #+#    #+#             */
/*   Updated: 2025/07/01 04:16:44 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

int	open_file(char *file, int mode)
{
	int	fd;

	if (mode == APPEND)
		fd = open(file, O_WRONLY | O_CREAT | O_APPEND, FILE_PERMS);
	else if (mode == TRUNCATE)
		fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, FILE_PERMS);
	else
		fd = open(file, O_RDONLY);
	if (fd < 0)
	{
		if (mode == READ_ONLY)
			ft_printf_fd(STDERR, ERR_FILE, file);
		else
			ft_printf_fd(STDERR, ERR_PERMS, file);
		return (-1);
	}
	return (fd);
}

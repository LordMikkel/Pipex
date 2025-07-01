/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc_bonus.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 16:38:36 by migarrid          #+#    #+#             */
/*   Updated: 2025/07/02 01:41:36 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/pipex_bonus.h"

void	here_doc_mode(t_pipex *px, int ac, char **av, char **envp)
{
	int		fd[2];
	char	*line;

	init_here_doc(px, ac, av, envp);
	if (pipe(fd) == ERROR)
		exit_error(ERR_PIPE, EXIT_FAILURE, px);
	ft_printf_fd(STDERR, "heredoc> ");
	while (1)
	{
		line = get_next_line(STDIN);
		if (!line)
			break ;
		if (ft_strncmp(line, px->limiter, ft_strlen(px->limiter)) == 0
			&& line[ft_strlen(px->limiter)] == '\n')
		{
			free(line);
			break ;
		}
		ft_printf_fd(fd[1], line);
		free(line);
		ft_printf_fd(STDERR, "heredoc> ");
	}
	close(fd[1]);
	px->infile = fd[0];
	get_next_line(CLEAN);
}

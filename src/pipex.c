/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/28 20:37:07 by migarrid          #+#    #+#             */
/*   Updated: 2025/07/01 03:44:46 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

int	main(int ac, char **av, char **envp)
{
	t_pipex	px;
	int		exit_status;

	if (ac < 5)
		exit_error(ERR_ARGS, EXIT_FAILURE, NULL);
	if (ft_strcmp(av[1], "here_doc") == 0)
		here_doc_mode(&px, ac, av, envp);
	else
		init_pipex(&px, ac, av, envp);
	exit_status = parent_process(&px, av);
	return (exit_status);
}

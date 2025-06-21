/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 16:32:23 by migarrid          #+#    #+#             */
/*   Updated: 2025/06/21 20:58:05 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

int	main(int ac, char **av)
{
	if (ac != 4)
		return(ft_putstr_fd(ERR_ARGS, STDERR), EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

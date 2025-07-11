/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 23:08:12 by migarrid          #+#    #+#             */
/*   Updated: 2025/07/01 20:15:51 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/pipex.h"

char	*get_path(char *cmd, char **envp)
{
	t_path	x;
	int		i;

	if (!envp || !cmd)
		return (NULL);
	i = 0;
	while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
		i++;
	x.paths = ft_split(envp[i] + 5, ':');
	if (!x.paths)
		return (NULL);
	i = -1;
	while (x.paths[++i])
	{
		x.part_path = ft_strjoin(x.paths[i], "/");
		x.path = ft_strjoin(x.part_path, cmd);
		free(x.part_path);
		if (!x.path)
			return (ft_free_str_array(x.paths), NULL);
		if (access(x.path, X_OK) == 0)
			return (ft_free_str_array(x.paths), x.path);
		free(x.path);
	}
	return (ft_free_str_array(x.paths), NULL);
}

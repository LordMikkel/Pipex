/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: migarrid <migarrid@student.42barcelona.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/19 17:01:50 by migarrid          #+#    #+#             */
/*   Updated: 2025/07/01 04:16:59 by migarrid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

/* ************************************************************************** */
/*                               Includes                                     */
/* ************************************************************************** */
# include "../lib/libft_plus.h"
# include <string.h>
# include <errno.h>
# include <sys/types.h>
# include <sys/wait.h>

/* ************************************************************************** */
/*                               Structure                                    */
/* ************************************************************************** */
typedef struct s_pipex
{
	int		infile;
	int		outfile;
	int		here_doc;
	char	*limiter;
	int		cmd_start;
	int		cmd_count;
	int		pipes_count;
	int		**pipes;
	char	**envp;
	pid_t	*pids;
}	t_pipex;

/* ************************************************************************** */
/*                                 pipex                                      */
/* ************************************************************************** */
int		parent_process(t_pipex *px, char **av);
void	child_process(t_pipex *px, char *cmd, int i);
void	execute(t_pipex *px, char *cmd, char **envp);
void	here_doc_mode(t_pipex *px, int ac, char **av, char **envp);
int		wait_all(t_pipex *px, int childs);

/* ************************************************************************** */
/*                             Initialization                                 */
/* ************************************************************************** */
void	init_pipex(t_pipex *px, int ac, char **av, char **envp);
void	init_here_doc(t_pipex *px, int ac, char **av, char **envp);
void	create_pipes(t_pipex *px);

/* ************************************************************************** */
/*                                 utils                                      */
/* ************************************************************************** */
int		open_file(char *file, int mode);
char	*get_path(char *cmd, char **envp);

/* ************************************************************************** */
/*                                 clean                                      */
/* ************************************************************************** */
void	clean_pipes(t_pipex *px);
void	close_pipes(t_pipex *px);
void	free_pipes(t_pipex *px);

/* ************************************************************************** */
/*                                  exit                                      */
/* ************************************************************************** */
int		exit_error(char *error, int type, t_pipex *px);

/* ************************************************************************** */
/*                                  Limits                                    */
/* ************************************************************************** */
# define STDIN			0
# define STDOUT			1
# define STDERR			2
# define TRUE			1
# define FALSE			0
# define READ			0
# define WRITE			1
# define OUTFILE		1
# define APPEND			0
# define TRUNCATE		1
# define READ_ONLY		2
# define ERROR			-1
# define CLEAN			-1
# define ERROR_EXECVE	1
# define EXIT_CMD		127
# define BUFFER_SIZE	1024
# define FILE_PERMS		0644	// rw-r--r--

/* ************************************************************************** */
/*                                 Msg.Errors                                 */
/* ************************************************************************** */
# define ERRNO			"Errno: %s\n"
# define ERR_ARGS		"Usage: ./pipex file1 \"cmd1\" \"cmd2\" file2\n"
# define ERR_PERMS		"Error: '%s' Permission denied\n"
# define ERR_PIPE		"Error: Failed to create pipe. System limit reached\n"
# define ERR_FILE		"Error: '%s' No such file or directory\n"
# define ERR_CMD		"Error: '%s' command not found\n"
# define ERR_EXEC		"Error: Failed to execute command\n"
# define ERR_FORK		"Error: Fork process failed\n"
# define ERR_MEM		"Error: Memory alloc failed. Check system resources\n"

typedef struct s_path
{
	char	**paths;
	char	*part_path;
	char	*path;
}	t_path;

#endif

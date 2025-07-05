# 🔧 Pipex - Simulating shell pipes in C

[![42 School](https://img.shields.io/badge/42-School-000000?style=flat&logo=42&logoColor=white)](https://42.fr/)
[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))

> A 42 project that replicates the behavior of Unix/Linux pipes, allowing commands to be connected together similar to how the shell works.

<p align="center"> <img src="image/.score.png" alt="alt text" /> </p>

## 📋 Table of Contents

- [What is Pipex?](#-what-is-pipex)
- [Fundamental Concepts](#-fundamental-concepts)
- [Technical Implementation](#-technical-implementation)
- [Project Architecture](#%EF%B8%8F-project-architecture)
- [Installation and Compilation](#-installation-and-compilation)
- [Error Handling](#%EF%B8%8F-error-handling)

## 🎯 What is Pipex?

**Pipex** is a program that simulates the behavior of **pipes** in Unix/Linux shell. It allows connecting multiple commands in a chain, where the output of one command becomes the input of the next, creating a continuous data flow.

### 🔄 Equivalences

**Basic version (2 commands):**
```bash
# Traditional shell
< infile grep "hello" | wc -l > outfile

# Our pipex
./pipex infile "grep hello" "wc -l" outfile
```

**Bonus version (multiple commands):**
```bash
# Traditional shell
< infile cat | grep "user" | sort | uniq > outfile

# Our pipex_bonus
./pipex_bonus infile "cat" "grep user" "sort" "uniq" outfile
```

**Here_doc (interactive input):**
```bash
# Traditional shell
grep "hello" << EOF | wc -l >> outfile

# Our pipex_bonus
./pipex_bonus here_doc EOF "grep hello" "wc -l" outfile
```

## 📚 Fundamental Concepts

### 🪈 What are Pipes?

**Pipes** are an inter-process communication mechanism that allows the output of one program to be the input of another. In our bonus project, we can create chains of multiple pipes:

```
┌─────────┐    ┌──────┐    ┌──────┐    ┌──────┐    ┌─────────┐
│ infile  │───▶│ cmd1 │───▶│ cmd2 │───▶│ cmd3 │───▶│ outfile │
└─────────┘    └──────┘    └──────┘    └──────┘    └─────────┘
```


## 🔬 Technical Implementation

### 🏁 Main Flow (Bonus Version)

```
1. Argument validation
   ↓
2. Detection: Is it here_doc?
   ├─ Yes → here_doc mode
   └─ No → normal mode
   ↓
3. Structure initialization
   ↓
4. Multiple pipes creation
   ↓
5. Fork for each command
   ↓
6. Input/output configuration per child
   ↓
7. Commands execution
   ↓
8. Waiting and synchronization
   ↓
9. Resource cleanup
```

### 🔧 Creating Multiple Pipes

In the bonus version, we create **N-1 pipes** for **N commands**:

```
Command 1 → Pipe 0 → Command 2 → Pipe 1 → Command 3 → Pipe 2 → Command 4
```

**Visual representation:**
```
┌─────────┐   pipe[0]   ┌─────────┐   pipe[1]   ┌─────────┐   pipe[2]   ┌─────────┐
│  cmd1   │ ────────▶	│  cmd2   │ ────────▶   │  cmd3   │ ────────▶  │  cmd4   │
│ PID[0]  │  [0] [1]    │ PID[1]  │  [0] [1]    │ PID[2]  │  [0] [1]    │ PID[3]  │
└─────────┘             └─────────┘             └─────────┘             └─────────┘
```

### 🍴 Process Management

**The parent process acts as the orchestra conductor: 👨🏻**

1. **Creates the infrastructure**: All necessary pipes
2. **Launches the actors**: One `fork()` per command
3. **Steps back**: Closes all pipes (doesn't need them)
4. **Waits for the end**: `waitpid()` for each child

```c
	i = 0;
	create_pipes(px);                    // Create all necessary pipes
	while (i < px->cmd_count)
	{
		px->pids[i] = fork();            // Create child process
		if (px->pids[i] == -1)
			exit_error(ERR_FORK, EXIT_FAILURE, px);
		if (px->pids[i] == 0)            // If I'm the child...
			child_process(px, av[px->cmd_start + i], i);  // Execute command
		i++;
	}
	close_pipes(px);                     // Parent closes all pipes
	status = wait_all(px, px->cmd_count); // Wait for all children
	free(px->pids);                      // Clean memory
	free_pipes(px);
	return (status);                     // Return final status
```

**Each child process specializes: 👶🏻**

1. **Configures its input**:
   - First child: Reads from file or here_doc
   - Rest: Read from previous pipe
2. **Configures its output**:
   - Last child: Writes to file
   - Rest: Write to next pipe

### 🎭 Input/Output Configuration

**For the command at position `i`:**

```c
// Configure STDIN (where the command reads from)
if (i == 0) {
    // First command: reads from file or here_doc
    dup2(px->infile, STDIN_FILENO);
} else {
    // Intermediate commands: read from previous pipe
    dup2(px->pipes_fd[i-1][0], STDIN_FILENO);
}

// Configure STDOUT (where the command writes to)
if (i == last_position) {
    // Last command: writes to file
    dup2(px->outfile, STDOUT_FILENO);
} else {
    // Intermediate commands: write to next pipe
    dup2(px->pipes_fd[i][1], STDOUT_FILENO);
}
```

3. **It transforms**: `execve()` to become the command

```c
	path = get_path(cmd[0], envp);       // Search command in PATH
	if (!path)
	{
		execve(cmd[0], cmd, envp);       // Try to execute directly if not found
		ft_printf_fd(STDERR, ERR_CMD, cmd[0]);
		ft_free_str_array(cmd);
		exit_error(NULL, EXIT_CMD, px);
	}
	execve(path, cmd, envp);             // Transform into the command!
```

### ⏳ Synchronization with waitpid()

The parent must wait for **all** children to avoid zombie processes:

```c
// ⏳ Wait for each child in order
while (i < childs)
{
    waitpid(pids[i], &status, 0);       // Wait for this specific child
    if (i == cmd_count - 1) 			// We only care about the last command's status
	{
        if (WIFEXITED(status))
		status = WEXITSTATUS(status);
    }
}
```

**Why waitpid() and not wait()?**
- `wait()`: Waits for any child (we don't control which one)
- `waitpid()`: Waits for a specific child and get us his status (more control)


### 📝 Here_doc Implementation

The here_doc creates a **temporary pipe** to simulate input:

```
User writes → Temporary pipe → First command
    ↓
heredoc> line 1
heredoc> line 2
heredoc> EOF
```

**Process:**
1. Create temporary pipe
2. Read lines from user until finding the delimiter
3. Write each line to the pipe
4. Close pipe write end
5. Use pipe read end as `infile`

## 🏗️ Project Architecture

### 🔧 Main Structure

```c
typedef struct s_pipex
{
    int     infile;          // Input file FD
    int     outfile;         // Output file FD
    int     here_doc;        // Flag: is it here_doc mode?
    char    *limiter;        // Delimiter for here_doc
    int     cmd_start;       // Index where commands start
    int     cmd_count;       // Total number of commands
    int     pipes_count;     // Number of pipes needed (cmd_count - 1)
    int     **pipes_fd;      // Array of arrays [pipe][read/write]
    char    **envp;          // Environment variables
    pid_t   *pids;           // Array with all children PIDs
}   t_pipex;
```

**Key differences with basic version:**
- **Multiple pipes**: `pipes_fd` is a 2D matrix for N pipes
- **Multiple processes**: `pids` stores all PIDs
- **Here_doc**: Flags and delimiter for interactive input
- **Scalability**: Handles any number of commands


### 📂 File Structure

```
pipex/
├── 📁 inc/
│   ├── pipex.h              # Basic version header
│   └── pipex_bonus.h        # Bonus version header
├── 📁 src/
│   ├── pipex.c              # Basic version main
│   ├── parent.c             # Basic parent process
│   ├── child.c              # Basic child process
│   ├── path.c               # Command search
│   ├── init.c               # Basic initialization
│   ├── clean.c              # Basic cleanup
│   ├── open.c               # Basic file handling
│   ├── exit.c               # Basic error handling
│   └── 📁 bonus/
│       ├── pipex_bonus.c         # Bonus version main
│       ├── parent_bonus.c        # Multiple processes handling
│       ├── child_bonus.c         # Advanced pipes setup
│       ├── here_doc_bonus.c      # Here_doc functionality
│       ├── init_bonus.c          # Multiple pipes initialization
│       ├── clean_bonus.c         # Multiple resources cleanup
│       ├── open_bonus.c          # File handling with append
│       ├── path_bonus.c          # Bonus command search
│       └── exit_bonus.c          # Advanced error handling
├── 📁 lib/
│   └── libft_plus.a         # Extended personal library
├── infile                   # Example input file
├── outfile                  # Generated output file
└── Makefile                 # Build system
```

## 🚀 Installation and Compilation

```bash
# Clone the repository
git clone git@github.com:LordMikkel/Pipex.git
cd pipex

# Compile basic version
make
./pipex infile "grep root" "wc -l" outfile

# Compile bonus version
make bonus
./pipex_bonus infile "cat" "grep user" "sort" "uniq" outfile

```


## 🎮 Usage

### 🔰 Basic Version (2 commands)

```bash
./pipex infile "cmd1" "cmd2" outfile
```

### 🌟 Bonus Version (N commands)

```bash
# Multiple pipes
./pipex_bonus infile "cmd1" "cmd2" "cmd3" "cmd4" outfile

# Here_doc mode
./pipex_bonus here_doc LIMITER "cmd1" "cmd2" outfile
```

### 📋 Practical Examples

**With system files:**
```bash
# Search users in the system
./pipex_bonus /etc/passwd "cut -d: -f1" "sort" "head -5" outfile

# Count system processes
./pipex_bonus /proc/cpuinfo "grep processor" "wc -l" outfile

# Analyze memory
./pipex_bonus /proc/meminfo "head -3" "grep -v Mem" outfile
```

**With repository files:**
```bash
# Search and count
./pipex_bonus infile "grep hello" "wc -l" outfile

# Chain of multiple commands
./pipex_bonus infile "cat" "sort" "uniq" "wc -l" outfile

# Use here_doc
./pipex_bonus here_doc EOF "grep hello" "wc -w" outfile
hello world from pipex
hello there
EOF
```

**Bash equivalences:**
```bash
# Our command:
./pipex_bonus /etc/passwd "head -10" "tail -5" "cut -d: -f1" outfile

# Equivalent in bash:
< /etc/passwd head -10 | tail -5 | cut -d: -f1 > outfile
```

## ⚠️ Error Handling

### 📋 Types of Handled Errors

The program handles multiple types of errors robustly:

- **❌ Invalid arguments**: Incorrect number of parameters or wrong format
- **📁 Files**: Don't exist, no read/write permissions, directories instead of files
- **🔧 Commands**: Not found in PATH, no execution permissions, empty commands
- **🔗 Pipes**: Failed to create pipes (system limits reached)
- **🍴 Fork**: Failed to create processes (system limits, insufficient memory)
- **💾 Memory**: malloc failure, insufficient system resources
- **⚡ Execve**: Failed to execute commands, corrupted files

### 🛡️ Recovery Strategy

The program is designed to **keep working** even when some components fail:

- **Non-existent input file**: First command receives empty input
- **Command not found**: Error is reported but continues with next command
- **Output file without permissions**: Error is reported but pipeline doesn't stop
- **Intermediate command fails**: Pipeline continues, output is passed to next command

This robustness simulates the real behavior of bash shell in Unix/Linux.

### 🎯 Real Applications

This knowledge is fundamental for:
- 🌐 **Server development**: Multiple connections handling
- 🔧 **DevOps**: Scripts and automation
- 🏗️ **Distributed systems**: Inter-service communication
- 🛡️ **Security**: Understanding how processes work

---

## ✍️ Credit

I am Mikel Garrido a student from 42 Barcelona, i always try to make the simpliest but strongest implementation in all my projects, hopufully i could help you with this guide.

[![42](https://img.shields.io/badge/-migarrid-000000?style=flat&logo=42&logoColor=white)](https://profile.intra.42.fr/users/migarrid)

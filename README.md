# 🔧 Pipex - Simulando pipes de shell en C

[![42 School](https://img.shields.io/badge/42-School-000000?style=flat&logo=42&logoColor=white)](https://42.fr/)
[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Norminette](https://img.shields.io/badge/Norminette-passing-success)](https://github.com/42School/norminette)

> Un proyecto de 42 que replica el comportamiento de los pipes (tuberías) de Unix/Linux, permitiendo conectar comandos entre sí de manera similar a como funciona el shell.

## 📋 Índice

- [¿Qué es Pipex?](#-qué-es-pipex)
- [Conceptos Fundamentales](#-conceptos-fundamentales)
- [Arquitectura del Proyecto](#-arquitectura-del-proyecto)
- [Instalación y Compilación](#-instalación-y-compilación)
- [Uso](#-uso)
- [Implementación Técnica](#-implementación-técnica)
- [Parte Bonus](#-parte-bonus)
- [Manejo de Errores](#-manejo-de-errores)
- [Testing](#-testing)
- [Reflexiones y Aprendizajes](#-reflexiones-y-aprendizajes)

## 🎯 ¿Qué es Pipex?

**Pipex** es un programa que simula el comportamiento de los **pipes** (tuberías) del shell Unix/Linux. En lugar de usar el operador `|` del shell, nuestro programa conecta dos comandos, tomando la salida del primero como entrada del segundo.

### 🔄 Equivalencia

```bash
# Shell tradicional
< file1 cmd1 | cmd2 > file2

# Nuestro pipex
./pipex file1 "cmd1" "cmd2" file2
```

### 💡 ¿Por qué es importante?

Este proyecto te enseña conceptos fundamentales de **programación de sistemas**:
- 🔧 **Procesos**: Creación y gestión con `fork()`
- 🔗 **IPC (Inter-Process Communication)**: Comunicación entre procesos usando pipes
- 📂 **File descriptors**: Manipulación y redirección de entrada/salida
- 🎮 **Ejecución de programas**: Uso de la familia `exec()`

## 📚 Conceptos Fundamentales

### 🌊 ¿Qué son los Pipes?

Los **pipes** son un mecanismo de comunicación entre procesos que permite que la salida de un programa sea la entrada de otro, creando un "flujo de datos" entre ellos.

```
┌─────────┐    ┌──────┐    ┌─────────┐
│  file1  │───▶│ cmd1 │───▶│  cmd2   │───▶ file2
└─────────┘    └──────┘    └─────────┘
```

### 🍴 Fork y Procesos

El **fork()** crea un proceso hijo idéntico al padre. Después del fork:
- **Proceso padre**: Obtiene el PID del hijo
- **Proceso hijo**: Obtiene 0

```c
pid_t pid = fork();
if (pid == 0) {
    // Código del proceso hijo
} else if (pid > 0) {
    // Código del proceso padre
}
```

### 📁 File Descriptors

Los **file descriptors** son números que el SO usa para identificar archivos abiertos:
- `0` → STDIN (entrada estándar)
- `1` → STDOUT (salida estándar)
- `2` → STDERR (error estándar)

### 🔀 Redirección con dup2()

`dup2()` duplica un file descriptor, permitiendo redireccionar entrada/salida:

```c
dup2(pipe_fd[1], STDOUT_FILENO); // Redirige STDOUT al pipe
```

## 🏗️ Arquitectura del Proyecto

### 📂 Estructura de Archivos

```
pipex/
├── 📁 inc/
│   └── pipex.h              # Header principal
├── 📁 src/
│   ├── pipex.c              # Función main
│   ├── init.c               # Inicialización de estructuras
│   ├── parent.c             # Lógica del proceso padre
│   ├── child.c              # Lógica del proceso hijo
│   ├── path.c               # Búsqueda de comandos en PATH
│   ├── open.c               # Manejo de archivos
│   ├── clean.c              # Liberación de recursos
│   ├── exit.c               # Manejo de errores y salida
│   └── 📁 bonus/            # Implementación bonus
├── 📁 lib/
│   └── libft_plus.a         # Biblioteca personal
└── Makefile                 # Sistema de compilación
```

### 🔧 Estructura Principal

```c
typedef struct s_pipex
{
    int     infile;          // FD del archivo de entrada
    int     outfile;         // FD del archivo de salida
    int     cmd_start;       // Índice donde empiezan los comandos
    int     pipe_fd[2];      // File descriptors del pipe
    char    **envp;          // Variables de entorno
    pid_t   pid[2];          // PIDs de los procesos hijos
}   t_pipex;
```

## 🚀 Instalación y Compilación

### 📋 Requisitos

- 🐧 Sistema Unix/Linux
- 🔨 GCC compilador
- 📚 Make

### ⚡ Compilación

```bash
# Clonar el repositorio
git clone [tu-repo] pipex
cd pipex

# Compilar versión básica
make

# Compilar versión bonus
make bonus

# Limpiar archivos objeto
make clean

# Limpieza completa
make fclean

# Recompilar todo
make re
```

## 🎮 Uso

### 🔰 Versión Básica

```bash
./pipex infile "cmd1" "cmd2" outfile
```

**Ejemplos prácticos:**

```bash
# Equivale a: < infile grep "hello" | wc -l > outfile
./pipex infile "grep hello" "wc -l" outfile

# Equivale a: < /etc/passwd cut -d: -f1 | sort > users.txt
./pipex /etc/passwd "cut -d: -f1" "sort" users.txt

# Equivale a: < file.txt cat | head -5 > first_lines.txt
./pipex file.txt "cat" "head -5" first_lines.txt
```

### 🌟 Versión Bonus

#### 🔢 Múltiples Pipes
```bash
./pipex_bonus infile "cmd1" "cmd2" "cmd3" "cmd4" outfile
# Equivale a: < infile cmd1 | cmd2 | cmd3 | cmd4 > outfile
```

#### 📝 Here_doc
```bash
./pipex_bonus here_doc LIMITER "cmd1" "cmd2" outfile
# Equivale a: cmd1 << LIMITER | cmd2 >> outfile
```

**Ejemplo de here_doc:**
```bash
./pipex_bonus here_doc EOF "grep hello" "wc -l" result.txt
hello world
hello pipex
EOF
```

## 🔬 Implementación Técnica

### 🏁 Flujo Principal

1. **Validación**: Verificar argumentos de entrada
2. **Inicialización**: Configurar estructura `t_pipex`
3. **Crear Pipe**: Establecer comunicación entre procesos
4. **Fork**: Crear procesos hijo para cada comando
5. **Ejecución**: Ejecutar comandos con redirección
6. **Espera**: El padre espera a que terminen los hijos
7. **Limpieza**: Cerrar FDs y liberar recursos

### 🔧 Funciones Clave

#### `init_pipex()` - Inicialización
```c
void init_pipex(t_pipex *px, int ac, char **av, char **envp)
{
    px->envp = envp;
    px->cmd_start = 2;
    px->infile = open_file(av[1], READ_ONLY);
    px->outfile = open_file(av[ac - 1], TRUNCATE);

    if (pipe(px->pipe_fd) == -1)
        exit_error(ERR_PIPE, EXIT_FAILURE, px);
}
```

#### `parent_process()` - Proceso Padre
```c
int parent_process(t_pipex *px, char **av)
{
    // Crear primer proceso hijo
    px->pid[0] = fork();
    if (px->pid[0] == 0)
        child_process(px, av[px->cmd_start], 0);

    // Crear segundo proceso hijo
    px->pid[1] = fork();
    if (px->pid[1] == 0)
        child_process(px, av[px->cmd_start + 1], 1);

    // Cerrar pipes y esperar hijos
    close_pipes(px);
    waitpid(px->pid[0], NULL, 0);
    waitpid(px->pid[1], &status, 0);

    return (WEXITSTATUS(status));
}
```

#### `child_process()` - Proceso Hijo
```c
void child_process(t_pipex *px, char *cmd, int i)
{
    if (i == 0) // Primer comando
    {
        dup2(px->infile, STDIN_FILENO);     // Leer del archivo
        dup2(px->pipe_fd[1], STDOUT_FILENO); // Escribir al pipe
    }
    else // Segundo comando
    {
        dup2(px->pipe_fd[0], STDIN_FILENO);  // Leer del pipe
        dup2(px->outfile, STDOUT_FILENO);    // Escribir al archivo
    }

    close_pipes(px);
    execute(px, cmd, px->envp);
}
```

#### `get_path()` - Búsqueda de Comandos
```c
char *get_path(char *cmd, char **envp)
{
    char **paths;
    char *full_path;

    // Buscar variable PATH en envp
    paths = ft_split(getenv("PATH"), ':');

    // Probar cada directorio del PATH
    for (int i = 0; paths[i]; i++)
    {
        full_path = ft_strjoin(paths[i], "/");
        full_path = ft_strjoin(full_path, cmd);

        if (access(full_path, X_OK) == 0)
            return (full_path);
    }
    return (NULL);
}
```

### 🎯 Redirección de Entrada/Salida

```
Primer proceso hijo:
┌─────────┐    ┌──────────┐    ┌──────────┐
│ infile  │───▶│ STDIN    │    │ STDOUT   │───▶ pipe[1]
└─────────┘    │ (FD 0)   │    │ (FD 1)   │
               └──────────┘    └──────────┘

Segundo proceso hijo:
┌──────────┐    ┌──────────┐    ┌─────────┐
│ pipe[0]  │───▶│ STDIN    │    │ STDOUT  │───▶│ outfile │
           │    │ (FD 0)   │    │ (FD 1)  │    └─────────┘
           └────┘──────────┘    └─────────┘
```

## 🌟 Parte Bonus

### 🔢 Múltiples Pipes

La versión bonus maneja **n comandos** creando una cadena de pipes:

```c
// Para n comandos, necesitamos (n-1) pipes
int pipes[cmd_count-1][2];

for (int i = 0; i < cmd_count-1; i++)
    pipe(pipes[i]);
```

### 📝 Here_doc

Implementa el comportamiento de `<<` del shell:

```c
void handle_here_doc(char *limiter)
{
    char *line;
    int tmp_fd;

    tmp_fd = open(".tmp_here_doc", O_CREAT | O_WRONLY | O_TRUNC, 0644);

    while (1)
    {
        ft_putstr_fd("pipe heredoc> ", STDOUT_FILENO);
        line = get_next_line(STDIN_FILENO);

        if (ft_strncmp(line, limiter, ft_strlen(limiter)) == 0)
            break;

        ft_putstr_fd(line, tmp_fd);
    }
    close(tmp_fd);
}
```

## ⚠️ Manejo de Errores

### 🛡️ Estrategia de Error Handling

```c
int exit_error(char *error, int type, t_pipex *px)
{
    // Mostrar error apropiado
    if (error)
        ft_putstr_fd(error, STDERR_FILENO);

    // Limpiar recursos si es necesario
    if (px && type == CLEAN)
        close_pipes(px);

    exit(type);
}
```

### 📋 Tipos de Errores Manejados

- ❌ **Argumentos inválidos**: Número incorrecto de parámetros
- 📁 **Archivos**: No existen, sin permisos, etc.
- 🔧 **Comandos**: No encontrados en PATH
- 🔗 **Pipes**: Fallo al crear pipe
- 🍴 **Fork**: Fallo al crear proceso
- 💾 **Memoria**: Fallo en malloc
- ⚡ **Execve**: Fallo al ejecutar comando

### 🔍 Códigos de Salida

```c
#define EXIT_SUCCESS    0   // Éxito
#define EXIT_FAILURE    1   // Error general
#define EXIT_CMD        127 // Comando no encontrado
```

## 🧪 Testing

### ✅ Tests Básicos

```bash
# Test 1: Funcionamiento básico
echo "hello world" > test.txt
./pipex test.txt "cat" "wc -w" result.txt
cat result.txt  # Debería mostrar: 2

# Test 2: Con grep
./pipex /etc/passwd "grep root" "wc -l" count.txt

# Test 3: Error handling
./pipex noexist.txt "cat" "wc -l" out.txt  # Debe mostrar error
```

### 🌟 Tests Bonus

```bash
# Multiple pipes
./pipex_bonus infile "cmd1" "cmd2" "cmd3" outfile

# Here_doc
./pipex_bonus here_doc EOF "cat" "wc -l" result.txt
test line 1
test line 2
EOF
```

### 🔍 Comparación con Shell

```bash
# Crear función de test
test_pipex() {
    # Shell tradicional
    < $1 $2 | $3 > expected.txt

    # Nuestro pipex
    ./pipex $1 "$2" "$3" actual.txt

    # Comparar resultados
    diff expected.txt actual.txt
}

test_pipex "/etc/passwd" "head -5" "tail -1"
```

## 🎓 Reflexiones y Aprendizajes

### 💪 Conceptos Dominados

1. **🔧 Programación de Sistemas**
   - Manejo de procesos con `fork()`, `wait()`, `execve()`
   - File descriptors y redirección de I/O
   - Pipes para comunicación entre procesos

2. **🎯 Gestión de Recursos**
   - Apertura/cierre correcto de archivos
   - Prevención de memory leaks
   - Manejo de errores robusto

3. **🏗️ Arquitectura de Software**
   - Separación de responsabilidades
   - Código modular y reutilizable
   - Manejo consistente de errores

### 🚀 Habilidades Desarrolladas

- **🔍 Debugging**: Uso de herramientas como `strace`, `valgrind`
- **📖 Documentación**: Lectura intensiva de man pages
- **🧪 Testing**: Creación de casos de prueba comprehensivos
- **🛠️ Herramientas**: Makefiles, git, debugging tools

### 💡 Lecciones Aprendidas

1. **La importancia del manejo de errores**: Cada syscall puede fallar
2. **Resource management**: Los FDs son recursos limitados
3. **Process synchronization**: Coordinar múltiples procesos es complejo
4. **UNIX philosophy**: "Do one thing and do it well"

### 🎯 Aplicaciones Reales

Este conocimiento es fundamental para:
- 🌐 **Desarrollo de servidores**: Manejo de múltiples conexiones
- 🔧 **DevOps**: Scripts y automatización
- 🏗️ **Sistemas distribuidos**: Comunicación entre servicios
- 🛡️ **Seguridad**: Entender como funcionan los procesos

---

## 📞 Contacto

Si tienes preguntas sobre la implementación o quieres discutir sobre programación de sistemas:

[![42 Profile](https://img.shields.io/badge/42-migarrid-000000?style=flat&logo=42&logoColor=white)](https://profile.intra.42.fr/users/migarrid)

---

> 💭 *"Los pipes son como las tuberías de agua: conectan la salida de un lugar con la entrada de otro, creando un flujo continuo de datos."*

**¡Happy coding!** 🚀

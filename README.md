# 🔧 Pipex - Simulando pipes de shell en C

[![42 School](https://img.shields.io/badge/42-School-000000?style=flat&logo=42&logoColor=white)](https://42.fr/)
[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![Norminette](https://img.shields.io/badge/Norminette-passing-success)](https://github.com/42School/norminette)

<p align="center"> <img src="image/.score.png" alt="alt text" /> </p>

> Un proyecto de 42 que replica el comportamiento de los pipes (tuberías) de Unix/Linux, permitiendo conectar comandos entre sí de manera similar a como funciona el shell.

## 📋 Índice

- [¿Qué es Pipex?](#-qué-es-pipex)
- [Conceptos Fundamentales](#-conceptos-fundamentales)
- [Implementación Técnica](#-implementación-técnica)
- [Arquitectura del Proyecto](#-arquitectura-del-proyecto)
- [Instalación y Compilación](#-instalación-y-compilación)
- [Manejo de Errores](https://github.com/LordMikkel/Pipex/blob/main/README.md#%EF%B8%8F-manejo-de-errores)

## 🎯 ¿Qué es Pipex?

**Pipex** es un programa que simula el comportamiento de los **pipes** (tuberías) del shell Unix/Linux. Permite conectar múltiples comandos en cadena, donde la salida de un comando se convierte en la entrada del siguiente, creando un flujo continuo de datos.

### 🔄 Equivalencias

**Versión básica (2 comandos):**
```bash
# Shell tradicional
< infile grep "hello" | wc -l > outfile

# Nuestro pipex
./pipex infile "grep hello" "wc -l" outfile
```

**Versión bonus (múltiples comandos):**
```bash
# Shell tradicional
< infile cat | grep "user" | sort | uniq > outfile

# Nuestro pipex_bonus
./pipex_bonus infile "cat" "grep user" "sort" "uniq" outfile
```

**Here_doc (entrada interactiva):**
```bash
# Shell tradicional
grep "hello" << EOF | wc -l >> outfile

# Nuestro pipex_bonus
./pipex_bonus here_doc EOF "grep hello" "wc -l" outfile
```

## 📚 Conceptos Fundamentales

### 🌊 ¿Qué son los Pipes?

Los **pipes** son un mecanismo de comunicación entre procesos que permite que la salida de un programa sea la entrada de otro. En nuestro proyecto bonus, podemos crear cadenas de múltiples pipes:

```
┌─────────┐    ┌──────┐    ┌──────┐    ┌──────┐    ┌─────────┐
│ infile  │───▶│ cmd1 │───▶│ cmd2 │───▶│ cmd3 │───▶│ outfile │
└─────────┘    └──────┘    └──────┘    └──────┘    └─────────┘
```


## 🔬 Implementación Técnica

### 🏁 Flujo Principal (Versión Bonus)

```
1. Validación de argumentos
   ↓
2. Detección: ¿Es here_doc?
   ├─ Sí → Modo here_doc
   └─ No → Modo normal
   ↓
3. Inicialización estructura
   ↓
4. Creación de múltiples pipes
   ↓
5. Fork para cada comando
   ↓
6. Configuración de entrada/salida por hijo
   ↓
7. Ejecución de comandos
   ↓
8. Espera y sincronización
   ↓
9. Limpieza de recursos
```

### 🔧 Creación de Múltiples Pipes

En la versión bonus, creamos **N-1 pipes** para **N comandos**:

```
Comando 1 → Pipe 0 → Comando 2 → Pipe 1 → Comando 3 → Pipe 2 → Comando 4
```

**Representación visual:**
```
┌─────────┐   pipe[0]   ┌─────────┐   pipe[1]   ┌─────────┐   pipe[2]   ┌─────────┐
│  cmd1   │ ────────▶	│  cmd2   │ ────────▶   │  cmd3   │ ────────▶  │  cmd4   │
│ PID[0]  │  [0] [1]    │ PID[1]  │  [0] [1]    │ PID[2]  │  [0] [1]    │ PID[3]  │
└─────────┘             └─────────┘             └─────────┘             └─────────┘
```

### 🍴 Gestión de Procesos

**El proceso padre actúa como director de orquesta: 👨🏻**

1. **Crea la infraestructura**: Todos los pipes necesarios
2. **Lanza a los actores**: Un `fork()` por comando
3. **Se retira**: Cierra todos los pipes (no los necesita)
4. **Espera el final**: `waitpid()` a cada hijo

```c
	i = 0;
	create_pipes(px);                    // Crear todos los pipes necesarios
	while (i < px->cmd_count)
	{
		px->pids[i] = fork();            // Crear proceso hijo
		if (px->pids[i] == -1)
			exit_error(ERR_FORK, EXIT_FAILURE, px);
		if (px->pids[i] == 0)            // Si soy el hijo...
			child_process(px, av[px->cmd_start + i], i);  // Ejecutar comando
		i++;
	}
	close_pipes(px);                     // Padre cierra todos los pipes
	status = wait_all(px, px->cmd_count); // Esperar a todos los hijos
	free(px->pids);                      // Limpiar memoria
	free_pipes(px);
	return (status);                     // Retornar estado final
```

**Cada proceso hijo se especializa: 👶🏻**

1. **Configura su entrada**:
   - Primer hijo: Lee del archivo o here_doc
   - Resto: Lee del pipe anterior
2. **Configura su salida**:
   - Último hijo: Escribe al archivo
   - Resto: Escribe al pipe siguiente

### 🎭 Configuración de Entrada/Salida

**Para el comando en posición `i`:**

```c
// Configurar STDIN (de dónde lee el comando)
if (i == 0) {
    // Primer comando: lee del archivo o here_doc
    dup2(px->infile, STDIN_FILENO);
} else {
    // Comandos intermedios: leen del pipe anterior
    dup2(px->pipes_fd[i-1][0], STDIN_FILENO);
}

// Configurar STDOUT (hacia dónde escribe el comando)
if (i == última_posición) {
    // Último comando: escribe al archivo
    dup2(px->outfile, STDOUT_FILENO);
} else {
    // Comandos intermedios: escriben al pipe siguiente
    dup2(px->pipes_fd[i][1], STDOUT_FILENO);
}
```

3. **Se transforma**: `execve()` para convertirse en el comando

```c
	path = get_path(cmd[0], envp);       // Buscar comando en PATH
	if (!path)
	{
		execve(cmd[0], cmd, envp);       // Intentar ejecutar directamente si no lo ha encontrado
		ft_printf_fd(STDERR, ERR_CMD, cmd[0]);
		ft_free_str_array(cmd);
		exit_error(NULL, EXIT_CMD, px);
	}
	execve(path, cmd, envp);             // ¡Transformarse en el comando!
```

### ⏳ Sincronización con waitpid()

El padre debe esperar a **todos** los hijos para evitar procesos zombie:

```c
// ⏳ Esperar a cada hijo en orden
while (i < childs)
{
    waitpid(pids[i], &status, 0);       // Esperar a este hijo específico
    if (i == cmd_count - 1) // Solo nos importa el estado del último comando
	{
        if (WIFEXITED(status))
		status = WEXITSTATUS(status);
    }
}
```

**¿Por qué waitpid() y no wait()?**
- `wait()`: Espera a cualquier hijo (no controlamos cuál)
- `waitpid()`: Espera a un hijo específico (más control)


### 📝 Implementación Here_doc

El here_doc crea un **pipe temporal** para simular la entrada:

```
Usuario escribe → Pipe temporal → Primer comando
    ↓
heredoc> línea 1
heredoc> línea 2
heredoc> EOF
```

**Proceso:**
1. Crear pipe temporal
2. Leer líneas del usuario hasta encontrar el limitador
3. Escribir cada línea al pipe
4. Cerrar escritura del pipe
5. Usar la lectura del pipe como `infile`

## 🏗️ Arquitectura del Proyecto

### 🔧 Estructura Principal

```c
typedef struct s_pipex
{
    int     infile;          // FD del archivo de entrada
    int     outfile;         // FD del archivo de salida
    int     here_doc;        // Flag: ¿es modo here_doc?
    char    *limiter;        // Limitador para here_doc
    int     cmd_start;       // Índice donde empiezan los comandos
    int     cmd_count;       // Número total de comandos
    int     pipes_count;     // Número de pipes necesarios (cmd_count - 1)
    int     **pipes_fd;      // Array de arrays [pipe][read/write]
    char    **envp;          // Variables de entorno
    pid_t   *pids;           // Array con PIDs de todos los hijos
}   t_pipex;
```

**Diferencias clave con la versión básica:**
- **Múltiples pipes**: `pipes_fd` es una matriz 2D para N pipes
- **Múltiples procesos**: `pids` almacena todos los PIDs
- **Here_doc**: Flags y limitador para entrada interactiva
- **Escalabilidad**: Maneja cualquier número de comandos


### 📂 Estructura de Archivos (Bonus)

```
pipex/
├── 📁 inc/
│   ├── pipex.h              # Header versión básica
│   └── pipex_bonus.h        # Header versión bonus
├── 📁 src/
│   ├── pipex.c              # Main versión básica
│   ├── parent.c             # Proceso padre básico
│   ├── child.c              # Proceso hijo básico
│   ├── path.c               # Búsqueda de comandos
│   ├── init.c               # Inicialización básica
│   ├── clean.c              # Limpieza básica
│   ├── open.c               # Manejo de archivos básico
│   ├── exit.c               # Manejo de errores básico
│   └── 📁 bonus/
│       ├── pipex_bonus.c         # Main versión bonus
│       ├── parent_bonus.c        # Manejo múltiples procesos
│       ├── child_bonus.c         # Setup avanzado de pipes
│       ├── here_doc_bonus.c      # Funcionalidad here_doc
│       ├── init_bonus.c          # Inicialización múltiples pipes
│       ├── clean_bonus.c         # Limpieza múltiples recursos
│       ├── open_bonus.c          # Manejo archivos con append
│       ├── path_bonus.c          # Búsqueda de comandos bonus
│       └── exit_bonus.c          # Manejo errores avanzado
├── 📁 lib/
│   └── libft_plus.a         # Biblioteca personal extendida
├── infile                   # Archivo de entrada de ejemplo
├── outfile                  # Archivo de salida generado
└── Makefile                 # Sistema de compilación
```

## 🚀 Instalación y Compilación

```bash
# Clonar el repositorio
git clone git@github.com:LordMikkel/Pipex.git
cd pipex

# Compilar versión básica
make
./pipex infile "grep root" "wc -l" outfile

# Compilar versión bonus
make bonus
./pipex_bonus infile "cat" "grep user" "sort" "uniq" outfile

```

## 🎮 Uso

### 🔰 Versión Básica (2 comandos)

```bash
./pipex infile "cmd1" "cmd2" outfile
```

### 🌟 Versión Bonus (N comandos)

```bash
# Múltiples pipes
./pipex_bonus infile "cmd1" "cmd2" "cmd3" "cmd4" outfile

# Here_doc mode
./pipex_bonus here_doc LIMITER "cmd1" "cmd2" outfile
```

### 📋 Ejemplos Prácticos

**Con archivos del sistema:**
```bash
# Buscar usuarios en el sistema
./pipex_bonus /etc/passwd "cut -d: -f1" "sort" "head -5" outfile

# Contar procesos del sistema
./pipex_bonus /proc/cpuinfo "grep processor" "wc -l" outfile

# Analizar memoria
./pipex_bonus /proc/meminfo "head -3" "grep -v Mem" outfile
```

**Con archivos del repositorio:**
```bash
# Buscar y contar
./pipex_bonus infile "grep hello" "wc -l" outfile

# Cadena de múltiples comandos
./pipex_bonus infile "cat" "sort" "uniq" "wc -l" outfile

# Usar here_doc
./pipex_bonus here_doc EOF "grep hello" "wc -w" outfile
hello world from pipex
hello there
EOF
```

**Equivalencias con bash:**
```bash
# Nuestro comando:
./pipex_bonus /etc/passwd "head -10" "tail -5" "cut -d: -f1" outfile

# Equivale en bash a:
< /etc/passwd head -10 | tail -5 | cut -d: -f1 > outfile
```

## ⚠️ Manejo de Errores

### 📋 Tipos de Errores Manejados

El programa maneja múltiples tipos de errores de forma robusta:

- **❌ Argumentos inválidos**: Número incorrecto de parámetros o formato erróneo
- **📁 Archivos**: No existen, sin permisos de lectura/escritura, directorios en lugar de archivos
- **🔧 Comandos**: No encontrados en PATH, sin permisos de ejecución, comandos vacíos
- **🔗 Pipes**: Fallo al crear pipes (límites del sistema alcanzados)
- **🍴 Fork**: Fallo al crear procesos (límites del sistema, memoria insuficiente)
- **💾 Memoria**: Fallo en malloc, recursos insuficientes del sistema
- **⚡ Execve**: Fallo al ejecutar comandos, archivos corruptos

### 🛡️ Estrategia de Recuperación

El programa está diseñado para **continuar funcionando** incluso cuando algunos componentes fallan:

- **Archivo de entrada inexistente**: Primer comando recibe entrada vacía
- **Comando no encontrado**: Se reporta error pero se continúa con el siguiente
- **Archivo de salida sin permisos**: Se reporta error pero no se detiene el pipeline
- **Comando intermedio falla**: El pipeline continúa, la salida se pasa al siguiente comando

Esta robustez simula el comportamiento real de la shell de bash en Unix/Linux.

### 🎯 Aplicaciones Reales

Este conocimiento es fundamental para:
- 🌐 **Desarrollo de servidores**: Manejo de múltiples conexiones
- 🔧 **DevOps**: Scripts y automatización
- 🏗️ **Sistemas distribuidos**: Comunicación entre servicios
- 🛡️ **Seguridad**: Entender como funcionan los procesos

---

## 📞 Contacto

[(https://img.shields.io/badge/42-migarrid-000000?style=flat&logo=42&logoColor=white)](https://profile.intra.42.fr/users/migarrid)

# 🔧 Pipex - Simulando pipes de shell en C

[![42 School](https://img.shields.io/badge/42-School-000000?style=flat&logo=42&logoColor=white)](https://42.fr/)
[![Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))

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

### 🌟 Características del Bonus

- **🔗 Múltiples pipes**: Conecta N comandos en cadena
- **📝 Here_doc**: Entrada interactiva como en bash (`<<`)
- **📎 Append mode**: Añadir al archivo en lugar de sobrescribir
- **🔧 Manejo robusto de errores**: Continúa funcionando aunque fallen comandos intermedios

### 💡 ¿Por qué es importante?

Este proyecto te enseña conceptos fundamentales de **programación de sistemas**:
- 🍴 **Procesos**: Creación, gestión y sincronización con `fork()` y `waitpid()`
- 🔗 **IPC (Inter-Process Communication)**: Comunicación entre procesos usando pipes
- 📂 **File descriptors**: Manipulación y redirección de entrada/salida con `dup2()`
- 🎮 **Ejecución de programas**: Reemplazo de procesos con `execve()`

## 📚 Conceptos Fundamentales

### 🌊 ¿Qué son los Pipes?

Los **pipes** son un mecanismo de comunicación entre procesos que permite que la salida de un programa sea la entrada de otro. En nuestro proyecto bonus, podemos crear cadenas de múltiples pipes:

```
┌─────────┐    ┌──────┐    ┌──────┐    ┌──────┐    ┌─────────┐
│ infile  │───▶│ cmd1 │───▶│ cmd2 │───▶│ cmd3 │───▶│ outfile │
└─────────┘    └──────┘    └──────┘    └──────┘    └─────────┘
```

### 🍴 Fork: Creando Procesos

**`fork()`** es la función que crea un proceso hijo **idéntico** al proceso padre. Es como "clonar" el programa en ejecución:

```c
pid_t pid = fork();
if (pid == 0) {
    // ¡Soy el proceso HIJO!
    // Voy a ejecutar un comando
} else if (pid > 0) {
    // ¡Soy el proceso PADRE!
    // pid contiene el ID del hijo que acabo de crear
}
```

#### 🏠 Proceso Padre en mi programa:
- **Coordina todo el proceso**: Crea todos los pipes necesarios
- **Lanza los hijos**: Hace un `fork()` por cada comando
- **Cierra pipes**: Cierra todos los file descriptors que no usa
- **Espera resultados**: Usa `waitpid()` para esperar que terminen todos los hijos
- **Retorna el estado**: Devuelve el código de salida del último comando

#### 👶 Proceso Hijo en mi programa:
- **Se especializa**: Cada hijo ejecuta UN comando específico
- **Configura entrada**: Conecta su STDIN al pipe anterior o al archivo de entrada
- **Configura salida**: Conecta su STDOUT al siguiente pipe o al archivo de salida
- **Se transforma**: Usa `execve()` para convertirse en el comando que debe ejecutar
- **Nunca regresa**: Después de `execve()`, ya no es nuestro código

### 🔀 dup2(): Redirección de Flujos

**`dup2()`** es la función mágica que permite "redirigir" la entrada y salida:

```c
dup2(pipe_fd[0], STDIN_FILENO);  // "Ahora mi STDIN lee del pipe"
dup2(pipe_fd[1], STDOUT_FILENO); // "Ahora mi STDOUT escribe al pipe"
```

**¿Cómo funciona?**
- Toma un file descriptor (como `pipe_fd[0]`)
- Lo "copia" sobre otro (como `STDIN_FILENO`)
- Ahora cuando el programa lea de STDIN, realmente lee del pipe

### ⏳ waitpid(): Sincronización

**`waitpid()`** permite al proceso padre esperar a que terminen sus hijos:

```c
waitpid(pid, &status, 0); // "Espera a que termine este proceso específico"
```

**En mi programa:**
- El padre espera a TODOS los hijos antes de terminar
- Recoge el código de salida del último comando (el importante)
- Evita procesos "zombie" (hijos que terminan pero no son recogidos)

### 📝 Here_doc: Entrada Interactiva

El **here_doc** simula el comportamiento de `<<` en bash:

```bash
# En bash:
cat << EOF
línea 1
línea 2
EOF

# En nuestro programa:
./pipex_bonus here_doc EOF "cat" "wc -l" outfile
```

**Funcionamiento:**
1. Crea un pipe temporal
2. Lee líneas del usuario hasta encontrar el limitador
3. Escribe las líneas al pipe
4. Usa el pipe como entrada para el primer comando

## 🏗️ Arquitectura del Proyecto

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

### 🔧 Estructura Principal (Bonus)

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

## 🚀 Instalación y Compilación

```bash
# Clonar el repositorio
git clone [tu-repo] pipex
cd pipex

# Compilar versión básica
make
./pipex infile "grep root" "wc -l" outfile

# Compilar versión bonus
make bonus
./pipex_bonus infile "cat" "grep user" "sort" "uniq" outfile

# Compilar ambas versiones
make all bonus

# Limpiar objetos
make clean

# Limpieza completa
make fclean

# Recompilar desde cero
make re
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
# Crear archivo de prueba
echo -e "hello world\nhello pipex\ngoodbye world" > infile

# Buscar y contar
./pipex_bonus infile "grep hello" "wc -l" outfile
# outfile contendrá: 2

# Cadena de múltiples comandos
./pipex_bonus infile "cat" "sort" "uniq" "wc -l" outfile

# Usar here_doc
./pipex_bonus here_doc EOF "grep hello" "wc -w" outfile
hello world from pipex
hello there
EOF
# outfile contendrá: 5
```

**Equivalencias con bash:**
```bash
# Nuestro comando:
./pipex_bonus /etc/passwd "head -10" "tail -5" "cut -d: -f1" outfile

# Equivale en bash a:
< /etc/passwd head -10 | tail -5 | cut -d: -f1 > outfile
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
│  cmd1   │ ────────▶   │  cmd2   │ ────────▶   │  cmd3   │ ────────▶   │  cmd4   │
│ PID[0]  │  [0] [1]    │ PID[1]  │  [0] [1]    │ PID[2]  │  [0] [1]    │ PID[3]  │
└─────────┘             └─────────┘             └─────────┘             └─────────┘
```

### 🍴 Gestión de Procesos

**El proceso padre actúa como director de orquesta:**

1. **Crea la infraestructura**: Todos los pipes necesarios
2. **Lanza a los actores**: Un `fork()` por comando
3. **Se retira**: Cierra todos los pipes (no los necesita)
4. **Espera el final**: `waitpid()` a cada hijo

**Cada proceso hijo se especializa:**

1. **Configura su entrada**:
   - Primer hijo: Lee del archivo o here_doc
   - Resto: Lee del pipe anterior
2. **Configura su salida**:
   - Último hijo: Escribe al archivo
   - Resto: Escribe al pipe siguiente
3. **Se transforma**: `execve()` para convertirse en el comando

### 🎭 Configuración de Entrada/Salida

**Para el comando en posición `i`:**

```c
// Configurar STDIN
if (i == 0) {
    // Primer comando: lee del archivo o here_doc
    dup2(px->infile, STDIN_FILENO);
} else {
    // Comandos intermedios: leen del pipe anterior
    dup2(px->pipes_fd[i-1][0], STDIN_FILENO);
}

// Configurar STDOUT
if (i == última_posición) {
    // Último comando: escribe al archivo
    dup2(px->outfile, STDOUT_FILENO);
} else {
    // Comandos intermedios: escriben al pipe siguiente
    dup2(px->pipes_fd[i][1], STDOUT_FILENO);
}
```

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

### 🔄 Redirección Visual Detallada

**Ejemplo con 3 comandos:**

```
ANTES de dup2():
┌─────────────────────────────────────────────────────────────────┐
│ Proceso Padre                                                   │
│ ┌─────────┐  ┌──────────────┐  ┌──────────────┐                │
│ │ infile  │  │   pipe[0]    │  │   pipe[1]    │                │
│ │   FD    │  │ [0]    [1]   │  │ [0]    [1]   │                │
│ └─────────┘  └──────────────┘  └──────────────┘                │
└─────────────────────────────────────────────────────────────────┘

DESPUÉS de fork() y dup2():
┌─────────────┐       ┌─────────────┐       ┌─────────────┐
│ Hijo 1      │       │ Hijo 2      │       │ Hijo 3      │
│ STDIN  ───▶ infile  │ STDIN  ───▶ pipe[0][0] │ STDIN  ───▶ pipe[1][0] │
│ STDOUT ───▶ pipe[0][1] │ STDOUT ───▶ pipe[1][1] │ STDOUT ───▶ outfile │
└─────────────┘       └─────────────┘       └─────────────┘
```

### ⏳ Sincronización con waitpid()

El padre debe esperar a **todos** los hijos para evitar procesos zombie:

```c
// Esperar a cada hijo en orden
for (int i = 0; i < cmd_count; i++) {
    waitpid(pids[i], &status, 0);
    // Solo nos importa el estado del último comando
    if (i == cmd_count - 1) {
        exit_status = WEXITSTATUS(status);
    }
}
```

**¿Por qué waitpid() y no wait()?**
- `wait()`: Espera a cualquier hijo (no controlamos cuál)
- `waitpid()`: Espera a un hijo específico (más control)


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

Esta robustez simula el comportamiento real del shell Unix/Linux.


### 🎯 Aplicaciones Reales

Este conocimiento es fundamental para:
- 🌐 **Desarrollo de servidores**: Manejo de múltiples conexiones
- 🔧 **DevOps**: Scripts y automatización
- 🏗️ **Sistemas distribuidos**: Comunicación entre servicios
- 🛡️ **Seguridad**: Entender como funcionan los procesos

---

## 📞 Contacto

[![42 Profile](https://img.shields.io/badge/42-migarrid-000000?style=flat&logo=42&logoColor=white)](https://profile.intra.42.fr/users/migarrid)

## 🌟 Parte Bonus

### 🔢 Múltiples Pipes en Detalle

La versión bonus permite conectar **cualquier número de comandos**. La fórmula es simple:
- **N comandos** = **N-1 pipes**
- **N procesos hijo** = **1 proceso por comando**

**Diferencias técnicas con la versión básica:**

| Aspecto | Básica | Bonus |
|---------|---------|-------|
| **Pipes** | 1 pipe fijo | N-1 pipes dinámicos |
| **Procesos** | 2 hijos fijos | N hijos dinámicos |
| **Memoria** | Stack arrays | Malloc dinámico |
| **Complejidad** | O(1) | O(N) |

### 📝 Here_doc: Entrada Interactiva

El here_doc es una funcionalidad avanzada que permite **entrada interactiva**:

**Sintaxis:**
```bash
./pipex_bonus here_doc LIMITADOR "cmd1" "cmd2" outfile
```

**Funcionamiento interno:**
1. **Detección**: Si argv[1] == "here_doc"
2. **Modo especial**: Cambia el comportamiento de entrada
3. **Recolección**: Lee líneas hasta encontrar el limitador
4. **Pipe temporal**: Almacena la entrada en un pipe
5. **Append mode**: El archivo de salida se abre en modo append (`>>`)

**Ejemplo visual:**
```
Usuario → heredoc> línea 1
         heredoc> línea 2
         heredoc> EOF      ← Limitador encontrado
                 ↓
         Pipe temporal contiene: "línea 1\nlínea 2\n"
                 ↓
         cmd1 lee del pipe → cmd2 → append a outfile
```

### 🔧 Gestión de Recursos Avanzada

**Memoria dinámica:**
- Array de pipes: `int **pipes_fd`
- Array de PIDs: `pid_t *pids`
- Liberación automática en caso de error

**File descriptors:**
- Cierre sistemático de todos los pipes no utilizados
- Prevención de leaks y deadlocks
- Manejo robusto de errores de apertura

## 🧪 Testing

### ✅ Tests Básicos

```bash
# Crear archivo de prueba
echo -e "hello world\nhello pipex\ngoodbye world\ntest line" > infile

# Test 1: Funcionamiento básico
./pipex_bonus infile "grep hello" "wc -l" outfile
cat outfile  # Debería mostrar: 2

# Test 2: Múltiples comandos
./pipex_bonus infile "cat" "sort" "uniq" "wc -l" outfile
cat outfile  # Debería mostrar: 4

# Test 3: Con archivos del sistema
./pipex_bonus /etc/passwd "head -5" "tail -1" "cut -d: -f1" outfile
```

### 🌟 Tests Here_doc

```bash
# Test here_doc básico
./pipex_bonus here_doc EOF "cat" "wc -l" outfile
test line 1
test line 2
EOF

# Test here_doc con múltiples comandos
./pipex_bonus here_doc STOP "grep test" "sort" "uniq" outfile
test apple
banana test
test zebra
STOP
```

### 🔍 Comparación con Shell

```bash
# Función de test automático
test_pipex() {
    echo "Test: $1 | $2 | $3"

    # Shell tradicional
    < infile $1 | $2 | $3 > expected.txt

    # Nuestro pipex
    ./pipex_bonus infile "$1" "$2" "$3" actual.txt

    # Comparar resultados
    if diff expected.txt actual.txt > /dev/null; then
        echo "✅ PASS"
    else
        echo "❌ FAIL"
    fi
}

# Ejecutar tests
test_pipex "grep hello" "wc -l" "cat"
test_pipex "cat" "sort" "head -3"
test_pipex "head -10" "tail -5" "wc -l"
```

## 🎓 Reflexiones y Aprendizajes

### 💪 Conceptos Dominados

1. **🔧 Programación de Sistemas**
   - Manejo de procesos con `fork()`, `waitpid()`, `execve()`
   - File descriptors y redirección de I/O con `dup2()`
   - Pipes para comunicación entre procesos

2. **🎯 Gestión de Recursos**
   - Apertura/cierre correcto de archivos y pipes
   - Prevención de memory leaks con malloc/free
   - Manejo de errores robusto y recuperación

3. **🏗️ Arquitectura de Software**
   - Separación de responsabilidades por módulos
   - Código modular y reutilizable entre versión básica y bonus
   - Escalabilidad de 2 comandos a N comandos

### 🚀 Habilidades Desarrolladas

- **🔍 Debugging**: Uso de herramientas como `strace`, `valgrind`, `gdb`
- **📖 Documentación**: Lectura intensiva de man pages (fork, execve, dup2, pipe)
- **🧪 Testing**: Creación de casos de prueba comparando con bash real
- **🛠️ Herramientas**: Makefiles avanzados, git, debugging de múltiples procesos

### 💡 Lecciones Aprendidas Clave

1. **La importancia del manejo de errores**: Cada syscall puede fallar, el programa debe ser resiliente
2. **Resource management**: Los FDs son recursos limitados, hay que cerrarlos correctamente
3. **Process synchronization**: Coordinar múltiples procesos es complejo pero poderoso
4. **UNIX philosophy**: "Do one thing and do it well" - cada proceso hace una tarea específica


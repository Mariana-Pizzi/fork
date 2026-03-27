// padre escribe del 2 al n
// hijo lee (2) y guarda los n° no multiplos de 2
// el padre escribe esos numeros
// hijo lee el siguiente numero (3) y guarda los n° no multiplos de 3
// el padre escribe esos numeros
// hijo lee el siguiente numero (5) y guarda los n° no multiplos de 5

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1
#define NUMERO_MINIMO_INGRESADO 2

void
crear_pipe(int pipe_fd[2])
{
	if (pipe(pipe_fd) < 0) {
		perror("Error en pipe");
		exit(1);
	}
}

int
crear_fork()
{
	int pid = fork();

	if (pid < 0) {
		printf("Error en fork %d\n", pid);
		exit(1);
	}

	return pid;
}

void
escribir(int fd_write, int valor)
{
	int escrito = write(fd_write, &valor, sizeof(valor));
	if (escrito < 0) {
		perror("write");
		exit(1);
	}
}

void
filtro(int fd_read)
{
	int primo;
	int leido = read(fd_read, &primo, sizeof(primo));

	if (leido <= 0) {
		close(fd_read);
		exit(0);
	}

	printf("primo %d\n", primo);

	int pipe_fd[2];
	crear_pipe(pipe_fd);

	int pid = crear_fork();

	if (pid == 0) {
		// hijo
		close(pipe_fd[WRITE]);
		close(fd_read);

		filtro(pipe_fd[READ]);

		close(pipe_fd[READ]);
		exit(0);

	} else {
		// padre
		close(pipe_fd[READ]);

		int num;
		while (read(fd_read, &num, sizeof(num)) > 0) {
			if (num % primo != 0) {
				escribir(pipe_fd[WRITE], num);
			}
		}
		close(fd_read);
		close(pipe_fd[WRITE]);
		wait(NULL);
	}
}


int
main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("Uso: %s <n>\n", argv[0]);
		exit(1);
	}

	int numero_ingresado = atoi(argv[1]);

	if (numero_ingresado < NUMERO_MINIMO_INGRESADO) {
		printf("El numero de entrada debe ser mayor o igual a 2\n");
		exit(1);
	}

	int pipe_fd[2];
	crear_pipe(pipe_fd);

	int pid = crear_fork();

	if (pid == 0) {
		// hijo (primer filtro)
		close(pipe_fd[WRITE]);
		filtro(pipe_fd[READ]);

	} else {
		// padre (generador)
		close(pipe_fd[READ]);

		for (int i = 2; i <= numero_ingresado; i++) {
			escribir(pipe_fd[WRITE], i);
		}

		close(pipe_fd[WRITE]);
		wait(NULL);
	}


	return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#ifndef NARGS
#define NARGS 4
#endif

int
main(int argc, char *argv[])
{
	char *comando = argv[1];
	char *buffer = NULL;
	size_t tamano = 0;

	char *argumentos[NARGS + 1];
	int cantidad = 0;

	while (getline(&buffer, &tamano, stdin) != -1) {
		buffer[strcspn(buffer, "\n")] = '\0';

		argumentos[cantidad] = strdup(buffer);
		if(argumentos[cantidad] == NULL) {
			perror("strdup");
			exit(1);
		}

		cantidad++;

		if( cantidad == NARGS) {
			char *argumentos_exec[NARGS + 2];
			argumentos_exec[0] = comando;

			for (int i = 0; i < cantidad; i++) {
				argumentos_exec[i + 1] = argumentos[i];
			}

			argumentos_exec[cantidad + 1] = NULL;

			int pid = fork();
			if(pid < 0) {
				perror("fork");
				exit(1);
			}

			if (pid == 0) {
				execvp(comando, argumentos_exec);
				perror("execvp");
				exit(1);
			} else {
				wait(NULL);
			}

			for (int i = 0; i < cantidad; i++) {
				free(argumentos[i]);
			}

			cantidad = 0;
		}
	}

	//cantidad que no llega a cumplir con NARGS
	if (cantidad > 0) {
		char *argumentos_exec[NARGS + 2];
		argumentos_exec[0] = comando;

		for (int i = 0; i < cantidad; i++) {
			argumentos_exec[i + 1] = argumentos[i];
		}

		argumentos_exec[cantidad + 1] = NULL;

		int pid = fork();
		if (pid < 0) {
			perror("fork");
			exit(1);
		}

		if (pid == 0) {
			execvp(comando, argumentos_exec);
			perror("execvp");
			exit(1);
		} else {
			wait(NULL);
		}

		for (int i = 0; i < cantidad; i++) {
			free(argumentos[i]);
		}
	}

	free(buffer);
	return 0;
}

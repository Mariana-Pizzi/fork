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

void filtro(int fds_read) {
	int primo;
	int leido = read(fds_read, &primo, sizeof(primo));

	if (leido <= 0) {
		close(fds_read);
		exit(1);
	}
	
	printf("primo %d\n", primo);

	int fds[2];

	if (pipe(fds) < 0) {
		perror("Error en pipe");
		exit(1);
	}

	int pid = fork();

	if (pid < 0) {
		printf("Error en fork %d\n", pid);
		exit(1);
	}

	if (pid == 0) {
		//hijo
		close(fds[1]);
		close(fds_read);
		
		filtro(fds[0]);
		
		close(fds[0]);
		exit(0);

	} else {
		//padre
		close(fds[0]);

		int num;
		while (read(fds_read, &num, sizeof(num)) > 0) {
			if (num % primo != 0) {
				if (write(fds[1], &num, sizeof(num)) < 0) {
					perror("write");
					exit(1);
				};
			}
		}
		close(fds_read);
		close(fds[1]);
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

	int numero_tope = atoi(argv[1]);

	if (numero_tope < 2) {
		printf("El numero de entrada debe ser mayor o igual a 2\n");
		exit(1);
	}

	int fds[2];

	if (pipe(fds) < 0) {
		perror("Error en pipe");
		exit(1);
	}

	int pid = fork();

	if (pid < 0) {
		printf("Error en fork %d\n", pid);
		exit(1);
	}

	if (pid == 0) {
		//hijo (primer filtro)
		close(fds[1]);
		filtro(fds[0]);

	} else {
		//padre (generador)
		close(fds[0]);
		
		for (int i = 2; i <=numero_tope; i++){
			if (write(fds[1], &i, sizeof(i)) < 0) {
				perror("write");
				exit(1);
			};
		}

		close(fds[1]);
		wait(NULL);
	}


	return 0;
}
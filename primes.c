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

int
main(int argc, char *argv[])
{
	int READ = 0;
	int WRITE = 1;

	int fds[2];
	pipe(fds);

	int pid = fork();

	if (pid < 0) {
		printf("Error en fork %d\n", pid);
		exit(-1);
	}

	if (pid == 0) {
		//hijo (primer filtro)
		close(fds[WRITE]);

		int primo;
		int leido = read(fds[READ], &primo, sizeof(primo));

		while(leido > 0){
			printf("Primo: %d\n", primo);

			int fds_1[2];
			pipe(fds_1);

			int pid2 = fork();

			if (pid2 < 0) {
				printf("Error en fork %d\n", pid2);
				exit(-1);
			}

			if (pid2 == 0) {
				//siguiente filtro
				close(fds_1[WRITE]);
				close(fds[READ]);

				fds[READ] = fds_1[READ];
				leido = read(fds[READ], &primo, sizeof(primo));
			} else {
				//padre sig filtro
				close(fds_1[READ]);

				int num;
				while (read(fds[READ], &num, sizeof(num)) > 0) {
					if (num % primo != 0) {
						write(fds_1[WRITE], &num, sizeof(num));
					}
				}
				close(fds_1[WRITE]);
				close(fds[READ]);
				wait(NULL);

				leido = 0;
			}
		}

	} else {
		//padre (generador)
		close(fds[READ]);

		int N = 100;
		
		for (int i = 2; i <=N; i++){
			write(fds[WRITE], &i, sizeof(i));
		}

		close(fds[WRITE]);
		wait(NULL);
	}


	return 0;
}
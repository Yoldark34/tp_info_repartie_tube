#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int tube[2];
char buf[20];
char taille[1];
pid_t pid;
int etat;

void trtSIGPIPE (int signalRecu, siginfo_t* info, void* pasUtileIci)
 {
  printf("Je receptionne le signal %d envoye par le processus %d\n", 
         signalRecu, info->si_pid); 
  printf("J'envoie le signal SIGTERM à ce processus\n");
  kill (info->si_pid, SIGTERM);
  fflush(stdout);
 }
void trtSIGTERM (int signalRecu, siginfo_t* info, void* pasUtileIci)
 {
  printf("Je receptionne le signal %d envoye par le processus %d\n", 
         signalRecu, info->si_pid); 
  printf("Je m'arrête avec la valeur 50\n");  
  fflush(stdout);
  exit (50);
 }

void trtSIGUSR1 (int signalRecu, siginfo_t* info, void* pasUtileIci)
 {
  printf("Je receptionne le signal %d envoye par le processus %d\n", 
         signalRecu, info->si_pid); 
	read(tube[0], taille, 1);
	printf("taille %s bien recu\n", taille);
	read(tube[0], buf, (int)(taille[0]));
	printf("%s bien recu\n", buf);
  printf("J'envoie le signal SIGUSR2 à ce processus\n");
  kill (info->si_pid, SIGUSR2);
  fflush(stdout);
 }

void trtSIGUSR2 (int signalRecu, siginfo_t* info, void* pasUtileIci)
 {
  printf("Je receptionne le signal %d envoye par le processus %d\n", 
         signalRecu, info->si_pid); 
  printf("J'envoie le signal SIGTERM à ce processus\n");
  kill (info->si_pid, SIGTERM);
  fflush(stdout);
 }

int main (int argc, char** argv) {

	
	struct sigaction pourSIGPIPE;
			pourSIGPIPE.sa_sigaction=&trtSIGPIPE;
			pourSIGPIPE.sa_flags=SA_SIGINFO;
	
	sigemptyset(&pourSIGPIPE.sa_mask);
	sigaction(SIGPIPE, &pourSIGPIPE, NULL);

	struct sigaction pourSIGTERM;
			pourSIGTERM.sa_sigaction = &trtSIGTERM;
			pourSIGTERM.sa_flags=SA_SIGINFO;

	sigemptyset(&pourSIGTERM.sa_mask);
  	sigaction(SIGTERM, &pourSIGTERM, NULL);

	struct sigaction pourSIGUSR1;
			pourSIGUSR1.sa_sigaction = &trtSIGUSR1;
			pourSIGUSR1.sa_flags=SA_SIGINFO;

	sigemptyset(&pourSIGUSR1.sa_mask);
  	sigaction(SIGUSR1, &pourSIGUSR1, NULL);

	struct sigaction pourSIGUSR2;
			pourSIGUSR2.sa_sigaction = &trtSIGUSR2;
			pourSIGUSR2.sa_flags=SA_SIGINFO;

	sigemptyset(&pourSIGUSR2.sa_mask);
  	sigaction(SIGUSR2, &pourSIGUSR2, NULL);

	pipe(tube);

	pid_t pid = fork ();
	pid_t pid_pere;
  
	switch (pid)
	{
	case -1 : printf ("Erreur dans la creation du processus fils.\n"); perror ("Erreur");
		break;
	case  0 : printf ("Fils : PID=%d\n", getpid()); 
		close(tube[1]);
		while(1);
	default : printf ("Pere : PID=%d\n", getpid());
		sleep(2);
		printf ("Pere : close 0\n");
		close(tube[0]);
		printf ("Pere : envoi de Bonjour\n");
		write(tube[1], "7", 1);
		write(tube[1], "Bonjour", 7);
		
		printf ("Pere : envoi de signal sigusr1 au Fils : %d\n", pid);
		kill(pid, SIGUSR1);
		
		pid = wait(&etat);
		printf("Pere : fils %d s'arrete avec la valeur %d\n", pid, &etat);
		break;
	}
	return 0;
}

/* A timer that counts actual seconds, not seconds running.
   Meant to keep track 
*/







#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#include "letters.h"
#define CHECK_INTERVAL 1
#define BASIC 0


void sig_digit(int n, int i, char *s) {
		if (n >= 10)
				sig_digit(n / 10, i, s);
		strcat(s, font_numbers[n % 10][i]);
		strcat(s, " ");
}


void pretty_print(time_t diff) {
		int h = diff / 3600;
		int m = (diff / 60) % 60;
		int s = diff % 60;
		int i;
		char str[ROW][255];

		if (BASIC) {
				fflush(stdout);
				if (h != 0 && m == 0) { 
						printf("%d seconds remaining\r", s);
				} else if (h == 0) {
						printf("%dm : %ds remaining\r", m, s);
				} else {
						printf("%dh : %dm : %ds remaining\r", h, m, s);
				}
		} else {
				/* Fancy Printing */
				for (i=0; i<ROW; i++) {
	                 	strcpy(str[i],"");
						if (h!=0) {
								sig_digit(h, i, str[i]);
								strcat(str[i], font_h[i]);
								strcat(str[i], "  ");
						}
						if (h!=0 || m!=0) {
								sig_digit(m, i, str[i]);
								strcat(str[i], font_m[i]);
								strcat(str[i], "  ");
						}
						sig_digit(s, i, str[i]);
						strcat(str[i], font_s[i]);
				}

				printf("\033[2J");
				for (i=0; i<ROW; i++)
						printf("%s\n", str[i]);
/*				
				printf("\033[XA"); // Move up X lines;
				printf("\033[XB"); // Move down X lines;
				printf("\033[XC"); // Move right X column;
				printf("\033[XD"); // Move left X column;
				printf("\033[2J"); // Clear screen
*/
		}
}

void bring_cursor_back() {
		printf("\e[?25h");
		exit(1);
}

int main(int argc, char *argv[]){
		time_t now = time(0);
		int hour, min;
		time_t future;
		int check;

	    signal(SIGINT, bring_cursor_back);			 

		/*  Arg parsing */
		if (argc == 1) {
				/* No argument */
				printf("usage: timer %%dh %%dm\n\nif one number is provided, it will be assumed to be minutes.");
				return 0;
		} else if (argc == 2) {
				/* Minute */
				check = sscanf(argv[1], "%dm", &min);
				if (check != 1) {
						check = sscanf(argv[1], "%d", &min);
				}
				if (check != 1) {
						fprintf( stderr, "%s", "invalid argument (1)");
						return 1;
				}
		} else if (argc == 3) {
				check = sscanf(argv[1], "%dh", &hour);
				if (check != 1) {
						fprintf( stderr, "%s", "invalid argument (2)");
						return 1;
				}
				check = sscanf(argv[2], "%dm", &min);
				if (check != 1) {
						fprintf( stderr, "%s", "invalid argument (3)");
						return 1;
				}
		}


		/* Anything crazy happening? */
		future = now + 3600 * hour + 60 * min;
		if (future < now) {
				fprintf( stderr, "%s", "Can't count into the past.");
				return 1;
		}

		printf("\e[?25l");
		printf("\033[2J");
		now = time(0);
		while (future > (now=time(0))) {
				pretty_print(future - now);
				sleep (CHECK_INTERVAL);
		}
		return 0;
}

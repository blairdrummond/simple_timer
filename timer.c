/* A timer that counts actual seconds, not seconds running. 

   - Blair Drummond. 2019
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "letters.h"
#define CHECK_INTERVAL 1
#define BASIC 0


void
sig_digit(int n, int i, char *s)
{
	/* Print numbers one digit at a time */
	if (n >= 10)
		sig_digit(n / 10, i, s);

	strcat(s, font_numbers[n % 10][i]);
	strcat(s, " ");
}


void
pretty_print(time_t diff)
{
	int h = diff / 3600;
	int m = (diff / 60) % 60;
	int s = diff % 60;
	int i;
	char str[ROW][255];

	if (BASIC) {
		fflush(stdout);
		if (h != 0 && m == 0)
			printf("%d seconds remaining\r", s);
		else if (h == 0)
			printf("%dm : %ds remaining\r", m, s);
		else
			printf("%dh : %dm : %ds remaining\r", h, m, s);
	} else {
		/* Fancy Printing: create each line of text */
		for (i = 0; i < ROW; i++) {
			strcpy(str[i],"");
			if (h != 0) {
				sig_digit(h, i, str[i]);
				strcat(str[i], font_h[i]);
				strcat(str[i], "  ");
			}
			if (h != 0 || m != 0) {
				sig_digit(m, i, str[i]);
				strcat(str[i], font_m[i]);
				strcat(str[i], "  ");
			}
			sig_digit(s, i, str[i]);
			strcat(str[i], font_s[i]);
		}

		/* Clear Screen */
		printf("\033[2J");
		/* Draw each line */
		for (i = 0; i < ROW; i++)
			printf("%s\n", str[i]);
	}
}


/* Catch Ctrl-C and restore cursor */
void
bring_cursor_back()
{
	printf("\e[?25h");
	exit(1);
}


int
main(int argc, char *argv[])
{
	time_t now = time(0);
	int hour, min;
	time_t future;
	int check;

	/* Catch Ctrl-C and restore cursor */
	signal(SIGINT, bring_cursor_back);

	/*  Arg parsing */
	switch (argc) {
	case 1:
		/* No argument */
		printf("usage: timer %%dh %%dm\n\nif one number is provided, it will be assumed to be minutes.");
		return 0;

	case 2:
		/* Minute */
		check = sscanf(argv[1], "%dm", &min);
		if (check != 1)
			check = sscanf(argv[1], "%d", &min);
		
		if (check != 1) {
			fprintf(stderr, "invalid argument (1)");
			return 1;
		}
		break;
		
	case 3:
		check = sscanf(argv[1], "%dh", &hour);
		if (check != 1) {
			fprintf(stderr, "invalid argument (2)");
			return 1;
		}
		check = sscanf(argv[2], "%dm", &min);
		if (check != 1) {
			fprintf(stderr, "invalid argument (3)");
			return 1;
		}
		break;
		
	default:
		fprintf(stderr, "Too many arguments!");
		return 1;
	}

	/* Anything crazy happening? */
	future = now + 3600 * hour + 60 * min;
	if (future < now) {
		fprintf(stderr, "Can't count into the past.");
		return 1;
	}

	/* Make Cursor Invisible */
	printf("\e[?25l");

	/* Flush the screen */
	printf("\033[2J");

	while (future > (now = time(0))) {
		pretty_print(future - now);
		sleep(CHECK_INTERVAL);
	}

	/* Bring back the cursor */
	printf("\e[?25h");
        sleep(1);
	return 0;
}

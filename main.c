#include <stdio.h>
#include <math.h>

#ifdef __unix__
	#include <termios.h>
	#include <sys/ioctl.h>
	#include <unistd.h>
	#include <stdlib.h>

	char *colors[] = {"\x1B[31m", "\x1B[33m","\x1B[32m","\x1B[36m","\x1B[34m","\x1B[35m","\x1B[37m","\x1B[0m"};
	
	static struct termios oldt,newt;
	
	void getConsoleSize(int *x, int *y) {
		struct winsize w;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
		*y = w.ws_row;
		*x = w.ws_col;
	}
	void setupConsole(void){
		tcgetattr(STDIN_FILENO, &oldt);
		newt = oldt;
		newt.c_lflag &= ~(ICANON | ECHO);
    	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	}
	void restoreConsole(void){
		tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	}
	int handleInput(double *zoom, double *posX, double *posY, int *iter){
		char c;
		while((c = getchar()) != 'q'){
			switch(c){
				case '\033':
					getchar(); // skipping the '['
					c = getchar();
					switch(c){
						case 'A':
							*posY -= 0.1/(*zoom);
							break;
						case 'B':
							*posY += 0.1/(*zoom);
							break;
						case 'C':
							*posX += 0.1/(*zoom);
							break;
						case 'D':
							*posX -= 0.1/(*zoom);
							break;
					}
					break;
				case '-':
				case 's':
					*zoom *= 0.9;
					break;
				case '+':
				case 'x':
					*zoom *= 1.1;
					break;
				case 'a':
					*iter *= 2;
					break;
				case 'z':
					if(*iter==4) break;
					*iter /= 2;
					break;
			}
			return 1;
		}
		return 0;
	}
	
	void clearScrn(void){
		system("clear");
	}
#else
	#error "Not supported!"
#endif

int mandelbrotCalc(double cr, double ci, int steps) {
	int i = 0;

	double zi = 0.0f, zr = 0.0f;
	while(i < steps && zr*zr+zi*zi<4.0) {
		double temp = zr*zr-zi*zi+cr;
		zi = zr * 2.0 * zi + ci;
		zr = temp;
		i++;
	}

	return i;
}

double mapToReal(int x, int resx, double minr, double maxr) {
	double range = maxr-minr;
	return x*(range/resx)+minr;
}

double mapToImag(int x, int resx, double mini, double maxi) {
	double range = maxi-mini;
	return x*(range/resx)+mini;
}

int main(int argc, char *argv[]) {
	int winW, winH, steps = 512;
	double zoom = 1.0f, posX = 0.0f, posY = 0.0f;
	getConsoleSize(&winW, &winH);
	setupConsole();

	do{
	clearScrn();
	for(int y = 0; y < winH; y++) {
	putchar('\n');
	for(int x = 0; x < winW; x++) {
		double ci = mapToImag(y, winH, -1.0/zoom+posY, 1.0/zoom+posY);
		double cr = mapToReal(x, winW, -2.0/zoom+posX, 1.0/zoom+posX);
		int n = mandelbrotCalc(cr, ci, steps);
	
		if(n!=steps) {
			printf("%s#%s",colors[(int)(log(n)/log(2))%7], colors[7]);
		} else putchar(' ');
	} 	
	}
	} while(handleInput(&zoom, &posX, &posY, &steps));	
	restoreConsole();
	return 0;
}

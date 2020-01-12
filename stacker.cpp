/*
 * @author kool catz pty ltd
*/

#include "MicroBit.h"

#define WIDTH 5
#define HEIGHT 5
#define MID_PIX (WIDTH/2)
#define LEFT -1
#define RIGHT 1
#define START_SPEED 300
#define SPEED_FACTOR 0.7
#define PIXEL_ON 255
#define PIXEL_OFF 0

MicroBit uBit;
MicroBitImage screen;

int c_row;
int speed;
int col;
int dir;

void init() {
	screen = MicroBitImage(WIDTH, HEIGHT);
	c_row = HEIGHT - 1;
	speed = START_SPEED;
	col = uBit.random(WIDTH);
	dir = RIGHT;
}

inline int centre_score(int x) {
	return 2 - abs(MID_PIX - x);
}

int row_score(int x, int y) {
	return centre_score(x)*(HEIGHT - y);

}

inline int score_to_brightness(int d) {
	switch (d) {
		case 2:
			return 255;
		case 1:
			return 50;
		case 0:
			return 5;
	}

	return 0;
}

int score() {
	int running_total = 0;
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			int on = screen.getPixelValue(x, y) != PIXEL_OFF;
			if (on) {
				running_total += row_score(x, y);
				break;
			}
		}
	}
	return running_total;
}

void reset() {
	uBit.sleep(2000);	
	uBit.display.scroll("SCORE");
	uBit.display.scroll(score());
	init();
}

void stop(MicroBitEvent) {
	screen.setPixelValue(col, c_row, score_to_brightness(centre_score(col)));

	// reset to the next row
	c_row -= 1;
	col = uBit.random(WIDTH);
	dir = RIGHT;
	speed *= SPEED_FACTOR;
}


int main() {
    uBit.init();
    uBit.seedRandom();
    uBit.display.setDisplayMode(DISPLAY_MODE_GREYSCALE);
    init();

    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, stop);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, stop);

    while (1) {
	// game over?
	if (c_row == -1) {
		reset();	
	}
	
	screen.setPixelValue(col, c_row, score_to_brightness(centre_score(col)));
	uBit.display.print(screen);
        uBit.sleep(speed);
	screen.setPixelValue(col, c_row, PIXEL_OFF);
	
	if (dir == LEFT && col == 0) {
		dir = RIGHT;
	} else if (dir == RIGHT && col == WIDTH - 1) {
		dir = LEFT;
	}
	col += dir;
    }
}


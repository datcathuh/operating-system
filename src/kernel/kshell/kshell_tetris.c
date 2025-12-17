#include "keyboard.h"
#include "kshell.h"
#include "kshell_tetris.h"
#include "memory.h"
#include "pit.h"
#include "string.h"
#include "types.h"
#include "video/vga.h"
#include "video/video.h"

#define PLAY_WIDTH 10
#define PLAY_HEIGHT 20
#define CELL_SIZE 16 /* pixels per cell */
#define BOARD_X 20   /* top-left pixel x of board */
#define BOARD_Y 20   /* top-left pixel y of board */

#define BG_COLOR 0x00101010 /* dark background */
#define GRID_COLOR 0x00404040
#define BORDER_COLOR 0x00FFFFFF
#define TEXT_COLOR 0x00FFFFFF

static const uint32_t tetro_colors[8] = {
	0x00000000, /* none */
	0x00FF0000, /* red */
	0x0000FF00, /* green */
	0x000000FF, /* blue */
	0x00FFFF00, /* yellow */
	0x00FF00FF, /* magenta */
	0x0000FFFF, /* cyan */
	0x00FFA500  /* orange */
};

/* Tetromino shapes encoded as 4x4 bitmaps per rotation (0..3).
   Each entry is 4 rows, low 4 bits used. Order: I, J, L, O, S, T, Z
*/
static const uint16_t tetrominoes[7][4] = {
	/* I */
	{0x0F00, /* 0000
                1111
                0000
                0000 */
     0x2222, /* 0010
                0010
                0010
                0010 */
     0x0F00, 0x2222},
	/* J */
	{
		0x8E00, /* 1000
                   1110
                   0000
                   0000 */
		0x6440, /* 0110
                   0100
                   0100
                   0000 */
		0x0E20, /* 0000
                   1110
                   0010
                   0000 */
		0x44C0  /* 0100
                   0100
                   1100
                   0000 */
	},
	/* L */
	{
		0x2E00, /* 0010
                   1110
                   0000
                   0000 */
		0x4460, /* 0100
                   0100
                   0110
                   0000 */
		0x0E80, /* 0000
                   1110
                   1000
                   0000 */
		0xC440  /* 1100
                   0100
                   0100
                   0000 */
	},
	/* O */
	{0x6600, /* 0110
                0110
                0000
                0000 */
     0x6600, 0x6600, 0x6600},
	/* S */
	{
		0x3600, /* 0011
                   0110
                   0000
                   0000 */
		0x4620, /* 0100
                   0110
                   0010
                   0000 */
		0x3600, /* 0011
                   0110
                   0000
                   0000 */
		0x4620  /* 0100
                   0110
                   0010
                   0000 */
	},
	/* T */
	{
		0x4E00, /* 0100
                   1110
                   0000
                   0000 */
		0x8C80, /* 1000
                   1100
                   1000
                   0000 */
		0xE400, /* 1110
                   0100
                   0000
                   0000 */
		0x4C40  /* 0100
                   1100
                   0100
                   0000 */
	},
	/* Z */
	{
		0x6C00, /* 0110
                   1100
                   0000
                   0000 */
		0x8C40, /* 1000
                   1100
                   0100
                   0000 */
		0x6C00, /* 0110
                   1100
                   0000
                   0000 */
		0x8C40  /* 1000
                   1100
                   0100
                   0000 */
	}};

struct piece {
	int type;     /* 0..6 */
	int rotation; /* 0..3 */
	int x, y;     /* top-left of 4x4 area on board coordinates (x cell) */
};

/* Game board: 0 means empty, 1..7 map to tetro_colors */
static uint8_t board[PLAY_HEIGHT][PLAY_WIDTH];

static struct video_device *vd = NULL;
static uint8_t *_font = NULL;

static void draw_cell(int cx, int cy, uint32_t color) {
	int px = BOARD_X + cx * CELL_SIZE;
	int py = BOARD_Y + cy * CELL_SIZE;
	video_draw_rect_filled(vd->buffer, px + 1, py + 1, CELL_SIZE - 2,
	                       CELL_SIZE - 2, color);
	video_draw_rect(vd->buffer, px, py, CELL_SIZE, CELL_SIZE, GRID_COLOR);
}

static void draw_board(void) {
	/* background rectangle for board */
	int bw = PLAY_WIDTH * CELL_SIZE;
	int bh = PLAY_HEIGHT * CELL_SIZE;
	video_draw_rect_filled(vd->buffer, BOARD_X - 4, BOARD_Y - 4, bw + 8, bh + 8,
	                       BG_COLOR);
	video_draw_rect(vd->buffer, BOARD_X - 4, BOARD_Y - 4, bw + 8, bh + 8,
	                BORDER_COLOR);

	for (int y = 0; y < PLAY_HEIGHT; ++y) {
		for (int x = 0; x < PLAY_WIDTH; ++x) {
			uint8_t v = board[y][x];
			if (v)
				draw_cell(x, y, tetro_colors[v]);
			else {
				/* empty cell draw slightly dark square */
				draw_cell(x, y, BG_COLOR);
			}
		}
	}
}

/* draw piece on top of board (preview) */
static void draw_piece(const struct piece *p, bool erase) {
	uint16_t shape = tetrominoes[p->type][p->rotation & 3];
	uint32_t color = erase ? BG_COLOR : tetro_colors[p->type + 1];
	for (int ry = 0; ry < 4; ++ry) {
		for (int rx = 0; rx < 4; ++rx) {
			if (shape & (0x8000 >> (ry * 4 + rx))) {
				int bx = p->x + rx;
				int by = p->y + ry;
				if (bx >= 0 && bx < PLAY_WIDTH && by >= 0 && by < PLAY_HEIGHT) {
					draw_cell(bx, by, color);
				}
			}
		}
	}
}

/* Check collision of piece with board or bounds. Returns true if colliding. */
static bool piece_collides(const struct piece *p,
                           const uint8_t test_board[PLAY_HEIGHT][PLAY_WIDTH]) {
	uint16_t shape = tetrominoes[p->type][p->rotation & 3];
	for (int ry = 0; ry < 4; ++ry) {
		for (int rx = 0; rx < 4; ++rx) {
			if (shape & (0x8000 >> (ry * 4 + rx))) {
				int bx = p->x + rx;
				int by = p->y + ry;
				if (bx < 0 || bx >= PLAY_WIDTH || by >= PLAY_HEIGHT) {
					return true;
				}
				if (by >= 0) { /* allow above board during spawn */
					if (test_board[by][bx]) {
						return true;
					}
				}
			}
		}
	}
	return false;
}

/* Lock piece into board (copy cells) */
static void lock_piece(const struct piece *p) {
	uint16_t shape = tetrominoes[p->type][p->rotation & 3];
	for (int ry = 0; ry < 4; ++ry) {
		for (int rx = 0; rx < 4; ++rx) {
			if (shape & (0x8000 >> (ry * 4 + rx))) {
				int bx = p->x + rx;
				int by = p->y + ry;
				if (bx >= 0 && bx < PLAY_WIDTH && by >= 0 && by < PLAY_HEIGHT) {
					board[by][bx] = (uint8_t)(p->type + 1);
				}
			}
		}
	}
}

/* Clear full lines, return number cleared */
static int clear_lines(void) {
	int cleared = 0;
	for (int y = PLAY_HEIGHT - 1; y >= 0; --y) {
		bool full = true;
		for (int x = 0; x < PLAY_WIDTH; ++x) {
			if (!board[y][x]) {
				full = false;
				break;
			}
		}
		if (full) {
			/* move everything above down one */
			for (int yy = y; yy > 0; --yy) {
				mem_copy(board[yy], board[yy - 1], PLAY_WIDTH);
			}
			/* clear top row */
			for (int xx = 0; xx < PLAY_WIDTH; ++xx) {
				board[0][xx] = 0;
			}
			++cleared;
			++y; /* re-check same y because rows moved down */
		}
	}
	return cleared;
}

/* Spawn a new random piece. For determinism in kernel context we use a simple
 * LCG. */
static uint32_t rng_seed = 1234567;
static int rand_range(int n) {
	rng_seed = rng_seed * 1664525u + 1013904223u;
	return (int)(rng_seed % (uint32_t)n);
}

static void spawn_piece(struct piece *p) {
	p->type = rand_range(7);
	p->rotation = 0;
	p->x = (PLAY_WIDTH / 2) - 2;
	p->y = -1; /* start above board so initial drop is natural */
}

/* draw HUD: score, controls */
static void draw_hud(int score, int level, int next_type) {
	char buf[64];
	int sx = BOARD_X + PLAY_WIDTH * CELL_SIZE + 20;
	int sy = BOARD_Y;
	/* background */
	video_draw_rect_filled(vd->buffer, sx - 8, sy - 8, 200, 200, BG_COLOR);
	/* Score */
	str_copy(buf, 64, "Score: ");
	// TODO: Set correct string length
	str_from_uint32(buf + str_length(buf), 64, score);
	video_draw_string(vd->buffer, _font, sx, sy, buf, TEXT_COLOR, BG_COLOR, 1);
	str_copy(buf, 64, "Level: ");
	// TODO: Set correct string length
	str_from_uint32(buf + str_length(buf), 64, level);
	video_draw_string(vd->buffer, _font, sx, sy + 18, buf, TEXT_COLOR, BG_COLOR,
	                  1);
	video_draw_string(vd->buffer, _font, sx, sy + 36, "Controls:", TEXT_COLOR,
	                  BG_COLOR, 1);
	video_draw_string(vd->buffer, _font, sx, sy + 54, "a: left  d: right",
	                  TEXT_COLOR, BG_COLOR, 1);
	video_draw_string(vd->buffer, _font, sx, sy + 72, "s: down  w: rot",
	                  TEXT_COLOR, BG_COLOR, 1);
	video_draw_string(vd->buffer, _font, sx, sy + 90, "space: hard drop",
	                  TEXT_COLOR, BG_COLOR, 1);
	video_draw_string(vd->buffer, _font, sx, sy + 108, "q: quit", TEXT_COLOR,
	                  BG_COLOR, 1);

	/* next piece preview */
	video_draw_string(vd->buffer, _font, sx, sy + 130, "Next:", TEXT_COLOR,
	                  BG_COLOR, 1);
	/* draw next piece in a 4x4 block */
	int px = sx;
	int py = sy + 150;
	uint16_t shape = tetrominoes[next_type][0];
	for (int ry = 0; ry < 4; ++ry) {
		for (int rx = 0; rx < 4; ++rx) {
			if (shape & (0x8000 >> (ry * 4 + rx))) {
				/* map to a small 8x8 cell box */
				video_draw_rect_filled(vd->buffer, px + rx * 8, py + ry * 8, 7,
				                       7, tetro_colors[next_type + 1]);
			} else {
				video_draw_rect_filled(vd->buffer, px + rx * 8, py + ry * 8, 7,
				                       7, BG_COLOR);
			}
		}
	}
}

void kshell_tetris_cb(void) {
	vd = video_current();
	if (!vd)
		return;

	_font = vga_font();

	/* clear board */
	mem_set(board, 0, sizeof(board));

	int score = 0;
	int level = 1;
	int lines_total = 0;

	struct piece cur;
	struct piece test;
	spawn_piece(&cur);
	int next_piece = rand_range(7);

	bool running = true;

	/* initial draw */
	video_draw_rect_filled(vd->buffer, 0, 0, vd->buffer->resolution.width,
	                       vd->buffer->resolution.height, BG_COLOR);
	draw_board();
	draw_hud(score, level, next_piece);
	draw_piece(&cur, false);

	while (running) {
		/* Poll user input repeatedly until next gravity tick.
		   Since we only have pit_wait_seconds, we do:
		     - poll keys and act
		     - then wait 1 second for gravity tick
		   If you later implement a millisecond tick, replace this behavior.
		*/
		bool lock_now = false;

		/* Input polling: check once before each gravity tick */
		char k;
		bool key_pressed = keyboard_get_key_if_exists(&k);
		while (key_pressed) {
			if (k == 'a') {
				mem_copy(&test, &cur, sizeof(cur));
				test.x -= 1;
				if (!piece_collides(&test, board)) {
					draw_piece(&cur, true);
					cur.x = test.x;
					draw_piece(&cur, false);
				}
			} else if (k == 'd') {
				mem_copy(&test, &cur, sizeof(cur));
				test.x += 1;
				if (!piece_collides(&test, board)) {
					draw_piece(&cur, true);
					cur.x = test.x;
					draw_piece(&cur, false);
				}
			} else if (k == 's') {
				/* soft drop */
				mem_copy(&test, &cur, sizeof(cur));
				test.y += 1;
				if (!piece_collides(&test, board)) {
					draw_piece(&cur, true);
					cur.y = test.y;
					draw_piece(&cur, false);
				} else {
					/* cannot move down => lock */
					// lock_now = true;
				}
			} else if (k == 'w') {
				/* rotate clockwise */
				mem_copy(&test, &cur, sizeof(cur));
				test.rotation = (test.rotation + 1) & 3;
				/* wall kicks: try center, left, right */
				bool ok = false;
				if (!piece_collides(&test, board))
					ok = true;
				else {
					test.x = cur.x - 1;
					if (!piece_collides(&test, board))
						ok = true;
					else {
						test.x = cur.x + 1;
						if (!piece_collides(&test, board))
							ok = true;
					}
				}
				if (ok) {
					draw_piece(&cur, true);
					cur = test;
					draw_piece(&cur, false);
				}
			} else if (k == ' ') {
				/* hard drop */
				draw_piece(&cur, true);
				while (1) {
					mem_copy(&test, &cur, sizeof(cur));
					test.y += 1;
					if (!piece_collides(&test, board)) {
						cur.y = test.y;
					} else
						break;
				}
				draw_piece(&cur, false);
				lock_now = true;
			} else if (k == 'q') {
				running = false;
			}
			/* redraw HUD */
			draw_hud(score, level, next_piece);
			key_pressed = keyboard_get_key_if_exists(&k);
		}

		if (running) {
			pit_wait_milliseconds(300);
		}

		/* gravity: move down 1 cell if possible, else lock */
		if (!lock_now) {
			mem_copy(&test, &cur, sizeof(cur));
			test.y += 1;
			if (!piece_collides(&test, board)) {
				draw_piece(&cur, true);
				cur.y = test.y;
				draw_piece(&cur, false);
			} else {
				lock_now = true;
			}
		}

		if (lock_now) {
			/* lock piece in place */
			lock_piece(&cur);
			draw_board();
			/* clear lines */
			int cleared = clear_lines();
			if (cleared) {
				/* scoring: simple formula */
				int add = 0;
				if (cleared == 1)
					add = 100;
				else if (cleared == 2)
					add = 300;
				else if (cleared == 3)
					add = 500;
				else if (cleared >= 4)
					add = 800;
				score += add * level;
				lines_total += cleared;
				/* level up for every 10 lines */
				level = 1 + (lines_total / 10);
			}
			/* spawn next */
			cur.type = next_piece;
			cur.rotation = 0;
			cur.x = (PLAY_WIDTH / 2) - 2;
			cur.y = -1;
			next_piece = rand_range(7);

			/* if new piece collides immediately -> game over */
			if (piece_collides(&cur, board)) {
				/* draw Game Over */
				const char *go = "GAME OVER (q to quit)";
				int cx = BOARD_X + (PLAY_WIDTH * CELL_SIZE) / 2 - 80;
				int cy = BOARD_Y + (PLAY_HEIGHT * CELL_SIZE) / 2 - 8;
				video_draw_string(vd->buffer, _font, cx, cy, go, TEXT_COLOR,
				                  BG_COLOR, 1);
				/* wait for 'q' */
				while (1) {
					char k2 = keyboard_get_key();
					if (k2 == 'q') {
						running = false;
						break;
					}
				}
			} else {
				draw_piece(&cur, false);
			}

			/* redraw HUD */
			draw_hud(score, level, next_piece);
		}
	} /* main loop */

	/* exit: clear area and return control */
	video_draw_rect_filled(vd->buffer, 0, 0, vd->buffer->resolution.width,
	                       vd->buffer->resolution.height, BG_COLOR);
}

void kshell_tetris_register() {
	struct kshell_command cmd = {.name = "tetris",
	                             .callback = kshell_tetris_cb};
	kshell_register_command(&cmd);
}

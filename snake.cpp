#include <iostream>
#include <unistd.h>
#include <ncurses.h>
#include <list>
using namespace std;

struct snakeSegment
{
    int x, y;
};

int main()
{
    srand(time(nullptr));
    while (1)
    {

        int max_h, max_w;
        int border_size = 2;
        int Score = 0;
        unsigned int ch;
        bool isDead = false;
        bool restartGame = false;
        bool gameOver = false;
        int FoodX = 10;
        int FoodY = 20;
        enum Direction
        {
            up = 0,
            right = 1,
            down = 2,
            left = 3
        };
        Direction snakeDirection = up;
        Direction prevDirection = snakeDirection;
        char gameover[] = "   _____          __  __ ______    ______      ________ _____    _ \n"
                          "  / ____|   /\\   |  \\/  |  ____|  / __ \\ \\    / /  ____|  __ \\  | |\n"
                          " | |  __   /  \\  | \\  / | |__    | |  | \\ \\  / /| |__  | |__) | | |\n"
                          " | | |_ | / /\\ \\ | |\\/| |  __|   | |  | |\\ \\/ / |  __| |  _  /  | |\n"
                          " | |__| |/ ____ \\| |  | | |____  | |__| | \\  /  | |____| | \\ \\  |_| \n"
                          "  \\_____/_/    \\_\\_|  |_|______|  \\____/   \\/   |______|_|  \\_\\ (_) \n";

        initscr();
        curs_set(0);
        noecho();
        cbreak();
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);

        getmaxyx(stdscr, max_h, max_w);
        const int ARENA_HEIGHT = max_h * 0.9 - 2 * border_size;
        const int ARENA_WIDTH = max_w * 0.9 - 2 * border_size;
        int score_win_h = 0.1 * max_h - 2 * border_size;
        int score_win_w = ARENA_WIDTH / 2;
        int score_win_x = (max_w - score_win_w) / 2;
        int win_w = (max_w - ARENA_WIDTH) / 2;
        int win_h = score_win_h + 4 * border_size;
        WINDOW *score_win = newwin(score_win_h, score_win_w, 0, score_win_x);
        WINDOW *win = newwin(ARENA_HEIGHT, ARENA_WIDTH, win_h, win_w);
        int win_x = getmaxx(win);
        int win_y = getmaxy(win);
        nodelay(win, true);
        keypad(win, true);

        // Drawing the scoreBoard
        wattron(score_win, COLOR_PAIR(1));
        box(score_win, ACS_VLINE, ACS_HLINE);
        wattroff(score_win, COLOR_PAIR(1));
        mvwprintw(score_win, 1, (score_win_w - 23) / 2, "Welcome to Snake Game!");
        mvwprintw(score_win, 5, (score_win_w - 20) / 2, "Created by pakosaan");

        // Snake Segment
        list<snakeSegment>
            snake = {{60, 15}, {61, 15}};

        while (!isDead)
        {
            if (snakeDirection == up || snakeDirection == down)
                usleep(250000);
            else
                usleep(100000);
            ch = wgetch(win);
            switch (ch)
            {
            case KEY_UP:
                if (prevDirection == left || prevDirection == right)
                {
                    snakeDirection = up;
                    prevDirection = snakeDirection;
                }
                break;
            case KEY_RIGHT:
                if (prevDirection == up || prevDirection == down)
                {
                    snakeDirection = right;
                    prevDirection = snakeDirection;
                }
                break;
            case KEY_DOWN:
                if (prevDirection == left || prevDirection == right)
                {
                    snakeDirection = down;
                    prevDirection = snakeDirection;
                }
                break;
            case KEY_LEFT:
                if (prevDirection == up || prevDirection == down)
                {
                    snakeDirection = left;
                    prevDirection = snakeDirection;
                }
                break;
            };
            switch (snakeDirection)
            {
            case up:
                snake.push_front({snake.front().x, snake.front().y - 1});
                break;
            case right:
                snake.push_front({snake.front().x + 1, snake.front().y});
                break;
            case down:
                snake.push_front({snake.front().x, snake.front().y + 1});
                break;
            case left:
                snake.push_front({snake.front().x - 1, snake.front().y});
                break;
            };

            // Food Collisons:
            if (snake.front().x == FoodX && snake.front().y == FoodY)
            {
                Score += 2;
                while (mvwinch(win, FoodY, FoodX) != L' ')
                {
                    FoodX = (rand() % (win_x - 2 * border_size)) + border_size;
                    FoodY = (rand() % (win_y - 2 * border_size)) + border_size;
                }

                for (int i = 0; i < 3; i++)
                    snake.push_back({snake.back().x, snake.back().y});
            }

            // Wall collision
            if (snake.front().x <= 0 || snake.front().x >= ARENA_WIDTH - 1)
                isDead = true;
            if (snake.front().y <= 0 || snake.front().y >= ARENA_HEIGHT - 1)
                isDead = true;

            // self collison

            for (list<snakeSegment>::iterator i = snake.begin(); i != snake.end(); i++)
                if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)
                    isDead = true;

            // Chop off Snakes tail
            snake.pop_back();
            // clear the window
            werase(win);
            // drawing game box
            wattron(win, COLOR_PAIR(2));
            box(win, ACS_VLINE, ACS_HLINE);
            wattroff(win, COLOR_PAIR(2));
            // Update the scoreboard
            mvwprintw(score_win, 3, (score_win_w - 10) / 2, "SCORE: %d", Score);
            wrefresh(score_win);

            // Drawing the Snake Body:

            for (auto s : snake)
            {
                mvwprintw(win, s.y, s.x, "%lc", L'o');
            }
            // Drawing the snake Head:
            mvwprintw(win, snake.front().y, snake.front().x, "%lc", isDead ? L'X' : L'@');

            // Draw the Food
            mvwprintw(win, FoodY, FoodX, "%lc", L'#');
            if (isDead)
            {
                mvwprintw(score_win, 0, 0, gameover);
                mvwprintw(win, ARENA_HEIGHT / 2, (ARENA_WIDTH - 10) / 2, "SCORE: %d", Score);
                mvwprintw(win, (ARENA_HEIGHT / 2) + 4, (ARENA_WIDTH - 30) / 2, "Press SPACEBAR to RESTART...");
                mvwprintw(win, (ARENA_HEIGHT / 2) + 8, (ARENA_WIDTH - 18) / 2, "Press ESC to EXIT.");
                wrefresh(score_win);
                wrefresh(win);
                while (true)
                {
                    // wait for space bar to be pressed
                    int ch = wgetch(win);
                    if (ch == ' ')
                    {
                        restartGame = true;
                        break;
                    }
                    else if (ch == 27)
                    {
                        gameOver = true;
                        break;
                    }
                }
            }
        }
        if (gameOver)
        {
            delwin(win);
            delwin(score_win);
            endwin();
            return 0;
        }
        else if (!restartGame)
            break;
        restartGame = false;
    }
}

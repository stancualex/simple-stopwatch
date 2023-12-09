#include <ncurses.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define SECS_IN_HR 3600
#define MINS_IN_HR 60
#define SECS_IN_MIN 60
#define NSEC_IN_MSEC 1000000
#define NSEC_IN_SEC 1000000000

#define FPS 100

bool paused = true;
bool quit = false;

struct timespec beforenow;
struct timespec now;

struct timespec begin = {0, 0};
struct timespec end;

void format_time(char* out_str, struct timespec time) {
    uint64_t hr = time.tv_sec / SECS_IN_HR;
    uint64_t min = time.tv_sec / SECS_IN_MIN % MINS_IN_HR;
    uint64_t sec = time.tv_sec % SECS_IN_MIN;
    uint64_t msec = time.tv_nsec / NSEC_IN_MSEC;
    sprintf(out_str, "%lu:%02lu:%02lu.%03lu", hr, min, sec, msec);
};

void show_watch(struct timespec time) {
    char fmt_str[23];
    format_time(fmt_str, time);
    attron(A_BOLD);
    mvprintw(1, 1, "%s", fmt_str);
    attroff(A_BOLD);
    refresh();
}

bool has_to_update() {
    return (now.tv_sec == beforenow.tv_sec && (now.tv_nsec - beforenow.tv_nsec) * FPS >= NSEC_IN_SEC) ||
           (now.tv_sec - beforenow.tv_sec == 1 &&
            ((NSEC_IN_SEC - beforenow.tv_nsec) + now.tv_nsec) * FPS >= NSEC_IN_SEC);
}

struct timespec diff_time(struct timespec a, struct timespec b) {
    struct timespec c;
    c.tv_sec = a.tv_sec - b.tv_sec;
    c.tv_nsec = a.tv_nsec - b.tv_nsec;
    if (c.tv_nsec < 0) {
        c.tv_sec--;
        c.tv_nsec += NSEC_IN_SEC;
    }
    return c;
}

void reset_watch() {
    begin.tv_sec = 0;
    begin.tv_nsec = 0;
}

void handle_input(int c) {
    switch (c) {
        case 'p':
            begin = end;
            paused = !paused;
            break;
        case 'q':
            quit = true;
            break;
        case 'r':
            reset_watch();
            paused = true;
        default:
            break;
    }
}

int main() {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    timeout(1);
    show_watch(begin);
    clock_gettime(CLOCK_MONOTONIC, &beforenow);
    while (!quit) {
        int c = getch();
        if (c != ERR)
            handle_input(c);
        clock_gettime(CLOCK_MONOTONIC, &now);
        end = diff_time(now, begin);
        if (has_to_update()) {
            if (paused)
                show_watch(begin);
            else
                show_watch(end);
            clock_gettime(CLOCK_MONOTONIC, &beforenow);
        }
    }
    endwin();
    return 0;
}

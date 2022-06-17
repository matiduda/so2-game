#include "client_func.h"

void update_windows_client(ui interface, char dest[][MAX_WORLD_SIZE], point map_center) {

    WINDOW *game_window = interface.game_window;
    WINDOW *stat_window = interface.stat_window;
    WINDOW *legend = interface.legend;

    // werase(stat_window);
    // werase(game_window);
    // // werase(legend);
    
    int MAP_Y = map_center.y - CLIENT_FOV + 1;
    int MAP_X = map_center.x - CLIENT_FOV + 1;

    for(int i = 0; i < CLIENT_MAP_SIZE; i++) {

        for(int j = 0; j < CLIENT_MAP_SIZE; j++) {

            char c = dest[i][j];

            switch(c) {
                case MAP_WALL:
                    wattron(game_window, COLOR_PAIR(WALL));
                    c = MAP_EMPTY;
                    break;
                case '1' || '2' || '3' || '4':
                    wattron(game_window, COLOR_PAIR(PLAYER));
                    break;
                case MAP_BEAST:
                    wattron(game_window, COLOR_PAIR(ENEMY));
                    break;
                case MAP_CAMPSITE:
                    wattron(game_window, COLOR_PAIR(CAMPSITE));
                    break;
                case MAP_BUSHES:
                    wattron(game_window, COLOR_PAIR(BUSHES));
                    break;
                case MAP_COIN_1:
                    wattron(game_window, COLOR_PAIR(COIN));
                    break;
                case MAP_COIN_10:
                    wattron(game_window, COLOR_PAIR(COIN));
                    break;
                case MAP_COIN_50:
                    wattron(game_window, COLOR_PAIR(COIN));
                    break;
                case MAP_COIN_DROPPED:
                    wattron(game_window, COLOR_PAIR(COIN));
                    break;
                default:
                    wattron(game_window, COLOR_PAIR(DEFAULT));
                    break;
            }

            mvwprintw(game_window, MAP_Y + i, MAP_X + j, "%c", c);
        }
    }

    wattron(game_window, COLOR_PAIR(1));

    box(game_window, 0, 0);

    wrefresh(stat_window);
    wrefresh(legend);
    wrefresh(game_window);
}

void print_info_client(WINDOW *w, info_client *info) {
    if(!w || !info)
        return;

    const int Y = 1;
    const int X = 1;

    mvwprintw(w, Y + 0,  X + 0, "Server's PID: %d", info->response->pid);

    mvwprintw(w, Y + 2,  X + 0, "Campsite X/Y: %02d/%02d", info->campsite_xy.x, info->campsite_xy.y);
    mvwprintw(w, Y + 3,  X + 0, "Round number: %d", info->response->round_number);

    mvwprintw(w, Y + 5,  X + 0, "Player:");
    mvwprintw(w, Y + 6,  X + 0, "Number");
    mvwprintw(w, Y + 7,  X + 0, "Type");
    mvwprintw(w, Y + 8,  X + 0, "Curr X/Y");
    mvwprintw(w, Y + 9,  X + 0, "Deaths");

    mvwprintw(w, Y + 11, X + 0, "Coins");
    mvwprintw(w, Y + 12, X + 0, "carried");
    mvwprintw(w, Y + 13, X + 0, "brought");

    int xoff = 13;

    mvwprintw(w, Y + 6,  X + xoff, "%d", info->response->number);
    if(info->response->type == 0)
        mvwprintw(w, Y + 7,  X + xoff, "HUMAN");
    else if(info->response->type == 1)
        mvwprintw(w, Y + 7,  X + xoff, "CPU");
    mvwprintw(w, Y + 8,  X + xoff, "%02d/%02d", info->response->player_position.x, info->response->player_position.y);
    mvwprintw(w, Y + 9,  X + xoff, "%d", info->response->deaths);

    mvwprintw(w, Y + 12, X + xoff, "%d", info->response->coins_carried);
    mvwprintw(w, Y + 13, X + xoff, "%d", info->response->coins_brought);
       
}
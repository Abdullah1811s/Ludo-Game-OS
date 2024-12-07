#ifndef STRUCTS_H
#define STRUCTS_H

// Struct to represent x and y coordinates
struct xy_coordinate
{
    int x;
    int y;
};

// Struct to represent starting positions with associated colors
struct start_pos
{
    int x;
    int y;
    const char *color;
};
// struct to store the position and the name of toke like ((6,6) , "T1");
struct name_and_pos
{
    int x, y;
    char *name;
};

// struct to store the complete info about token red_token1(((6,6) , "RT1") , red);
struct token
{
    struct name_and_pos *name_and_pos;
    char *color;
};

struct player_data
{
    char *player_name; // player name like red player yellow player
    struct token *all_tokens;
    int dice_value[3];
    int player_id;
    int can_go_home;
};

extern struct start_pos start_places[];
struct xy_coordinate tokens[] = {
    {11, 2}, {11, 3}, {12, 2}, {12, 3}, {2, 2}, {3, 2}, {3, 3}, {2, 3}, {2, 11}, {3, 11}, {3, 12}, {2, 12}, {11, 11}, {12, 11}, {12, 12}, {11, 12}};
#endif
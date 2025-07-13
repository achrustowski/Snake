#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_timer.h"
#include "singly_linked_list.h"

#define FPS                 8
#define FRAME_DELAY         (1000 / FPS)
#define GRID_WIDTH          30
#define GRID_HEIGHT         20
#define CELL_SIZE           24
#define SNAKE_LENGTH        4
#define SCREEN_WIDTH        (GRID_WIDTH * CELL_SIZE)
#define SCREEN_HEIGHT       (GRID_HEIGHT * CELL_SIZE)

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} Snake_Direction;

typedef struct
{
    SDL_Surface* text;
    SDL_Texture* texture;
    char str[20];
} User_Message;

int score = 0;
bool moved = true;
bool is_paused = false;
bool is_running = true;
bool game_over = false;

TTF_Font* font_small = NULL;
TTF_Font* font_large = NULL;
SDL_Color color = {200, 194, 188, 255};
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

static void game_over_render(void);
static User_Message* create_text(TTF_Font* font, SDL_Color color, char* str);
static void do_logic(Linked_List* snake, Linked_List* apples);
static void detect_collision(Linked_List* snake);
static void detect_apple(Linked_List* snake, Linked_List* apples);
static void update_snake(Linked_List* snake);
static void draw_snake(Linked_List* snake);
static void draw_apples(Linked_List* apples);
static void reset_game(Linked_List* snake, Linked_List* apples);
static void render(Linked_List* snake, Linked_List* apples);
static void do_input(Linked_List* snake);
static void prepare_scene(void);
static void present_scene(void);

static User_Message* create_text(TTF_Font* font, SDL_Color color, char* str)
{
    User_Message* display = (User_Message*)malloc(sizeof(User_Message));

    memcpy(display->str, str, sizeof(&str));

    display->text = TTF_RenderText_Solid(font, str, 0, color);
    if (display->text)
    {
        display->texture = SDL_CreateTextureFromSurface(renderer, display->text);
    }

    return display;
}

static void detect_collision(Linked_List* snake)
{
    for (Node* node = snake->head->next; node; node = node->next)
    {
        if (snake->head->x == node->x && snake->head->y == node->y)
        {
            game_over = true;
        }
    }

    if (snake->head->x + CELL_SIZE > SCREEN_WIDTH ||
            snake->head->y + CELL_SIZE > SCREEN_HEIGHT ||
            snake->head->x < 0 ||
            snake->head->y < 0)
    {
        game_over = true;
    }
}

static void detect_apple(Linked_List* snake, Linked_List* apples)
{
    for (Node* apple = apples->head; apple; apple = apple->next)
    {
        if (snake->head->x == apple->x && snake->head->y == apple->y)
        {
            apple->x = (rand() % (SCREEN_WIDTH / CELL_SIZE)) * CELL_SIZE;
            apple->y = (rand() % (SCREEN_HEIGHT / CELL_SIZE)) * CELL_SIZE;
            INSERT_AT_TAIL(snake, NEW_NODE(snake->tail->x, snake->tail->y, snake->head->dir));
            score += 5;
        }
    }
}

static void update_snake(Linked_List* snake)
{
    Node* head = LIST_GET_HEAD(snake);
    Node* current = head;
    Node* previous = NULL;

    while (current->next != NULL)
    {
        previous = current;
        current = current->next;
    }

    switch (snake->head->dir)
    {
        case RIGHT:
            current->x = head->x + CELL_SIZE;
            current->y = head->y;
            break;
        case LEFT:
            current->x = head->x - CELL_SIZE;
            current->y = head->y;
            break;
        case UP:
            current->x = head->x;
            current->y = head->y - CELL_SIZE;
            break;
        case DOWN:
            current->x = head->x;
            current->y = head->y + CELL_SIZE;
            break;
    }

    previous->next = NULL;
    snake->tail = previous;
    previous->dir = snake->head->dir;
    current->dir = snake->head->dir;

    INSERT_AT_HEAD(snake, current);
    free(previous->next);
}

static void do_logic(Linked_List* snake, Linked_List* apples)
{
    update_snake(snake);
    moved = false;
    detect_apple(snake, apples);
    detect_collision(snake);
}

static void draw_apples(Linked_List* apples)
{
    SDL_FRect r;
    r.w = CELL_SIZE;
    r.h = CELL_SIZE;

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    for (Node* node = apples->head; node; node = node->next)
    {
        r.x = node->x;
        r.y = node->y;
        SDL_RenderFillRect(renderer, &r);
    }
}

static void draw_snake(Linked_List* snake)
{
    SDL_FRect r;
    r.w = CELL_SIZE;
    r.h = CELL_SIZE;

    for (Node* node = snake->head; node; node = node->next)
    {
        r.x = node->x;
        r.y = node->y;

        if (node == snake->head)
        {
            SDL_SetRenderDrawColor(renderer, 252, 253, 0, 255);
        } else
        {
            SDL_SetRenderDrawColor(renderer, 10, 105, 6, 255);
        }
        SDL_RenderFillRect(renderer, &r);
    }
}

static void reset_game(Linked_List* snake, Linked_List* apples)
{
    CLEAR_LIST(snake);
    CLEAR_LIST(apples);
    game_over = false;
    score = 0;

    int i;
    int snake_pos_x = SCREEN_WIDTH / 2 - CELL_SIZE;
    int snake_pos_y = SCREEN_HEIGHT / 2 - CELL_SIZE;

    for (i = 0; i < SNAKE_LENGTH; ++i)
    {
        INSERT_AT_TAIL(snake, NEW_NODE(snake_pos_x, snake_pos_y, 3));
        snake_pos_x -= CELL_SIZE;
        INSERT_AT_TAIL(apples, NEW_NODE((rand() % (SCREEN_WIDTH / CELL_SIZE)) * CELL_SIZE,
                    (rand() % (SCREEN_HEIGHT / CELL_SIZE)) * CELL_SIZE, 0));
    }
}

static void render(Linked_List* snake, Linked_List* apples)
{
    draw_snake(snake);
    draw_apples(apples);
}

static void do_input(Linked_List* snake)
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                is_running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (event.key.key)
                {
                    case SDLK_UP:
                        if (snake->head->dir != DOWN && !moved)
                        {
                            snake->head->dir = UP;
                            moved = true;
                        }
                        break;
                    case SDLK_DOWN:
                        if (snake->head->dir != UP && !moved)
                        {
                            snake->head->dir = DOWN;
                            moved = true;
                        }
                        break;
                    case SDLK_LEFT:
                        if (snake->head->dir != RIGHT && !moved)
                        {
                            snake->head->dir = LEFT;
                            moved = true;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (snake->head->dir != LEFT && !moved)
                        {
                            snake->head->dir = RIGHT;
                            moved = true;
                        }
                        break;
                    case SDLK_P:
                        is_paused = true;
                        break;
                }
        }
    }
}

static void game_over_render(void)
{
    char str[10];
    User_Message* game_over_message = create_text(font_large, color, "GAME OVER");
    User_Message* final_score = create_text(font_small, color, "FINAL SCORE: ");
    User_Message* show_score = create_text(font_small, color, SDL_itoa(score, str, 10));
    SDL_FRect m;
    m.w = game_over_message->texture->w;
    m.h = game_over_message->texture->h;
    m.x = (SCREEN_WIDTH / 2.0f) - (m.w / 2);
    m.y = (SCREEN_HEIGHT / 2.0f) - (2 * CELL_SIZE);
    SDL_FRect m1;
    m1.w = final_score->texture->w;
    m1.h = final_score->texture->h;
    m1.x = (SCREEN_WIDTH / 2.0f) - (m1.w / 2) - (CELL_SIZE / 2.0f);
    m1.y = (SCREEN_HEIGHT / 2.0f) + CELL_SIZE;
    SDL_FRect m2;
    m2.w = show_score->texture->w;
    m2.h = show_score->texture->h;
    m2.x = (SCREEN_WIDTH / 2.0f) + (m1.w / 2) - (CELL_SIZE / 2.0f);
    m2.y = (SCREEN_HEIGHT / 2.0f) + CELL_SIZE;

    while(game_over)
    {
        prepare_scene();

        SDL_Event event;

        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_EVENT_QUIT:
                    game_over = false;
                    is_running = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    switch(event.key.key)
                    {
                        case SDLK_RETURN:
                            game_over = false;
                            break;
                    }
            }
        }
        SDL_DestroySurface(game_over_message->text);
        SDL_DestroySurface(final_score->text);
        SDL_DestroySurface(show_score->text);
        SDL_RenderTexture(renderer, game_over_message->texture, NULL, &m);
        SDL_RenderTexture(renderer, final_score->texture, NULL, &m1);
        SDL_RenderTexture(renderer, show_score->texture, NULL, &m2);

        present_scene();
    }
    SDL_DestroyTexture(game_over_message->texture);
    SDL_DestroyTexture(final_score->texture);
    SDL_DestroyTexture(show_score->texture);
    free(game_over_message);
    free(final_score);
    free(show_score);
}

static void present_scene(void)
{
    SDL_RenderPresent(renderer);
}

static void prepare_scene(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Snake", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, NULL);

    TTF_Init();

    font_small = TTF_OpenFont("/Users/work/Library/Fonts/Terminus (TTF) Bold 700.ttf", 24.0f);
    font_large = TTF_OpenFont("/Users/work/Library/Fonts/Terminus (TTF) Bold 700.ttf", 48.0f);

    srand(time(NULL));

    Linked_List* snake = NEW_LINKED_LIST();
    Linked_List* apples = NEW_LINKED_LIST();

    reset_game(snake, apples);
    Uint32 frame_start;
    int frame_time;

    while (is_running)
    {
        frame_start = SDL_GetTicks();
        prepare_scene();
        do_input(snake);

        do_logic(snake, apples);

        if (game_over)
        {
            game_over_render();
            reset_game(snake, apples);
        } else
        {
            render(snake, apples);
        }

        present_scene();

        frame_time = SDL_GetTicks() - frame_start;

        if (frame_time < FRAME_DELAY)
        {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }
    DELETE_LIST(snake);
    DELETE_LIST(apples);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#include <SDL.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

typedef struct{
    float x;
    float y;
    float r;
    float m;
    int id;
    float vx;
    float vy;
	int c1;
	int c2;
	int c3;
}Object;

typedef struct{
    int width;
    int height;
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool quit;
}application;

void draw_circle(SDL_Renderer *renderer, int x, int y, int radius);
void update(float dt, Object* objects);
void check_collision(Object* objects);
void PollEvent(application* app);
void Close(application* app);
void handle_collision(Object* ball1,Object* ball2);
void draw_to_renderer(application* app,Object* objects);
void init_objects(Object* objects);
int init_placement(Object* objects,int s);
const float G = 9.8;
const int FPS = 100;
const int amount = 15;
const int width = 700;
const int height = 700;
int main(int argc, char* args[]) {
	srand(time(NULL));

	Object objects[amount];
	init_objects(objects);

	
	

    SDL_Init(SDL_INIT_VIDEO);


    SDL_Window* window = SDL_CreateWindow("Physics", SDL_WINDOWPOS_CENTERED_DISPLAY(1), SDL_WINDOWPOS_CENTERED_DISPLAY(1), width, height, SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    application app = {width, height, window, renderer, false};

    const float dt = 1000.0 / FPS;
    int starting_tick = 0;

    while(!app.quit) {
        starting_tick = SDL_GetTicks();

        PollEvent(&app);

        check_collision(objects);

        update(dt, objects);
        
        

        
		draw_to_renderer(&app, objects);


        if ((dt) > SDL_GetTicks() - starting_tick){
            SDL_Delay(dt - (SDL_GetTicks() - starting_tick));
        }
    }
	free(objects);
    Close(&app);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
void draw_circle(SDL_Renderer* renderer, int x, int y, int radius) {
    for (int w = -radius; w <= radius; w++) {
        for (int h = -radius; h <= radius; h++) {
            int dx = w * w;
            int dy = h * h;
            if ((dx + dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + w, y + h);
            }
        }
    }
}
void update(float dt,Object* objects){
	for (int i = 0; i < amount; i++){
		Object* b = &objects[i];
		b->vy += G * dt / 200;
		b->y += b->vy;
		b->x += b->vx / 2;
		if(b->y + b->r >= height && b->vy > 0){
			b->vy *= -1;
			b->y--;
		}
		if(b->x + b->r >= width && b->vx > 0 ){
			b->vx *= -1;
		}
		if(b->x - b->r < 0){
			b->vx *= -1;
		}	
	}
}

void check_collision(Object* objects){
	for (int i = 0; i < amount; i++){
		for (int j = i+1; j < amount; j++){
			handle_collision(&objects[i],&objects[j]);
		}
	}
}
void PollEvent(application* app){
	SDL_Event ev;
		while (SDL_PollEvent(&ev) != 0){
			if (ev.type == SDL_QUIT){
				app->quit = true;
				break;
			}
		}
}
void Close(application* app){
	SDL_DestroyWindow(app->window);
	SDL_DestroyRenderer(app->renderer);
	SDL_Quit();
}
void handle_collision(Object* ball1,Object* ball2){
	float dx = ball2->x - ball1->x;
    float dy = ball2->y - ball1->y;
    float dist = sqrt(dx*dx + dy*dy);
    if(dist <= ball1->r + ball2->r){
        float nx = dx / dist;
        float ny = dy / dist;
        float tx = -ny; 
        float ty = nx;

        float v1n = ball1->vx * nx + ball1->vy * ny;
        float v1t = ball1->vx * tx + ball1->vy * ty;
        float v2n = ball2->vx * nx + ball2->vy * ny;
        float v2t = ball2->vx * tx + ball2->vy * ty;

        float v1n_new = ((ball1->m - ball2->m) * v1n + 2 * ball2->m * v2n) / (ball1->m + ball2->m);
        float v2n_new = ((ball2->m - ball1->m) * v2n + 2 * ball1->m * v1n) / (ball1->m + ball2->m);

        ball1->vx = v1n_new * nx + v1t * tx;
        ball1->vy = v1n_new * ny + v1t * ty;
        ball2->vx = v2n_new * nx + v2t * tx;
        ball2->vy = v2n_new * ny + v2t * ty;
    }
}
void draw_to_renderer(application* app,Object* objects){
	SDL_SetRenderDrawColor(app->renderer, 0, 0, 0, 255);
    SDL_RenderClear(app->renderer);
	for (int i = 0; i < amount; i++){
		SDL_SetRenderDrawColor(app->renderer, objects[i].c1, objects[i].c2, objects[i].c3, 255);
        draw_circle(app->renderer, objects[i].x, objects[i].y, objects[i].r);
	}
	SDL_RenderPresent(app->renderer);

	
}
void init_objects(Object* objects){
	for (int i = 0; i < amount; i++){
		objects[i].r = rand()%30 + 15;
		objects[i].x = init_placement(objects,i);
		objects[i].y = rand()%200+50;
		objects[i].m = objects[i].r/2;
		objects[i].vx = rand()%40-20;
		objects[i].vy = 0;
		objects[i].c1 = rand()%200+55;
		objects[i].c2 = rand()%200+55;
		objects[i].c3 = rand()%200+55;
	}
}
int init_placement(Object* objects,int s){
	int x = rand()%(int)(width-(objects[s].r*2)) + objects[s].r;
	for (int i = s; i > 0; i--){
		float dx = objects[i-1].x - x;
    	float dy = objects[i-1].y - objects[i].y;
    	float dist = sqrt(dx*dx + dy*dy);
    	if(dist <= objects[i].r + objects[i].r){
			return init_placement(objects,s);
		}
	}
	return x;
}
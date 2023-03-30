#include <SDL.h>
#include <random>
#include <time.h>


class Object{
	public:
		float x;
		float y;
		float r;
		float vx;
		float vy;
		float m;
		int id;
		float px;
		float py;
		static int count;
	Object(float x, float y, float r, float m){
	count++;
    this->x = x;
    this->y = y;
	this->vx = rand()%100/5;
	this->vy = 0;
	this->r = r;
	this->m = m;
	this->id = count;
    }
};
class application{
	public:
		int width;
		int height;
		SDL_Window* window;
		SDL_Renderer* renderer;
		bool quit;
		
		application(int w, int h, SDL_Window* win,SDL_Renderer* ren){
			width = w;
			height = h;
			window = win;
			renderer = ren;
			quit = false;
		}
};

void draw_circle(SDL_Renderer *renderer, int x, int y, int radius);
void update(float dt, Object* b);
void check_collision(Object *ball1, Object *ball2);
void PollEvent(application* app);
void Close(application* app);
void handle_collision(Object* ball1,Object* ball2);

constexpr float G = 9.8;
constexpr int FPS = 60;
int Object::count = 0;
int main(int argc, char* args[]){
	SDL_Init(SDL_INIT_VIDEO);
	srand(time(NULL));
	int color1 = rand()%150 + 105;
	int color2 = rand()%150 + 105;
	int color3 = rand()%150 + 105;

	SDL_Window* window = SDL_CreateWindow("Physics",SDL_WINDOWPOS_CENTERED_DISPLAY(1), SDL_WINDOWPOS_CENTERED_DISPLAY(1), 500, 500, SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Renderer* renderer = SDL_CreateRenderer(window,-1,0);

	application app(500,500, window, renderer);


	const float dt = 1000 / FPS;
	int starting_tick = 0;
	Object b1(100,100,20,10);
	Object b2(400,100,80,40);
	SDL_Log("%d",b2.id);
	while(!app.quit){
		starting_tick = SDL_GetTicks();
		
		PollEvent(&app);
		
		check_collision(&b1,&b2);

		
		update(dt,&b1);
		update(dt,&b2);

		SDL_SetRenderDrawColor(app.renderer,0,0,0,255);
		SDL_RenderClear(app.renderer);
		SDL_SetRenderDrawColor(app.renderer,color1,color2,color3,255);
		draw_circle(app.renderer,b1.x,b1.y,b1.r);
		SDL_SetRenderDrawColor(app.renderer,color2,color3,color1,255);
		draw_circle(app.renderer,b2.x,b2.y,b2.r);

		SDL_RenderPresent(app.renderer);
		if ((dt) > SDL_GetTicks64() - starting_tick){
	        SDL_Delay(dt - (SDL_GetTicks() - starting_tick));
	    }
	}

	Close(&app);
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
void update(float dt,Object* b){
		
		b->vy += G * dt / 100;
		b->y += b->vy;
		b->x += b->vx / 2;
		
		if(b->y + b->r >= 500 && b->vy > 0){
			b->vy *= -1;
			b->y--;
		}
		if(b->x + b->r >= 500 && b->vx > 0 ){
			b->vx *= -1;
		}
		if(b->x - b->r < 0 && b->px > 0){
			b->vx *= -1;
		}
		b->px = b->x;
}
void check_collision(Object *ball1, Object *ball2){
	handle_collision(ball1,ball2);
    /*if(ball1->id != ball2->id){
        if(((ball1->x - ball2->x) * (ball1->x - ball2->x) + (ball1->y - ball2->y) * (ball1->y - ball2->y)) <= ((ball1->r + ball2->r) * (ball1->r + ball2->r))){
        }
    }*/
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
    if(dist <= ball1->r + ball2->r){ // check if the circles are colliding
        float nx = dx / dist; // calculate the normal vector
        float ny = dy / dist;
        float tx = -ny; // calculate the tangential vector
        float ty = nx;

        // resolve the velocities along the normal and tangential directions
        float v1n = ball1->vx * nx + ball1->vy * ny;
        float v1t = ball1->vx * tx + ball1->vy * ty;
        float v2n = ball2->vx * nx + ball2->vy * ny;
        float v2t = ball2->vx * tx + ball2->vy * ty;

        // calculate the new velocities after the collision
        float v1n_new = ((ball1->m - ball2->m) * v1n + 2 * ball2->m * v2n) / (ball1->m + ball2->m);
        float v2n_new = ((ball2->m - ball1->m) * v2n + 2 * ball1->m * v1n) / (ball1->m + ball2->m);

        // update the velocities of the balls
        ball1->vx = v1n_new * nx + v1t * tx;
        ball1->vy = v1n_new * ny + v1t * ty;
        ball2->vx = v2n_new * nx + v2t * tx;
        ball2->vy = v2n_new * ny + v2t * ty;
    }
}

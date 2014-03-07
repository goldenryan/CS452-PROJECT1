#include "owninitshader2.h"
#include <cstdlib>
using namespace std;

// TEST WITH g++ testlab3.cpp -lSDL2 -lglut -lGL -lGLEW -g ; ./a.out

int MAX_MOVE_X = 0;
int MIN_MOVE_X = -52;
int MAX_MOVE_Y = 0;
int MIN_MOVE_Y = -26;

int BALL_WIDTH = 2;
int PADDLE_WIDTH = 2;
int PADDLE_HEIGHT = 8;

float BALL_SPEED_FACTOR = 1.001;

void rotate(GLuint locate);

GLuint program;

GLfloat pit, yaw;
GLfloat scale = 1;

glm::vec3 transCamera;

glm::vec3 transP1;
glm::vec3 transP2;
glm::vec3 transBall;

float ballVelX = 0;
float ballVelY = 0;

GLuint playerVBO[2];
GLuint vao[2];

GLfloat width;

GLfloat verts_player[] = {
	2,8,0,
	0,8,0,
	0,0,0,
	2,0,0,

	// BALL
	2,1,0,
	0,1,0,
	0,-1,0,
	2,-1,0
};

GLfloat colors[]={
	1.0f,1.0f,0.0f,1.0f,
	1.0f,1.0f,0.0f,1.0f,
	1.0f,1.0f,1.0f,1.0f,
	1.0f,1.0f,1.0f,1.0f,

	// BALL
	1.0f,0.5f,1.0f,1.0f,
	1.0f,1.0f,1.0f,1.0f,
	1.0f,0.5f,1.0f,1.0f,
	1.0f,1.0f,1.0f,1.0f
};

void reset();

void init(){
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 600, 600);

	glGenVertexArrays(2, vao);
	glBindVertexArray(vao[0]);

	glGenBuffers(2, playerVBO);

	// PLAYER VERTS //
	glBindBuffer(GL_ARRAY_BUFFER, playerVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts_player), verts_player, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, playerVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// BALL VERTS //
	//glBindVertexArray(vao[1]);

	ShaderInfo shaders[]={
		{ GL_VERTEX_SHADER , "vertexshader.glsl"},
		{ GL_FRAGMENT_SHADER , "fragmentshader.glsl"},
		{ GL_NONE , NULL}
	};

	program = initShaders(shaders);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	//glEnableVertexAttribArray(2);

	reset();	
}

void reset(){
	// SETUP POSITIONS //
	transCamera.x = 25;

	transP1.x = MIN_MOVE_X;

	transP2.y = MIN_MOVE_Y;

	transBall.x = MIN_MOVE_X / 2;
	ballVelX = -0.1;
	ballVelY = -0.1;
}

/** Render player at translation. */
void renderGameObj(glm::vec3 & transGameObj, int vertOffset){
	glm::mat4 trans;
	trans=glm::translate(trans, transCamera);
	trans=glm::translate(trans, transGameObj);
	trans=glm::scale(trans, glm::vec3(scale));

	GLint tempLoc = glGetUniformLocation(program, "modelMatrix");//Matrix that handle the transformations

	glUniformMatrix4fv(tempLoc, 1, GL_FALSE, &trans[0][0]);

	glDrawArrays(GL_POLYGON, vertOffset, 4);
}

void display(SDL_Window* screen){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, *playerVBO);
	renderGameObj(transP1, 0);
	renderGameObj(transP2, 0);
	renderGameObj(transBall, 4);

	glFlush();
	SDL_GL_SwapWindow(screen);
}

void input(SDL_Window* screen){
	// UPDATE BALL
	transBall.x += ballVelX;
	transBall.y += ballVelY;

	ballVelX *= BALL_SPEED_FACTOR;
	ballVelY *= BALL_SPEED_FACTOR;

	// Check top and bottom collision
	if(transBall.y > MAX_MOVE_Y + PADDLE_HEIGHT || transBall.y < MIN_MOVE_Y){
		ballVelY *= -1;
	}
	// Check right collision
	if(ballVelX > 0){
		float barrier = MAX_MOVE_X - BALL_WIDTH;
		if(transBall.x > barrier + 1){
			reset();
		} else if(transBall.x > barrier){
			if(transBall.y >= transP2.y && transBall.y <= transP2.y + PADDLE_HEIGHT){
				ballVelX *= -1;
			}
		}
	}
	// Check left collision
	if(ballVelX < 0){
		float barrier = MIN_MOVE_X + PADDLE_WIDTH;
		if(transBall.x < barrier - 1){
			reset();
		} else if(transBall.x < barrier){
			if(transBall.y >= transP1.y && transBall.y <= transP1.y + PADDLE_HEIGHT){
				ballVelX *= -1;
			}
		}
	}

	// cout << "Ball: " << transBall.x << '\n';

	// GRAB INPUTS
	SDL_Event event;
	while (SDL_PollEvent(&event)){//keyboard events
	 switch (event.type){
			case SDL_QUIT:exit(0);break;
			case SDL_KEYDOWN:
			switch(event.key.keysym.sym){
				// UNIVERSAL //
				case SDLK_ESCAPE:exit(0);
				// PLAYER 1 //
				case SDLK_w:
					if(transP1.y < MAX_MOVE_Y){
					transP1.y += 1;
					}
					// cout << "P1: " << transP1.y << "\n";
					break;
				case SDLK_s:
					if(transP1.y > MIN_MOVE_Y){
						transP1.y -= 1;
					}
					// cout << "P1: " << transP1.y << "\n";
					break;
				// PLAYER 2 //
				case SDLK_i:
					if(transP2.y < MAX_MOVE_Y){
						transP2.y += 1;
					}
					// cout << "P2: " << transP1.y << "\n";
					break;
				case SDLK_k:
					if(transP2.y > MIN_MOVE_Y){
						transP2.y -= 1;
					}
					// cout << "P2: " << transP1.y << "\n";
					break;
				//case SDLK_a:translate.x-=2;break;
				//case SDLK_d:translate.x+=2;break;
				//case SDLK_e:scale *= 0.9f;break;
				//case SDLK_q:scale *= 1.1f;break;
				//case SDLK_i:pit+=2;break;
				//case SDLK_k:pit-=2;break;
				//case SDLK_j:yaw+=2;break;
				//case SDLK_l:yaw-=2;break;
			}
		}
	}
}


int main(int argc, char **argv){

		//SDL window
		SDL_Window *window;
		if(SDL_Init(SDL_INIT_VIDEO)<0){//initilizes the SDL video subsystem
				fprintf(stderr,"Unable to create window: %s\n", SDL_GetError());
				SDL_Quit();
				exit(1);//error checking
		}
		//make a windo using SDL
		window = SDL_CreateWindow(
				"Sunset House", //Window title
				0,//SDL_WINDOWPOS_UNDEFINED, //x position
				0,//SDL_WINDOWPOS_UNDEFINED, //y position
				600,	//width, in pixels
				400,	//height, in pixels
				SDL_WINDOW_OPENGL	//# of flags
		);
		//error checking
		if(window==NULL){
				fprintf(stderr,"Unable to create window: %s\n",SDL_GetError());
		}
		//creates opengl context associated with the window
		SDL_GLContext glcontext=SDL_GL_CreateContext(window);
		
		glewExperimental=GL_TRUE;
		if(glewInit()){
				fprintf(stderr, "Unable to initalize GLEW");
				exit(EXIT_FAILURE);
		}
		init();
		while(true){
				input(window);//keyboard controls
				display(window);
		}
		SDL_GL_DeleteContext(glcontext);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 0;
}

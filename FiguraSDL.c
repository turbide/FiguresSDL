/*Este programa es software libre: usted puede redistribuirlo y / o modificarlo
    bajo los términos de la Licencia Pública General GNU publicada por
    la Fundación para el Software Libre, ya sea la versión 3 de la Licencia, o
    (A su elección) cualquier versión posterior.

    Este programa se distribuye con la esperanza de que sea útil,
    pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de
    COMERCIALIZACIÓN o IDONEIDAD PARA UN PROPÓSITO PARTICULAR. Consulte la
    GNU General Public License para más detalles.

    Debería haber recibido una copia de la Licencia Pública General de GNU
    junto con este programa. Si no es así, consulte <http://www.gnu.org/licenses/gpl.html>.

    Autores: Diego Alejandro Cruz Ramirez, Ana Karen Guadalupe, Luis Daniel Iturbide Ramirez.
    
    Orden y practica patrocinada por el Prof. Eduardo Campos Serrano.
*/

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <stdio.h>

#define MAX_WIDTH_IMG_SIZE 128 //Tamaño en ancho de la imagen
#define MAX_HIGH_IMG_SIZE 128 //Tamaño en alto de la imagen
#define MOVING_SIZE 64 //Pixeles que se mueve la figura en focus de la imagen

#define WIDTH_SCREEN_SIZE 800 //Ancho de la pantalla completa (Surface)
#define HIGH_SCREEN_SIZE 600 //Alto de la pantalla completa

#define FPS (1000/24) //Frames por segundo

enum { //Enumeracion que guardalos nombres de cada figura
	TRIANGLE,
	CIRCLE,
	SQUARE,
		NUM_FIGURES
};

enum { //Enumeracion que guarda los estados de cada figura
	ENABLE,
	UNABLE,
		FIGURE_FOCUS_STATE
};

char *images_figures[NUM_FIGURES] = { //Coleccion de la ruta de las imagenes en PNG
	"images/triangle.png",
	"images/circle.png",
	"images/square.png"
};

char *images_figures_focus[NUM_FIGURES] = { //Coleccion de la ruta de las imagenes con focus en PNG
	"images/triangleFocus.png",
	"images/circleFocus.png",
	"images/squareFocus.png"
};

typedef struct  _Figure //Struct con los atributos de una figura
{
	struct _Figure *next;
	struct _Figure *prev;
	int x;
	int y;
	int pivotX;
	int pivotY;
	int w;
	int h;
	int type;
	int focus;
	int mirror;
	int mirror_corner;
	int mirrorX;
	int mirrorY;

} Figure;

// Declaraciones globales de SDL (Surface, imagen, destino de la imagen, pantalla)
 SDL_Surface *image, *screen;
 SDL_Rect dest;
 SDL_Event event;

 //Apuntadores a la lista de figuras
 Figure *first_figure = NULL;
 Figure *last_figure = NULL;

/*
*Funcion que inicializa el video y la pantalla en SDL
*/
int init_SDL()
{
	// init the SDL
 	if (SDL_Init(SDL_INIT_VIDEO) < 0) 
 	{
  		printf("No se pudo iniciar SDL: %s \n",SDL_GetError());
  		exit(1);
	}
 	// Video Mode
 	screen = SDL_SetVideoMode(800,600,24,SDL_HWSURFACE);
 	if (screen == NULL) 
 	{
  		printf("No se puede inicializar el modo gráfico: %s \n",SDL_GetError());
  		exit(1);
 	}
	return 0;
}

/*
*Funcion dibuja la figura que recibe como parametro y dependiendo de la posicion en donde se encuentra
*Ya sea en las esquinas o a los lados, las dibuja n veces
*/
int set_SDL_BlitSurface(Figure *SelectFigure)
{
	dest.x = SelectFigure-> x;
	dest.y = SelectFigure-> y;
	dest.w = SelectFigure-> w;
	dest.h = SelectFigure-> h;

    SDL_BlitSurface(image, NULL, screen, &dest);

    if(SelectFigure->mirror == ENABLE)
	{
		dest.x = SelectFigure-> mirrorX;
		dest.y = SelectFigure-> mirrorY;
		dest.w = SelectFigure-> w;
		dest.h = SelectFigure-> h;

		SDL_BlitSurface(image, NULL, screen, &dest);
	}
	// Valida el espejo de las figuras en cada esquina
	if(SelectFigure->mirror_corner == ENABLE)
	{
		dest.x = 0 - MOVING_SIZE;
		dest.y = 0 - MOVING_SIZE;
		dest.w = SelectFigure-> w;
		dest.h = SelectFigure-> h;

		SDL_BlitSurface(image, NULL, screen, &dest);

		dest.x = WIDTH_SCREEN_SIZE - MOVING_SIZE;
		dest.y = 0 - MOVING_SIZE;
		dest.w = SelectFigure-> w;
		dest.h = SelectFigure-> h;

		SDL_BlitSurface(image, NULL, screen, &dest);

	    dest.x = WIDTH_SCREEN_SIZE - MOVING_SIZE;
		dest.y = HIGH_SCREEN_SIZE - MOVING_SIZE;
		dest.w = SelectFigure-> w;
		dest.h = SelectFigure-> h;

		SDL_BlitSurface(image, NULL, screen, &dest);

		dest.x = 0 - MOVING_SIZE;
		dest.y = HIGH_SCREEN_SIZE - MOVING_SIZE;
		dest.w = SelectFigure-> w;
		dest.h = SelectFigure-> h;

		SDL_BlitSurface(image, NULL, screen, &dest);
	}
}

/*
*Inicializa cada figura dandole valores a sus atributos
*/
int initialize_figures()
{
	int e;
	Figure *newFigure;

	for (e = 0; e < NUM_FIGURES; e++)
	{
		newFigure = (Figure *) malloc (sizeof (Figure));

		//Inicializa la figura
		switch(e)
		{
			case TRIANGLE:
				newFigure->x = 320;
				newFigure->y = 256;
				newFigure->mirrorX = newFigure->x;
				newFigure->mirrorY = newFigure->y;
				newFigure->pivotX = newFigure->x;
				newFigure->pivotY = newFigure->y;
				newFigure->type = TRIANGLE;
				newFigure->focus = ENABLE;
				newFigure->mirror = UNABLE;
				newFigure->mirror_corner = UNABLE;
			break;

			case CIRCLE:
				newFigure->x = 448;
				newFigure->y = 256;
				newFigure->mirrorX = newFigure->x;
				newFigure->mirrorY = newFigure->y;
				newFigure->pivotX = newFigure->x;
				newFigure->pivotY = newFigure->y;
				newFigure->type = CIRCLE;
				newFigure->focus = UNABLE;
				newFigure->mirror = UNABLE;
				newFigure->mirror_corner = UNABLE;
			break;

			case SQUARE:
				newFigure->x = 384;
				newFigure->y = 384;
				newFigure->mirrorX = newFigure->x;
				newFigure->mirrorY = newFigure->y;
				newFigure->pivotX = newFigure->x;
				newFigure->pivotY = newFigure->y;
				newFigure->type = SQUARE;
				newFigure->focus = UNABLE;
				newFigure->mirror = UNABLE;
				newFigure->mirror_corner = UNABLE;
			break;
		}
		// Le da a la figura el ancho y altura maxima
		newFigure->w = MAX_WIDTH_IMG_SIZE;
		newFigure->h = MAX_HIGH_IMG_SIZE;

		//Agrega figura a la lista
		newFigure-> next = NULL;
		newFigure-> prev = last_figure;

		if(last_figure == NULL)
		{
			first_figure = last_figure = newFigure;
		} 
		else 
		{
			last_figure->next = newFigure;
			last_figure = newFigure;
		}

	}
	return 0;
}

void move_focus()
{
	int e;
	Figure *tempFigure = first_figure;

	for (e = 0; e < NUM_FIGURES; e++)
	{
		if(tempFigure->focus == ENABLE)
		{
			tempFigure->focus = UNABLE;

			// Checa la primera y la ultima figura en la lista
			if(tempFigure->next == NULL)
			{
				tempFigure = first_figure;
				tempFigure->focus = ENABLE;
				return;
			}
			else
			{
				//mueve a la siguiente figura
				tempFigure = tempFigure->next;
				tempFigure->focus = ENABLE;
				return;
			}
			if(tempFigure->prev == NULL)
			{
				tempFigure = last_figure;
				tempFigure->focus = ENABLE;
				return;
			}
		}
		tempFigure=tempFigure->next;
	}
}

/*
*Checa si la figura esta en modo FOCUS y la dibuja hasta al final para que este arriba de las demas
*y manda a llamar a la funcion set_SDL_BlitSurface(Figure) para dibujar cada figura en un for()
*/
void draw_figures()
{
	SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format,0,0,0));
 	int e, focus;
	Figure *SelectFigure = NULL;
	Figure *last = NULL;
	if(SelectFigure == NULL)
	{
		SelectFigure = first_figure;
	}

	for(e = 0; e < NUM_FIGURES; e++)
	{
		if(SelectFigure-> focus == ENABLE)
		{
			image = IMG_Load(images_figures_focus[e]);
			focus=e;
			last = SelectFigure;
		}
		else
		{
			image = IMG_Load(images_figures[e]);
			set_SDL_BlitSurface(SelectFigure);
		}
		SelectFigure = SelectFigure-> next;
	}
	image = IMG_Load(images_figures_focus[focus]);
	set_SDL_BlitSurface(last);
	SDL_Flip(screen);
}

/*
*Suma valores a los atributos de x y y de la figura seleccionada para que asi se mueve en la pantalla
*/
void move_figure(int direction)
{
	int e;
	Figure *tempFigure=first_figure;

	for (e = 0; e < NUM_FIGURES; e++)
	{
		if(tempFigure->focus == ENABLE)
		{
			switch(direction)
			{
				case SDLK_UP:
					tempFigure->y-=MOVING_SIZE;
					break;

				case SDLK_DOWN:
					tempFigure->y+=MOVING_SIZE;
					break;

				case SDLK_LEFT:
					tempFigure->x-=MOVING_SIZE;
					break;

				case SDLK_RIGHT:
					tempFigure->x+=MOVING_SIZE;
					break;
			}

			// update virtual pivot position
			tempFigure->pivotX = tempFigure->x + 64;
			tempFigure->pivotY = tempFigure->y + 64;
		}
		tempFigure=tempFigure->next;
	}
}

/*
*Evalua si la figura esta en una posicion posible para que se realize el espejo sobre la figura seleccionada en las esquinas
*y asigna valores para poder realizar eso
*/
void eval_mirror_image()
{
	int e;
	int flagIf=UNABLE;
	int flagIfCorner=UNABLE;
	Figure *tempFigure=first_figure;

	for (e = 0; e < NUM_FIGURES; e++)
	{
		tempFigure->mirrorY = tempFigure->y;
		tempFigure->mirrorX = tempFigure->x;

		if((tempFigure->pivotX == 0 && tempFigure->pivotY == 0) || (tempFigure->pivotX == WIDTH_SCREEN_SIZE && tempFigure->pivotY == 0 ) ||
					(tempFigure->pivotX == WIDTH_SCREEN_SIZE && tempFigure->pivotY == HIGH_SCREEN_SIZE)
					|| (tempFigure->pivotX == 0 && tempFigure->pivotY == HIGH_SCREEN_SIZE))
		{
			flagIfCorner = ENABLE;
			tempFigure->mirror_corner = ENABLE;
		}
		else
		{
			flagIfCorner = UNABLE;
			tempFigure->mirror_corner = UNABLE;
		}

		if(tempFigure->pivotX == 0)
		{
			tempFigure->mirror = ENABLE;
			tempFigure->mirrorX = WIDTH_SCREEN_SIZE - MOVING_SIZE;
			flagIf = ENABLE;
		}
		if(tempFigure->pivotX == WIDTH_SCREEN_SIZE)
		{
			tempFigure->mirror = ENABLE;
			tempFigure->mirrorX = 	0 - MOVING_SIZE;

			flagIf = ENABLE;
		}
		if(tempFigure->pivotY == 0)
		{
			tempFigure->mirror = ENABLE;
			tempFigure->mirrorY = HIGH_SCREEN_SIZE - MOVING_SIZE;
			flagIf = ENABLE;
		}
		if(tempFigure->pivotY == HIGH_SCREEN_SIZE)
		{
			tempFigure->mirror = ENABLE;
			tempFigure->mirrorY = 	0 - MOVING_SIZE;

			flagIf = ENABLE;
		}
		if(flagIf != ENABLE)
		{
			tempFigure->mirror = UNABLE;
		}
		//check for flagcorner
		if(flagIfCorner != ENABLE)
		{
			tempFigure->mirror_corner = UNABLE;
		}
		tempFigure = tempFigure->next;
	}
}

/*
*Evalua posicion de la figura para hacer mirror en x y y y asigna valores para poder reaizar eso
*/
void eval_position()
{
	int e;
	Figure *tempFigure=first_figure;

	for (e = 0; e < NUM_FIGURES; e++)
	{
		//check for X edge position
		// check to rigth edge of screen
		if(tempFigure->pivotX > WIDTH_SCREEN_SIZE)
		{
			tempFigure->x = 0;
		}
		// check to left edge of screen
		if(tempFigure->pivotX < 0 )
		{
			tempFigure->x = WIDTH_SCREEN_SIZE - MOVING_SIZE * 2;
		}
		//check for y edge position
		// check to upper edge of screen
		if(tempFigure->pivotY > HIGH_SCREEN_SIZE)
		{
			tempFigure->y = 0;
		}
		// check to bottom edge of screen
		if(tempFigure->pivotY < 0 )
		{
			tempFigure->y = HIGH_SCREEN_SIZE  - MOVING_SIZE * 2;
		}
		tempFigure=tempFigure->next;
	}
}

/*
* Funcion principal donde se inicializan las figuras, la pantalla, y donde se encuentra el loop principal que es el corazon del programa
*tambien se evalua las teclas presionadas y dependiendo de la tecla se mueve la figura en el eje x o y
*/
int main(int argc, char *argv[])
{
	int done = 0;
	init_SDL();
	initialize_figures(); 
	//display_list();
 	atexit(SDL_Quit);
 	//wait for a hotkey to Exit

 	Uint32 last_time, now_time;

 	draw_figures();
 	while(done == 0)
 	{
 		/* Verificar el tiempo al inicio de la iteración */
  		last_time = SDL_GetTicks ();

  		while ( SDL_PollEvent(&event) )
		{
			switch(event.type)
			{
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
				    	case SDLK_TAB:
				     		move_focus();
				     		break;

				     	case SDLK_UP:
				     		move_figure(SDLK_UP);
				     		break;

				     	case SDLK_DOWN:
				     		move_figure(SDLK_DOWN);
				     		break;

				     	case SDLK_LEFT:
				     		move_figure(SDLK_LEFT);
				     		break;

				     	case SDLK_RIGHT:
				     		move_figure(SDLK_RIGHT);
				     		break;

				   		case SDLK_ESCAPE:
				    		done = 1;
				    		break;
					}
					eval_position();
   					eval_mirror_image();
   					draw_figures();
					break;
				case SDL_QUIT:
					/* Salir de la loop */
					done = 1;
					break;
	   		}
   		}

   		/* Si vamos más rápido que los 24 cuadros por segundo, bajar la velocidad */
   		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS)
		{
			SDL_Delay(last_time + FPS - now_time);
   		}
   	}
	return EXIT_SUCCESS;
}

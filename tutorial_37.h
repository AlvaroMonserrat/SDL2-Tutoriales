#ifndef TUTORIAL_37_H_INCLUDED
#define TUTORIAL_37_H_INCLUDED


#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include <sstream>
#include <vector>
#include <fstream>

/*
    Tutorial 37: CERAMICAS
*/

/*Constantes*/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

//The dimensions of the level
const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

//Tile constants
const int TILE_WIDTH = 80;
const int TILE_HEIGHT = 80;
const int TOTAL_TILES = 192;
const int TOTAL_TILE_SPRITES = 12;

//The different tile sprites
const int TILE_RED = 0;
const int TILE_GREEN = 1;
const int TILE_BLUE = 2;
const int TILE_CENTER = 3;
const int TILE_TOP = 4;
const int TILE_TOPRIGHT = 5;
const int TILE_RIGHT = 6;
const int TILE_BOTTOMRIGHT = 7;
const int TILE_BOTTOM = 8;
const int TILE_BOTTOMLEFT = 9;
const int TILE_LEFT = 10;
const int TILE_TOPLEFT = 11;


//Clase Tile
class Tile
{
    public:
        //Inicializar posición y tipo
        Tile(int x, int y, int tileType);

        //Shows the tile
        void render(SDL_Rect& camera);

        //Get the tile type
        int getType();

        //Get Collision box
        SDL_Rect getBox();

    private:
        //The attributes of the tile
        SDL_Rect mBox;

        //Tule type
        int mType;
};

//Clase timer
class LTimer
{
public:
    //constructor
    LTimer();

    //clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    //gets the timer's time
    Uint32 getTicks();

    //Checks status timer
    bool isStarted();
    bool isPaused();

private:
    //the clock time when the timer started
    Uint32 mStartTicks;

    //The ticks stored when the timer was paused
    Uint32 mPausedTicks;

    //The timer status
    bool mPaused;
    bool mStarted;
};

LTimer::LTimer()
{
    //Iniciar variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void LTimer::start()
{
    //Start the timer
    mStarted = true;
    mPaused = false;

    //Obtener actual clock timer
    mStartTicks = SDL_GetTicks();
    mPausedTicks = 0;
}

void LTimer::stop()
{
    //Stop the timer
    mStarted = false;

    //Unpause the timer
    mPaused = false;

    //Clear ticks variables
    mStartTicks = 0;
    mPausedTicks = 0;
}

void LTimer::pause(){


    if(mStarted && !mPaused)
    {
        mPaused = true;


        mPausedTicks = SDL_GetTicks() - mStartTicks;
        mStartTicks = 0;
    }

}

void LTimer::unpause()
{
    if(mStarted && mPaused)
    {
        mPaused = false;

        mStartTicks = SDL_GetTicks() - mPausedTicks;

        mPausedTicks = 0;

    }
}

Uint32 LTimer::getTicks()
{
    //The actual timer time
    Uint32 time = 0;

    //If the timer is running
    if(mStarted)
    {
        //If the timer is paused
        if(mPaused)
        {
            //Return the number of ticks when the timer was paused
            time = mPausedTicks;
        }
        else
        {
            //return the current time minus the start time
            time = SDL_GetTicks() - mStartTicks;
        }
    }

    return time;
}

bool LTimer::isStarted()
{
    return mStarted;
}

bool LTimer::isPaused()
{
    return mPaused && mStarted;
}

//Clase Particle
const int TOTAL_PARTICLES = 20;

//Clase
class LTexture{
public:
    //Iniciar variables
    LTexture();

    //Desasignar memoria
    ~LTexture();

    //Cargar imagen desde una ruta especifica
    bool loadFromFile(std::string path);

    #if defined(SDL_TTF_MAJOR_VERSION)
    //Crear imagen desde un font string
    bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
    #endif
    //Desasignar textura en memoria
    void free();

    //Set modulación de colores
    void setColor(Uint8 red, Uint8 green, Uint8 blue);

    //Set blending
    void setBlendMode(SDL_BlendMode blending);

    //Set alpha modulation
    void setAlpha(Uint8 alpha);

    //Renders textura en un punto dado
    void render(int x, int y, SDL_Rect* clip = NULL,double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
    //Obtener dimensioens de la imagen
    int getWidth();
    int getHeight();

private:
    //La textura de hardware actual
    SDL_Texture* mTexture;

    //Dimensiones de la imagen
    int mWidth;
    int mHeight;
};


//Clase Dot
class Dot
{
public:
    //dimensiones del punto
    static const int DOT_WIDTH = 20;
    static const int DOT_HEIGHT = 20;

    //Maximum axis velocity
    static const int DOT_VEL = 10;

    //Inicializar
    Dot();

    //Capturar key evento
    void handleEvent(SDL_Event& e);

    //Mover el punto
    void move2(Tile *tiles[]);

    //Centers the camera over the dot
    void setCamera(SDL_Rect& camera);

    //Mostrar el punto en la pantalla
    void render(SDL_Rect& camera);

private:
    // Collision box of the dot
    SDL_Rect mBox;

    //velocidad
    int mVelX, mVelY;
};



/*Variables Globales*/
SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer =NULL;

//Scene texture
LTexture gDotTexture;
LTexture gTileTexture;
SDL_Rect gTileClips[TOTAL_TILE_SPRITES];

//Cargar una imagen individual como textura
SDL_Texture* loadTexture(std::string path);

//Event Handler
SDL_Event eventHandler;

//Flags
bool quit = false;


//Iniciar SDL y crear ventana
bool init();
//Loads media
bool loadMedia(Tile* tiles[]);
//Frees media and shuts down SDL
void close(Tile* tiles[]);

//Box collision detector
bool checkCollision(SDL_Rect a, SDL_Rect b);

//Checks collision box against set of tiles
bool touchesWall(SDL_Rect box, Tile* tiles[]);

//Sets tiles from tile map
bool setTiles(Tile *tiles[]);

//Cargan Imagen Individual
SDL_Surface* loadSurface(std::string path);


LTexture::LTexture(){
    //Inicializar
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture(){
    //Dellocate
    free();
}

bool LTexture::loadFromFile(std::string path){
    //liberar texturas preexistente
    free();

    SDL_Texture* newTexture = NULL;

    //Cargar la imagen desde una ruta especifica
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL){
        std::cout << "Error al cargar la imagen en la ruta: " << path.c_str() << std::endl;
    }else{
        //Color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

        newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if(newTexture == NULL){
            std::cout << "No se pudo crear la texture error: " << SDL_GetError();
        }else{
            //Obtener dimensiones de la imagen
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }
        //liberar puntero de superficie cargada
        SDL_FreeSurface(loadedSurface);
    }

    mTexture = newTexture;
    return mTexture != NULL;
}

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor){

    //Liberar textura preexistente
    free();

    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
    if(textSurface == NULL){
            cout << "Error, no se puedo crear la superficie: " << TTF_GetError() << endl;
    }else{
        //Crear textura desde superficie de pixeles
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if(mTexture == NULL){
            cout << "Error, no se puedo crear la textura: " << SDL_GetError() << endl;
        }else{
            //Obtener dimensiones de la imagen
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }
        //
        SDL_FreeSurface(textSurface);
    }

    return mTexture != NULL;

}
#endif

void LTexture::free(){
    //Liberar textura si existe
    if(mTexture != NULL){
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue){
    //Modular textura
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending){

    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha){
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip){
    //Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    //Set clip rendering dimensions
    if( clip != NULL){
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth(){
    return mWidth;
}

int LTexture::getHeight(){
    return mHeight;
}


Tile::Tile(int x, int y, int tileType)
{
    //Get the offsets
    mBox.x = x;
    mBox.y = y;

    //Set the  collision box
    mBox.w = TILE_WIDTH;
    mBox.h = TILE_HEIGHT;

    //Get the tile type
    mType = tileType;
}

void Tile::render(SDL_Rect& camera)
{
    //If the tile is on screen
    if(checkCollision(camera, mBox))
    {
        //Show the title
        gTileTexture.render(mBox.x - camera.x, mBox.y - camera.y, &gTileClips[mType]);
    }
}

int Tile::getType()
{
    return mType;
}

SDL_Rect Tile::getBox()
{
    return mBox;
}

Dot::Dot()
{
    //Initialize the collision box
    mBox.x = 0;
    mBox.y = 0;
	mBox.w = DOT_WIDTH;
	mBox.h = DOT_HEIGHT;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;

}


void Dot::handleEvent(SDL_Event &e)
{

    //Si una tecla fue presionada
    if(e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        switch(e.key.keysym.sym)
        {
            case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
        }
    }else if(e.type == SDL_KEYUP && e.key.repeat == 0){
        switch(e.key.keysym.sym)
        {
            case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }

    }

}

void Dot::move2(Tile *tiles[])
{
    //Move the dot left or right
    mBox.x += mVelX;

    //If the dot went too far to the left or right
    if((mBox.x < 0) || (mBox.x + DOT_WIDTH > LEVEL_WIDTH) || touchesWall(mBox, tiles))
    {
        //Move back
        mBox.x -= mVelX;
    }

    //Move the dot up or down
    mBox.y += mVelY;

    //If the dot went too far up or down
    if( ( mBox.y < 0 ) || ( mBox.y + DOT_HEIGHT > LEVEL_HEIGHT ) || (touchesWall(mBox, tiles)) )
    {
        //Move back
        mBox.y -= mVelY;
    }

}

void Dot::setCamera(SDL_Rect& camera)
{
    //Center the camera over the dot
    camera.x = (mBox.x + DOT_WIDTH / 2) - SCREEN_WIDTH / 2;
    camera.y = (mBox.y + DOT_HEIGHT / 2) - SCREEN_HEIGHT / 2;

    //Keep the camera in bounds
    if(camera.x < 0)
    {
        camera.x = 0;
    }
    if(camera.y < 0)
    {
        camera.y = 0;
    }
    if(camera.x > LEVEL_WIDTH - camera.w)
    {
        camera.x = LEVEL_WIDTH - camera.w;
    }
    if(camera.y > LEVEL_HEIGHT - camera.h)
    {
        camera.y = LEVEL_HEIGHT - camera.h;
    }
}

void Dot::render(SDL_Rect& camera)
{
    //show dot
    gDotTexture.render(mBox.x - camera.x, mBox.y - camera.y);

}




bool init(){
    //Bandera de inicialización
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Error al iniciar SDL library: " << SDL_GetError() << std::endl;
        success = false;
    }else{
        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 37", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if( gWindow == NULL){
           std::cout << "La ventana no pudo ser creada: " << SDL_GetError() << std::endl;
           success = false;
        }else{

            //Crear un render para la ventana
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(gRenderer == NULL){
                std::cout << "Renderer no pudo ser creado: " << SDL_GetError() << std::endl;
                success = false;
            }else{

                //Iniciar renderer Color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                //Iniciar PNG Loading
                int imgFlags = IMG_INIT_PNG;
                if(!( IMG_Init(imgFlags) & imgFlags)){
                    std::cout << "SDL_image no se pudo iniciar " << SDL_GetError() << std::endl;
                    success = false;
                }

            }



        }
    }

    return success;
}

bool loadMedia(Tile* tiles[]){
    //Bandera de inicialización
    bool success = true;

    //Open the font
    if(!gDotTexture.loadFromFile("images/dot.bmp"))
    {
        std::cout << "Error al cargar la imagen dot" << std::endl;
        success = false;
    }

    //Load tile texture
    if( !gTileTexture.loadFromFile( "tiles/tiles.png" ) )
    {
        std::cout << "Failed to load tile set texture!" << std::endl;
        success = false;
    }

    //Load tile map
    if( !setTiles( tiles ) )
    {
        printf( "Failed to load tile set!\n" );
        success = false;
    }

    return success;
}

bool setTiles(Tile* tile[])
{
    //
    bool tilesLoaded = true;

    //The tile offsets
    int x = 0, y = 0;

    //Open the map
    std::ifstream mapa("map/lazy.map");

    //Si no puede cargar el mapa
    if(mapa.fail())
    {
        std::cout << "El mapa no pudo ser cargado" << std::endl;
        tilesLoaded = false;
    }
    else
    {
        //Inicializar las ceramicas
        for(int i=0; i < TOTAL_TILES; ++i)
        {
            //Determines what kind of tile will be made
            int tileType = -1;

            //Read tile from map file
            mapa >> tileType;

            if(mapa.fail())
            {
                std::cout << "Error al cargar el mapa" << std::endl;
                tilesLoaded = false;
                break;
            }

            //Si el numero es un numero de ceramica valido
            if((tileType >= 0) && (tileType < TOTAL_TILE_SPRITES))
            {
                tile[i] = new Tile(x, y, tileType);
            }
            else
            {
                //Stop loading map
                std::cout << "Error al cargar el mapa en la ceramica: " << i << std::endl;
                tilesLoaded = false;
                break;
            }

            //Mover a la siguiente ceramica a colocar
            x += TILE_WIDTH;

            //Si llegamos muy lejos
            if(x >=LEVEL_WIDTH)
            {
                //Regresar a 0
                x = 0;

                //Mover a la siguiente Fila
                y += TILE_HEIGHT;
            }
        }

        //Clip the sprite sheet
        if(tilesLoaded)
        {
            gTileClips[TILE_RED].x = 0;
            gTileClips[TILE_RED].y = 0;
            gTileClips[TILE_RED].w = TILE_WIDTH;
            gTileClips[TILE_RED].h = TILE_HEIGHT;

            gTileClips[ TILE_GREEN ].x = 0;
            gTileClips[ TILE_GREEN ].y = 80;
            gTileClips[ TILE_GREEN ].w = TILE_WIDTH;
            gTileClips[ TILE_GREEN ].h = TILE_HEIGHT;

            gTileClips[ TILE_BLUE ].x = 0;
            gTileClips[ TILE_BLUE ].y = 160;
            gTileClips[ TILE_BLUE ].w = TILE_WIDTH;
            gTileClips[ TILE_BLUE ].h = TILE_HEIGHT;

            gTileClips[ TILE_TOPLEFT ].x = 80;
            gTileClips[ TILE_TOPLEFT ].y = 0;
            gTileClips[ TILE_TOPLEFT ].w = TILE_WIDTH;
            gTileClips[ TILE_TOPLEFT ].h = TILE_HEIGHT;

            gTileClips[ TILE_LEFT ].x = 80;
            gTileClips[ TILE_LEFT ].y = 80;
            gTileClips[ TILE_LEFT ].w = TILE_WIDTH;
            gTileClips[ TILE_LEFT ].h = TILE_HEIGHT;

            gTileClips[ TILE_BOTTOMLEFT ].x = 80;
            gTileClips[ TILE_BOTTOMLEFT ].y = 160;
            gTileClips[ TILE_BOTTOMLEFT ].w = TILE_WIDTH;
            gTileClips[ TILE_BOTTOMLEFT ].h = TILE_HEIGHT;

            gTileClips[ TILE_TOP ].x = 160;
            gTileClips[ TILE_TOP ].y = 0;
            gTileClips[ TILE_TOP ].w = TILE_WIDTH;
            gTileClips[ TILE_TOP ].h = TILE_HEIGHT;

            gTileClips[ TILE_CENTER ].x = 160;
            gTileClips[ TILE_CENTER ].y = 80;
            gTileClips[ TILE_CENTER ].w = TILE_WIDTH;
            gTileClips[ TILE_CENTER ].h = TILE_HEIGHT;

            gTileClips[ TILE_BOTTOM ].x = 160;
            gTileClips[ TILE_BOTTOM ].y = 160;
            gTileClips[ TILE_BOTTOM ].w = TILE_WIDTH;
            gTileClips[ TILE_BOTTOM ].h = TILE_HEIGHT;

            gTileClips[ TILE_TOPRIGHT ].x = 240;
            gTileClips[ TILE_TOPRIGHT ].y = 0;
            gTileClips[ TILE_TOPRIGHT ].w = TILE_WIDTH;
            gTileClips[ TILE_TOPRIGHT ].h = TILE_HEIGHT;

            gTileClips[ TILE_RIGHT ].x = 240;
            gTileClips[ TILE_RIGHT ].y = 80;
            gTileClips[ TILE_RIGHT ].w = TILE_WIDTH;
            gTileClips[ TILE_RIGHT ].h = TILE_HEIGHT;

            gTileClips[ TILE_BOTTOMRIGHT ].x = 240;
            gTileClips[ TILE_BOTTOMRIGHT ].y = 160;
            gTileClips[ TILE_BOTTOMRIGHT ].w = TILE_WIDTH;
            gTileClips[ TILE_BOTTOMRIGHT ].h = TILE_HEIGHT;
        }
    }

    //Close the file
    mapa.close();

    return tilesLoaded;
}

bool touchesWall(SDL_Rect box, Tile* tiles[])
{
    //Recorrer ceramicas
    for(int i = 0; i < TOTAL_TILES; ++i)
    {
        //If the tile is a wall type tile
        if( (tiles[i]->getType() >= TILE_CENTER) && (tiles[i]->getType() <= TILE_TOPLEFT))
        {
            //If the collision box touches the wall tile
            if(checkCollision(box, tiles[i]->getBox()))
            {
                return true;
            }
        }
    }

    //If no wall tiles were touched
    return false;
}

void close(Tile* tiles[]){

    //Deallocate tiles
    for(int i = 0; i < TOTAL_TILES; ++i)
    {
        if(tiles[i] != NULL)
        {
            delete tiles[i];
            tiles[i] = NULL;
        }
    }

    gDotTexture.free();
    gTileTexture.free();

    // Destruir ventana
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Cerrar SDL subsistemas
    IMG_Quit();
    SDL_Quit();
}

bool checkCollision(SDL_Rect a, SDL_Rect b)
{
    //The sides of rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    //Calculate the sides of rect B
    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    //
    if( bottomA <= topB )
    {
        return false;
    }

    if( topA >= bottomB )
    {
        return false;
    }

    if( rightA <= leftB )
    {
        return false;
    }

    if( leftA >= rightB )
    {
        return false;
    }

    //If none of the sides from A are outside B
    return true;
}

void gameRun(){
    if(!init()){
        std::cout << "Ha fallado la inicialización" << std::endl;
    }
    else
    {
        // The level tiles
        Tile* tileSet[TOTAL_TILES];

        if(!loadMedia(tileSet))
        {
            std::cout << "Error al cargar las ceramicas" << std::endl;
        }
        else
        {
            //The dot that will be moving around on the screen
            Dot dot;

            //Level camera
            SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

            while(!quit)
            {
                //start cap timer

                while(SDL_PollEvent(&eventHandler) != 0)
                {
                    if(eventHandler.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                    dot.handleEvent(eventHandler);
                }

				//Clear screen
				dot.move2(tileSet);
                dot.setCamera(camera);

                //Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

				//Render level
				for(int i = 0; i < TOTAL_TILES; ++i)
				{
                    tileSet[i]->render(camera);
				}

                //Render dot
				dot.render(camera);
                //Update
                SDL_RenderPresent(gRenderer);

            }

        }
        // Libera recursos y cerrar SDL
        close(tileSet);

    }

}


#endif // TUTORIAL_37_H_INCLUDED

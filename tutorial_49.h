#ifndef TUTORIAL_49_H_INCLUDED
#define TUTORIAL_49_H_INCLUDED

#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<GL/glew.h>
#include<SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <sstream>
#include <vector>
#include <fstream>

/*
    Tutorial 49: SDL and Modern OpenGL
*/

/*Constantes*/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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

    //Creates blank texture
    bool createBlank(int width, int height, SDL_TextureAccess = SDL_TEXTUREACCESS_STREAMING);

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

    //Set self as render target
    void setAsRenderTarget();

    //Obtener dimensioens de la imagen
    int getWidth();
    int getHeight();

    //Manipulación de pixeles
    bool lockTexture();
    bool unlockTexture();
    void* getPixels();
    void copyPixels(void* pixels);
    int getPitch();
    Uint32 getPixel32(unsigned int x, unsigned int y);

private:
    //La textura de hardware actual
    SDL_Texture* mTexture;
    void* mPixels;
    int mPitch;

    //Dimensiones de la imagen
    int mWidth;
    int mHeight;
};

/*Variables Globales*/
SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer =NULL;

//OpenGL context
SDL_GLContext gContext;

//Render flag
bool gRenderQuad = true;

//Scene textures
LTexture gCallBackTexture;

//Cargar una imagen individual como textura
SDL_Texture* loadTexture(std::string path);

//Event Handler
SDL_Event eventHandler;

//Flags
bool quit = false;

//Iniciar SDL y crear ventana
bool init();

//Iniciar matrices y clear color
bool initGL();

//Input hanlder
void handlerKeys(unsigned char key, int x, int y);

//Per Frame update
void update();

//Renders quad to the screen
void render();

//Loads media
bool loadMedia();
//Frees media and shuts down SDL
void close();

//Shader loading utility programs
void printProgramLog(GLuint program);
void printShaderLog(GLuint shader);

//Graphics program
GLuint gProgramID = 0;
GLint gVertexPos2DLocation = -1;
GLuint gVBO = 0;
GLuint gIBO = 0;

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

        //Convertir la superficie a un formato display
        SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
        if( formattedSurface == NULL)
        {
             std::cout << "No es posible convertir la superficie en formato displat SDL Error: " << SDL_GetError() << std::endl;
        }
        else
        {
            //Crear Textura fluida en blanco
            newTexture = SDL_CreateTexture(
                gRenderer,
                SDL_PIXELFORMAT_RGBA8888,
                SDL_TEXTUREACCESS_STREAMING,
                formattedSurface->w,
                formattedSurface->h
                );

            if(newTexture == NULL){
                std::cout << "No se pudo crear la texture error: " << SDL_GetError() << std::endl;
            }else{

                //Enable blending on texture
                SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

                //Lock texture para manipulación
                SDL_LockTexture(newTexture, NULL, &mPixels, &mPitch);

                //Copiar superficie de pixeles cargada/formateada
                memcpy(mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

                //Obtener dimensiones de la imagen
                mWidth = formattedSurface->w;
                mHeight = formattedSurface->h;

                //Get pixel data in editable format
                Uint32* pixels = (Uint32*)mPixels;
                int pixelCount = ( mPitch / 4 ) * mHeight;

                //Map colors
                Uint32 colorKey = SDL_MapRGB( formattedSurface->format, 0, 0xFF, 0xFF );
                Uint32 transparent = SDL_MapRGBA( formattedSurface->format, 0x00, 0xFF, 0xFF, 0x00 );

                //Color key pixels
                for( int i = 0; i < pixelCount; ++i )
                {
                    if( pixels[ i ] == colorKey )
                    {
                        pixels[ i ] = transparent;
                    }
                }

                //Unlock texture to update
                SDL_UnlockTexture(newTexture);
                mPixels = NULL;


            }
            //liberar puntero de superficie formateada
            SDL_FreeSurface(formattedSurface);
        }
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

bool LTexture::createBlank(int width, int height, SDL_TextureAccess access)
{
    //Create uninitialized texture
    mTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height);
    if(mTexture == NULL)
    {
        std::cout << "Error al crear la textura blanca" << std::endl;
    }
    else
    {
        mWidth = width;
        mHeight = height;
    }

    return mTexture != NULL;
}

void LTexture::setAsRenderTarget()
{
    //Make self render target
    SDL_SetRenderTarget(gRenderer, mTexture);
}

void LTexture::free(){
    //Liberar textura si existe
    if(mTexture != NULL){
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
        mPixels = NULL;
        mPitch = 0;
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

bool LTexture::lockTexture()
{
    bool success = true;

    //Texture is already locked
    if(mPixels != NULL)
    {
        std::cout << "Texture is already locked!" << std::endl;
        success = false;
    }
    //Lock texture
    else
    {
        if(SDL_LockTexture(mTexture, NULL, &mPixels, &mPitch) != 0)
        {
            std::cout << "Unable to lock texture!" << SDL_GetError() << std::endl;
            success = false;
        }
    }

    return success;

}

bool LTexture::unlockTexture()
{
    bool success = true;

    if(mPixels == NULL)
    {
        std::cout << "Texture is not locked!" << std::endl;
        success = false;
    }
    //Unlock texture
    else
    {
        SDL_UnlockTexture(mTexture);
        mPixels = NULL;
        mPitch = 0;
    }

    return success;
}

void* LTexture::getPixels()
{
    return mPixels;
}

void LTexture::copyPixels(void* pixels)
{
    //Texture is locked
    if(mPixels != NULL)
    {
        //Copy to locked pixels
        memcpy(mPixels, pixels, mPitch * mHeight);
    }
}

int LTexture::getPitch()
{
    return mPitch;
}

Uint32 LTexture::getPixel32(unsigned int x, unsigned int y)
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32*)mPixels;

    //Get the pixel request
    return pixels[(y * (mPitch / 4)) + x];
}


bool init(){
    //Bandera de inicialización
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Error al iniciar SDL library: " << SDL_GetError() << std::endl;
        success = false;
    }else{

        //Use OpenGL 2.1
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 49", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

        if( gWindow == NULL){
           std::cout << "La ventana no pudo ser creada: " << SDL_GetError() << std::endl;
           success = false;
        }else{

            //Crear contexto
            gContext = SDL_GL_CreateContext(gWindow);

            if(gContext == NULL)
            {
                std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
                success = false;
            }
            else{
                //Initialize GLEW
                glewExperimental = GL_TRUE;
                GLenum glewError = glewInit();

                if(glewError != GLEW_OK)
                {
                    std::cout << "Error initializing GLEW!: " << glewGetErrorString(glewError) << std::endl;
                }

                //Use Vsync
                if(SDL_GL_SetSwapInterval(1) < 0)
                {
                    std::cout << "Warning: Unable to set VSync! SDL Error: "  << SDL_GetError() << std::endl;
                }

                //Initialize OpenGL
                if(!initGL())
                {
                     std::cout << "Unable to initialize OpenGL!" << std::endl;
                    success = false;
                }

            }

        }
    }

    return success;
}

bool initGL()
{
    bool success = true;

    //Generate program
    gProgramID = glCreateProgram();

    //Create vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

    //Get vertex source
    const GLchar* vertexShaderSource[] = {
        "#version 140\nin vec2 LVertexPos2D; void main(){ gl_Position = vec4(LVertexPos2D.x, LVertexPos2D.y, 0, 1);}"
    };

    //Set vertex source
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);

    //Compile vertex source
    glCompileShader(vertexShader);

    //Check vertex shader for errors
    GLint vShaderCompiled = GL_FALSE;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);

    if(vShaderCompiled != GL_TRUE)
    {
        std::cout << "Unable to compile vertex shader " << vertexShader << std::endl;
        success = false;
    }
    else
    {
        //Attach vertex shader to program
        glAttachShader(gProgramID,  vertexShader);

        //Create fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        //Get fragment source
        const GLchar* fragmentShaderSource[] =
        {
            "#version 140\nout vec4 LFragment; void main() { LFragment = vec4( 1.0, 1.0, 1.0, 1.0 ); }"
        };

        //Set fragment source
        glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

        //Compile fragment source
        glCompileShader(fragmentShader);

        //Check fragment shader for errors
        GLint fShaderCompiled = GL_FALSE;
        glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled );
        if( fShaderCompiled != GL_TRUE )
        {
            printf( "Unable to compile fragment shader %d!\n", fragmentShader );
            printShaderLog( fragmentShader );
            success = false;
        }
        else
        {
            //Attach fragment shader to program
            glAttachShader(gProgramID, fragmentShader);

            //Link program
            glLinkProgram(gProgramID);

            //Check for errors
            GLint programSuccess = GL_TRUE;
            glGetProgramiv( gProgramID, GL_LINK_STATUS, &programSuccess );
            if( programSuccess != GL_TRUE )
            {
                printf( "Error linking program %d!\n", gProgramID );
                printProgramLog( gProgramID );
                success = false;
            }
            else
            {
                //Get vertex attribute location
                gVertexPos2DLocation = glGetAttribLocation(gProgramID, "LVertexPos2D");
                if(gVertexPos2DLocation == -1)
                {
                    std::cout << "LVertexPos2D is not a valid glsl program variable!" << std::endl;
                    success = false;
                }
                else
                {
                    //Initialize clear color
                    glClearColor(0.f, 0.f, 0.f, 1.f);

                    //VBO data (Vertex Buffer Objects)
                    GLfloat vertexData[] =
                    {
                        -0.5f, -0.5,
                         0.5f, -0.5f,
                         0.5f,  0.5f,
                        -0.5f,  0.5f,
                    };

                    //IBO data (Index Buffer Objects)
                    GLuint indexData[] = {0, 1, 2, 3};

                    //Create VBO
                    glGenBuffers(1, &gVBO);
                    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
                    glBufferData(GL_ARRAY_BUFFER, 2*4*sizeof(GLfloat), vertexData, GL_STATIC_DRAW);

                    //Create IBO
                    glGenBuffers(1, &gIBO);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), indexData, GL_STATIC_DRAW);

                }
            }
        }
    }

    return success;
}

void printProgramLog(GLuint program)
{
    //Make sure name is program
    if(glIsProgram(program))
    {
        //Program log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetProgramiv(program,  GL_INFO_LOG_LENGTH, &maxLength);

        //Allocate string
        char * infoLog = new char[maxLength];

        //Get info Log
        glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
        if(infoLogLength > 0)
        {
            //Print Log
            std::cout << infoLog << std::endl;
        }

        delete [] infoLog;
    }
    else
    {
        std::cout << "Name is not a program: " << program << std::endl;
    }

}

void printShaderLog(GLuint shader)
{
    //Make sure name is shader
    if(glIsProgram(shader))
    {
        //Program log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetShaderiv(shader,  GL_INFO_LOG_LENGTH, &maxLength);

        //Allocate string
        char * infoLog = new char[maxLength];

        //Get info Log
        glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
        if(infoLogLength > 0)
        {
            //Print Log
            std::cout << infoLog << std::endl;
        }

        delete [] infoLog;
    }
    else
    {
        std::cout << "Name is not a shader: " << shader << std::endl;
    }

}

void handlerKeys(unsigned char key, int x, int y)
{
    //Toggle quad
    if(key == 'q')
    {
        gRenderQuad = !gRenderQuad;
    }
}

void update()
{
    //No per frame update needed
}

void render()
{
    //Clear color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    //Render quad
    if(gRenderQuad)
    {
        //Bind program
        glUseProgram(gProgramID);

        //Enable vertex position
        glEnableVertexAttribArray(gVertexPos2DLocation);

        //Set vertex data
        glBindBuffer(GL_ARRAY_BUFFER, gVBO);
        glVertexAttribPointer(gVertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);

        //Set index data and render
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
        glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, NULL);

        //Disable vertex position
        glDisableVertexAttribArray(gVertexPos2DLocation);

        //Unbind program
        glUseProgram(NULL);
    }

}

bool loadMedia(){

    //Bandera de inicialización
    bool success = true;

    //Load blank texture
    if(!gCallBackTexture.loadFromFile("images/splash.png"))
    {
        std::cout << "Error al crear la background texture" << std::endl;
        success = false;
    }

    return success;
}


void close(){

    glDeleteProgram(gProgramID);
    // Destruir ventana
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    //Cerrar SDL subsistemas
    SDL_Quit();
}


void gameRun(){

    if(!init()){
        std::cout << "Ha fallado la inicialización" << std::endl;
    }
    else
    {

        //Enable text input
        SDL_StartTextInput();

        while(!quit)
        {
            while(SDL_PollEvent(&eventHandler) != 0)
            {
                if(eventHandler.type == SDL_QUIT)
                {
                    quit = true;
                }
                else if(eventHandler.type == SDL_TEXTINPUT)
                {
                    int x = 0, y = 0;
                    SDL_GetMouseState(&x, &y);
                    handlerKeys(eventHandler.text.text[0], x, y);
                }
            }

            //Render quad
            render();

            //Update screen
            SDL_GL_SwapWindow(gWindow);
        }

        //disable text input
        SDL_StopTextInput();

    }
    // Libera recursos y cerrar SDL
    close();


}


#endif // TUTORIAL_49_H_INCLUDED

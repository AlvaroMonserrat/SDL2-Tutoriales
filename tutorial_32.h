#ifndef TUTORIAL_32_H_INCLUDED
#define TUTORIAL_32_H_INCLUDED


#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include <sstream>
#include <string>
#include <vector>
#include <SDL2/SDL_ttf.h>

/*
    Tutorial 31: Leer y escribir archivos
*/


/*Constantes*/
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

//
const int TOTAL_DATA = 10;

//Maximo numero de dispositivos de grabación soportados
const int MAX_RECORDING_DEVICES = 10;

//Maximo tiempo de grabación
const int MAX_RECORDING_SECONDS = 5;

//Maximo tiempo de grabación más padding
const int RECORDING_BUFFER_SECONDS = MAX_RECORDING_SECONDS + 1;


//Acciones de grabación
enum RecordingState
{
    SELECTING_DEVICE,
    STOPPED,
    RECORDING,
    RECORDED,
    PLAYBACK,
    ERROR
};
//Recording/Playback callbacks
void audioRecordingCallback(void* userdata, Uint8* stream, int len);
void audioPlayCallback(void* userdata, Uint8* stream, int len);

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



/*Variables Globales*/
SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer =NULL;

//Globally used fond
TTF_Font *gFont = NULL;
SDL_Color gTextColor = {0, 0, 0, 0xFF};

//Cargar una imagen individual como textura
SDL_Texture* loadTexture(std::string path);

//Scene texture
LTexture gPromptTextTexture;


LTexture gDeviceTextures[MAX_RECORDING_DEVICES];

//Numero de dispositivos disponibles
int gRecordingDeviceCount = 0;

//Event Handler
SDL_Event eventHandler;

SDL_AudioSpec gReceivedRecordingSpec;
SDL_AudioSpec gReceivedPlaybackSpec;


//Recording Data buffer
Uint8* gRecordingBuffer = NULL;

//Size of data buffer
Uint32 gBufferByteSize = 0;

//Position in data buffer
Uint32 gBufferBytePosition = 0;

//Posición máxima en le buffer de data recording
Uint32 gBufferByteMaxPosition = 0;

//Flags
bool quit = false;


//Iniciar SDL y crear ventana
bool init();
//Loads media
bool loadMedia();
//Frees media and shuts down SDL
void close();


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
            std::cout << "Error, no se puedo crear la superficie: " << TTF_GetError() << std::endl;
    }else{
        //Crear textura desde superficie de pixeles
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if(mTexture == NULL){
            std::cout << "Error, no se puedo crear la textura: " << SDL_GetError() << std::endl;
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


bool init(){
    //Bandera de inicialización
    bool success = true;

    if( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
        std::cout << "Error al iniciar SDL library: " << SDL_GetError() << std::endl;
        success = false;
    }else{
        //Crear la ventana
        gWindow = SDL_CreateWindow("Tutorial 31", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

                //Iniciar SDL ttf
                if(TTF_Init() == -1){
                    std::cout << "SDL_ttf no se pudo iniciar " << TTF_GetError() << std::endl;
                    success = false;
                }

            }



        }
    }

    return success;
}

bool loadMedia(){

    //Bandera de inicialización
    bool success = true;

    //Open the font
    gFont = TTF_OpenFont("fonts/lazy.ttf", 28);

    if(gFont == NULL)
    {
        std::cout << "Error al cargar la Font" << TTF_GetError()<< std::endl;
        success = false;
    }
    else
    {
        //Set starting prompt
        gPromptTextTexture.loadFromRenderedText("Seleciona el dispositivo de grabación", gTextColor);

        //Get capture device count
        gRecordingDeviceCount = SDL_GetNumAudioDevices(SDL_TRUE);

        //No recording devices
        if(gRecordingDeviceCount < 1)
        {
            std::cout << "No disponible obtener dispositivos de grabación de audio"<< std::endl;
            success = false;
        }
        //Al menos un dispositivo conectado
        else{
            //Cap recording device count
            if(gRecordingDeviceCount > MAX_RECORDING_DEVICES)
            {
                gRecordingDeviceCount = MAX_RECORDING_DEVICES;
            }

            //Render devices names
            std::stringstream prompText;
            for(int i = 0; i < gRecordingDeviceCount; ++i)
            {
                prompText.str(" ");
                prompText << i << ": " << SDL_GetAudioDeviceName(i, SDL_TRUE);

                //Set texture from name
                gDeviceTextures[i].loadFromRenderedText(prompText.str().c_str(), gTextColor);

            }

        }
    }


    return success;
}

void close(){

    //Free Textures
    gPromptTextTexture.free();

    for(int i = 0; i < MAX_RECORDING_DEVICES; ++i)
    {
        gDeviceTextures[i].free();
    }


    TTF_CloseFont(gFont);
    gFont = NULL;

    // Destruir ventana
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Free playback audio
    if(gRecordingBuffer != NULL)
    {
        delete[] gRecordingBuffer;
        gRecordingBuffer = NULL;
    }

    //Cerrar SDL subsistemas
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void audioRecordingCallback(void* userdata, Uint8* stream, int len)
{
    //Copy audio from stream
    memcpy(&gRecordingBuffer[gBufferBytePosition], stream, len);

    //Move along buffer
    gBufferBytePosition += len;
}

void audioPlayCallback(void* userdata, Uint8* stream, int len)
{
    //Copy audio to stream
    memcpy(stream, &gRecordingBuffer[gBufferBytePosition], len);

    //Move along buffer
    gBufferBytePosition += len;
}

void gameRun(){
    if(!init()){
        std::cout << "Ha fallado la inicialización" << std::endl;
    }else{
        if(!loadMedia()){
         }else{

            //Set the default recording sate
            RecordingState currentState = SELECTING_DEVICE;

            //Audio device IDs
            SDL_AudioDeviceID recordingDeviceId = 0;
            SDL_AudioDeviceID playbackDeviceId = 0;

            while(!quit)
            {
                while(SDL_PollEvent(&eventHandler) != 0)
                {
                    if(eventHandler.type == SDL_QUIT)
                    {
                        quit = true;
                    }

                    switch( currentState )
                    {
                        case SELECTING_DEVICE:
                            if(eventHandler.type == SDL_KEYDOWN)
                            {
                                //Handle key press from 0 to 8
                                if(eventHandler.key.keysym.sym >= SDLK_0 && eventHandler.key.keysym.sym <= SDLK_9)
                                {
                                    //Get Selecion index
                                    int index = eventHandler.key.keysym.sym - SDLK_0;

                                    //Indes is valid
                                    if(index < gRecordingDeviceCount)
                                    {
                                        //Default audio spec
                                        SDL_AudioSpec desiredRecordingSpec;
                                        SDL_zero(desiredRecordingSpec);
                                        desiredRecordingSpec.freq = 44100;
                                        desiredRecordingSpec.format = AUDIO_F32;
                                        desiredRecordingSpec.channels = 2;
										desiredRecordingSpec.samples = 4096;
										desiredRecordingSpec.callback = audioRecordingCallback;

										//Open recording device
										recordingDeviceId = SDL_OpenAudioDevice(
                                            SDL_GetAudioDeviceName( index, SDL_TRUE ),
                                            SDL_TRUE, &desiredRecordingSpec,
                                            &gReceivedRecordingSpec,
                                            SDL_AUDIO_ALLOW_FORMAT_CHANGE );

                                        if(recordingDeviceId == 0)
                                        {
                                            std::cout << "Error al abrir el dispositivo de grabación" << std::endl;
                                            gPromptTextTexture.loadFromRenderedText("Error al abrir el dispositivo!", gTextColor);
                                            currentState = ERROR;
                                        }
                                        //Dispositivo abierto exitosamente
                                        else
                                        {
                                            SDL_AudioSpec desiredPlaybackSpec;
                                            SDL_zero(desiredPlaybackSpec);
                                            desiredPlaybackSpec.freq = 44100;
                                            desiredPlaybackSpec.format = AUDIO_F32;
                                            desiredPlaybackSpec.channels = 2;
                                            desiredPlaybackSpec.samples = 4096;
                                            desiredPlaybackSpec.callback = audioPlayCallback;

                                            //Abrir dispositivo de reproducción
                                            playbackDeviceId = SDL_OpenAudioDevice(
                                                NULL,
                                                SDL_FALSE,
                                                &desiredPlaybackSpec,
                                                &gReceivedPlaybackSpec,
                                                SDL_AUDIO_ALLOW_FORMAT_CHANGE
                                            );

                                            //Error al abrir dispositivo
                                            if( playbackDeviceId == 0)
                                            {
                                                std::cout << "Error al abrir el dispositivo de reproducción" << std::endl;
                                                gPromptTextTexture.loadFromRenderedText("Error al cargar dispositivo de reproducción", gTextColor);
                                                currentState = ERROR;
                                            }
                                            //Dispositivo se abrio exitosamente
                                            else
                                            {
                                                //Calculate per sample bytes
												int bytesPerSample =
												gReceivedRecordingSpec.channels * ( SDL_AUDIO_BITSIZE( gReceivedRecordingSpec.format ) / 8 );

												//Calculate bytes per seconds
												int bytesPerSecond = gReceivedRecordingSpec.freq * bytesPerSample;

												//Calculate buffer size
												gBufferByteSize = RECORDING_BUFFER_SECONDS * bytesPerSecond;

												//Calculate max buffer use
												gBufferByteMaxPosition = MAX_RECORDING_SECONDS * bytesPerSecond;

												//Allocate and initialize byte buffer
												gRecordingBuffer = new Uint8[gBufferByteSize];
												memset(gRecordingBuffer, 0, gBufferByteSize);

												//Ir al siguiente estado
												gPromptTextTexture.loadFromRenderedText("Press 1 para grabar 5 segundos.", gTextColor);
												currentState = STOPPED;

                                            }

                                        }


                                    }
                                }

                            }
                            break;

                        //User getting ready to record
                        case STOPPED:

                            //On key press
                            if(eventHandler.type == SDL_KEYDOWN)
                            {
                                //Start recording
                                if(eventHandler.key.keysym.sym == SDLK_1)
                                {
                                    //Go back to beginning of buffer
                                    gBufferBytePosition = 0;

                                    //Start recording
                                    SDL_PauseAudioDevice(recordingDeviceId, SDL_FALSE);

                                    //Go on to next state
                                    gPromptTextTexture.loadFromRenderedText("Recording...", gTextColor);
                                    currentState = RECORDING;

                                }
                            }
                            break;

                        case RECORDED:
                            //On key press
                            if(eventHandler.type == SDL_KEYDOWN)
                            {
                                //Start playback
                                if(eventHandler.key.keysym.sym == SDLK_1)
                                {
                                    //Go back to beginning of buffer
                                    gBufferBytePosition = 0;

                                    //Start playback
                                    SDL_PauseAudioDevice(playbackDeviceId, SDL_FALSE);

                                    //Go on to next state
                                    gPromptTextTexture.loadFromRenderedText("Playing...", gTextColor);
                                    currentState = PLAYBACK;
                                }
                                //Record again
                                if(eventHandler.key.keysym.sym == SDLK_2)
                                {
                                    //Reset the buffer
                                    gBufferBytePosition = 0;
                                    memset(gRecordingBuffer, 0, gBufferByteSize);

                                    //Start recording
                                    SDL_PauseAudioDevice(recordingDeviceId, SDL_FALSE);

                                    //Go on to next state
                                    gPromptTextTexture.loadFromRenderedText("Recording...", gTextColor);
                                    currentState = RECORDING;
                                }

                            }
                            break;

                    }

                }

                //Updating recording
                if(currentState == RECORDING)
                {
                    //Lock callback
                    SDL_LockAudioDevice(recordingDeviceId);

                    //Finished playback
                    if(gBufferBytePosition > gBufferByteMaxPosition)
                    {
                        //Stop recording audio
                        SDL_PauseAudioDevice(recordingDeviceId, SDL_TRUE);

                        //Go on to next state
                        gPromptTextTexture.loadFromRenderedText("Press 1 to play back. Press 2 to record again", gTextColor);
                        currentState = RECORDED;
                    }


                    //Unlock callback
                    SDL_UnlockAudioDevice(recordingDeviceId);

                }
                //Upadating
                else if( currentState == PLAYBACK)
                {
                    //Lock callback
                    SDL_LockAudioDevice(playbackDeviceId);

                    //Finish Playback
                    if(gBufferBytePosition > gBufferByteMaxPosition)
                    {
                        //Stop playing audio
                        SDL_PauseAudioDevice(playbackDeviceId, SDL_TRUE);

                        //Go on to next state
                        gPromptTextTexture.loadFromRenderedText("Press 1 to play back. Press 2 to record again", gTextColor);
                        currentState = RECORDED;
                    }

                    //Unlock callback
                    SDL_UnlockAudioDevice(playbackDeviceId);
                }

                //Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );

                //Render prompt centered at the top of the screen
                gPromptTextTexture.render((SCREEN_WIDTH - gPromptTextTexture.getWidth()) / 2, 0);

                //User is selecting
                if(currentState == SELECTING_DEVICE)
                {
                    //Render devices names
                    int yOffset = gPromptTextTexture.getHeight() * 2;
                    for(int i = 0; i < gRecordingDeviceCount; ++i)
                    {
                        gDeviceTextures[i].render(0, yOffset);
                        yOffset += gDeviceTextures[i].getHeight() + 1;
                    }
                }

                //Update
                SDL_RenderPresent(gRenderer);

            }

        }
    }

    // Libera recursos y cerrar SDL
    close();
}



#endif // TUTORIAL_32_H_INCLUDED

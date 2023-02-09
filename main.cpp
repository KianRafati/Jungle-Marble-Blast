#include <bits/stdc++.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfx.h>
#include <SDL2/SDL_mixer.h>
#define PI 3.14159265359
using namespace std;
//==================================================================== STRUCTS ====================================================================
struct Point
{
    float x , y ;
};
//------------------------------------------------------------------------------------------------------------------
struct account
{
    string username ;
    string password ;
    int highScore = 0 ;
    int pwr = 0;
};
//------------------------------------------------------------------------------------------------------------------
struct Ball
{
    Point P;
    int BallColor ; // 0: Red, 1: Green , 2:Blue , 3:Yellow
    double angle ;
    SDL_Texture* texture ;
    int pwr;    // 0: null, 1: ? pwr, 2: ice pwr, 3: rock pwr || 4: fire, 5: bomb, 6: lightning, 7: color ball, 8: meteroite
    int eff ;     // 0: null, 1: pause eff , 2: slow mo , 3: back eff
    int SampleKey ;
};
//==================================================================== VARIABLES AND DS ====================================================================
vector <account> accounts ;
vector <Ball> balls , balls2 , shootedBalls;

SDL_Renderer * m_renderer;
SDL_Window * m_window;
SDL_Event event ;
bool SoundState = true , MusicState = true;

//==================================================================== ALL FUNCTIONS ====================================================================
void inGame(int Ultimate , int W, int H, int mode,int mapnum,account* anAccount);
void inMenu(int W,int H);
void RegisterMenu(int W, int H);
bool AreYouSureMenu(int W, int H);
void mapSelect(int W,int H,account* anAccount);
void GameMenu(int W,int H,account* anAccount);
void SettingsMenu(int W,int H);
void LBoard(int W,int H);
void modeSelect(int W, int H, account* anAccount);
void LastMenu(int W,int H,account* anAccount,string score, int GameState,int mapnum , int mode);
//------------------------------------------------------------------------------------------------------------------
void rect(SDL_Renderer *Renderer, int x,int y,int w,int h,int R, int G, int B, int fill_boolian )
{
    SDL_Rect rectangle ;
    rectangle.x = x;
    rectangle.y = y;
    rectangle.w = w;
    rectangle.h = h;
    SDL_SetRenderDrawColor(Renderer, R, G, B, 255);
    if (fill_boolian==1)
        SDL_RenderFillRect(Renderer, &rectangle);
    SDL_RenderDrawRect(Renderer, &rectangle);
}
//------------------------------------------------------------------------------------------------------------------
float getAngle(int x1, int y1, int x2, int y2)
{
    int deltax=x1-x2;
    int deltay=(y1-y2);

    double theta=atan2(deltay,deltax);
    float angle =   theta* (180 / PI)-30;
    return angle >= 0 ? angle : 360 + angle;
}
//------------------------------------------------------------------------------------------------------------------
void specialLine(SDL_Renderer* m_renderer ,int x1, int y1,int x2,int y2 , int L , int R , int G, int B)
{
    int n , S ;
    S = sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
    n = S/L ;
    for(int i = 1 ; i <= 2*n-1 ; i++)
        if(i%2)
            aalineRGBA(m_renderer,x2 - ((x2-x1)/(2*n-1))*i,y2 - ((y2-y1)/(2*n-1))*i,x2 - ((x2-x1)/(2*n-1))*(i-1),y2 - ((y2-y1)/(2*n-1))*(i-1),R,G,B,255) ;
}
//------------------------------------------------------------------------------------------------------------------
void ShowImage (SDL_Renderer *m_renderer,SDL_Texture *image,double x,double y,int width,int height,double angle)
{
    SDL_Rect rectangle{x,y,width,height};
    //SDL_QueryTexture(image,NULL,NULL,&width,&height);
    SDL_RenderCopyEx(m_renderer,image,NULL,&rectangle,angle,NULL,SDL_FLIP_NONE);
}
//------------------------------------------------------------------------------------------------------------------
void betterText(
    SDL_Renderer *renderer, string S,
    int x, int y,
    SDL_Color color, int a,
    int size, string Font /*def = arial */)
{
    if (!TTF_WasInit())
        TTF_Init();
    string fontDir = "./fonts/" + Font + ".ttf"; // should have fonts as .ttf files in ./fonts folder from project path
    TTF_Font *font = TTF_OpenFont(fontDir.c_str(), size);
    SDL_Surface *surface = TTF_RenderText_Solid(font, S.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect;
    int h, w;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    rect.h = h;
    rect.w = w;
    rect.x = x - w / 2;
    rect.y = y - h / 2;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    // free resources
    TTF_CloseFont(font);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
//------------------------------------------------------------------------------------------------------------------
int WRND(vector<int> choice_weight)
{
    int sum_of_weight = 0 ;
    for(int i=0; i<choice_weight.size() ; i++) sum_of_weight += choice_weight[i];
    int numRnd = rand()%sum_of_weight;
    for(int i=0; i<choice_weight.size(); i++)
    {
        if(numRnd < choice_weight[i])
            return i;
        numRnd -= choice_weight[i];
    }
}
//------------------------------------------------------------------------------------------------------------------
double L(Point A, Point B)
{
    return sqrt((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y));
}
//------------------------------------------------------------------------------------------------------------------
 string TextBox(double x , double y, SDL_Texture*Texture )
{
    string name = "" ;
    bool caps_lock = false;
    SDL_Color Color{0,0,0} ;
    int A=0.5;
    bool quit = false ;
    char ch=0;
    SDL_Event *event = new SDL_Event();
    while (!quit)
    {
        SDL_RenderClear(m_renderer);
        SDL_RenderCopy(m_renderer,Texture,NULL,NULL);
        if(SDL_PollEvent(event))
        {
            if(event -> type == SDL_KEYUP)
            {
                switch(event->key.keysym.sym)
                {
                    case SDLK_SPACE:
                        ch = ' ';
                        break;
                    case SDLK_RETURN:
                        quit = true;
                        return name;
                        break;
                    case SDLK_CAPSLOCK:
                        ch=0;
                        caps_lock = !caps_lock;
                        break;
                    case SDLK_BACKSPACE:
                        ch = 0;
                        if(name.length()!=0)
                            name.pop_back();
                        break;
                    default :
                        if(event->key.keysym.sym != SDLK_UP && event->key.keysym.sym != SDLK_DOWN && event->key.keysym.sym != SDLK_LEFT && event->key.keysym.sym != SDLK_RIGHT)
                        ch = event->key.keysym.sym;
                        break;
                }
                if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch == ' ') && name.length()<25 && !quit)
                {
                    if(caps_lock == 1 && ch != ' ')
                    name+=ch + 'A' - 'a';
                    else
                    name+=ch ;
                }
            }
        }
        if(name != "" && !quit)
        betterText(m_renderer,name,x,y,Color,255,30,"pythia");
        SDL_RenderPresent(m_renderer);
        SDL_Delay(10);
    }
}
//------------------------------------------------------------------------------------------------------------------
string PasswordTextBox(double x , double y, SDL_Texture*Texture )
{
    string name = "" , astrics = "" ;
    bool caps_lock = false;
    SDL_Color Color{0,0,0} ;
    int A=0.5;
    bool quit = false ;
    char ch=0;
    SDL_Event *event = new SDL_Event();
    while (!quit)
    {
        SDL_RenderClear(m_renderer);
        SDL_RenderCopy(m_renderer,Texture,NULL,NULL);
        if(SDL_PollEvent(event))
        {
            if(event -> type == SDL_KEYUP)
            {
                switch(event->key.keysym.sym)
                {
                    case SDLK_SPACE:
                        ch = ' ';
                        break;
                    case SDLK_RETURN:
                        quit = true;
                        return name;
                        break;
                    case SDLK_CAPSLOCK:
                        ch=0;
                        caps_lock = !caps_lock;
                        break;
                    case SDLK_BACKSPACE:
                        ch = 0;
                        if(name.length()!=0)
                        {
                            name.pop_back();
                            astrics.pop_back();
                        }
                        break;
                    default :
                        if(event->key.keysym.sym != SDLK_UP && event->key.keysym.sym != SDLK_DOWN && event->key.keysym.sym != SDLK_LEFT && event->key.keysym.sym != SDLK_RIGHT)
                        ch = event->key.keysym.sym;
                        break;
                }
                if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch == ' ') && name.length()<25 && !quit)
                {
                    if(caps_lock == 1 && ch != ' ')
                    {
                        name+=ch + 'A' - 'a';
                        astrics+='*';
                    }
                    else
                    {
                        name+=ch ;
                        astrics+='*';
                    }
                }
            }
        }
        if(astrics != "" && !quit)
        betterText(m_renderer,astrics,x,y,Color,255,30,"pythia");
        SDL_RenderPresent(m_renderer);
        SDL_Delay(10);
    }
}
//------------------------------------------------------------------------------------------------------------------
bool cmp(account A, account B)
{
    if(A.highScore > B.highScore)
        return true;
    else
        return false;
}
//--------------------------------------------------------- File Functions ---------------------------------------------------------
void import_accounts_from_Txt_file()
{
    account temp_account ;
    fstream accountsTXTfile ;
    accountsTXTfile.open("accountsTXTfile.txt");
    while(accountsTXTfile.good())
    {
        accountsTXTfile >> temp_account.username ;
        accountsTXTfile >> temp_account.password ;
        accountsTXTfile >> temp_account.highScore ;
        accountsTXTfile >> temp_account.pwr ;
        accounts.push_back(temp_account) ;
    }
    accounts.pop_back();
    accountsTXTfile.close();
}
//------------------------------------------------------------------------------------------------------------------
void update_Txt_file()
{
    fstream accountsTXTfile ;
    accountsTXTfile.open("accountsTXTfile.txt");
    for(int i = 0 ; i < accounts.size() ; i++)
    {
        accountsTXTfile << accounts[i].username << " " ;
        accountsTXTfile << accounts[i].password << " " ;
        accountsTXTfile << accounts[i].highScore << " " ;
        accountsTXTfile << accounts[i].pwr << endl ;
    }
    accountsTXTfile.close();
}
//------------------------------------------------------------------------------------------------------------------
int checking_user_and_password_of_input(string user , string pass)
{
    for(int i = 0 ; i < accounts.size() ; i++)
    {
        if(accounts[i].username == user)
        {
            if(accounts[i].password == pass)
                return 2; //true pass for existing user
            else
                return 1; //false pass for existing user
        }
    }
    return 0; //user doesn't exist
}
//------------------------------------------------------------------------------------------------------------------
void add_new_account_to_vector(string user , string pass)
{
    account temp_account ;
    temp_account.username = user , temp_account.password = pass ;
    accounts.push_back(temp_account) ;
}
//==================================================================== MAIN FUNC ====================================================================
int main( int argc, char * argv[] )
{
    //{pre process
    import_accounts_from_Txt_file() ;
    //}pre process

    Uint32 SDL_flags = SDL_INIT_VIDEO | SDL_INIT_TIMER ;
    Uint32 WND_flags = SDL_WINDOW_SHOWN ;//|| SDL_WINDOW_FULLSCREEN_DESKTOP;//SDL_WINDOW_BORDERLESS ;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO );
    SDL_Init( SDL_flags );
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    int W = DM.w;//1280;//DM.w/2;
    int H = DM.h;//800;//DM.h/2;
    SDL_CreateWindowAndRenderer( W,H , WND_flags, &m_window, &m_renderer );
    SDL_RaiseWindow(m_window);

//    account * anAccount;
//    anAccount->pwr = 1;
//    inGame(1,W,H,0,0,anAccount);
    inMenu(W,H);

    //{kill program
    SDL_DestroyWindow( m_window );
    SDL_DestroyRenderer( m_renderer );
    IMG_Quit();
    SDL_Quit();
    return 0;
    //}kill program
}
//------------------------------------------------------------------------------------------------------------------
bool AreYouSureMenu(int W,int H)
{
    SDL_Texture * MenuBackground = IMG_LoadTexture(m_renderer,"./Assets/Blured_background.png");
    SDL_Texture * WoodButton3 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton3.png");
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
    Mix_Music* MenuBackgroundSound1 = Mix_LoadMUS("./Assets/Music/The-Road-Home.mp3");
    Mix_Music* MenuBackgroundSound2 = Mix_LoadMUS("./Assets/Music/Minstrels-Song.mp3");
    Mix_Music* MenuBackgroundSound3 = Mix_LoadMUS("./Assets/Music/The-Pyre.mp3");
    Mix_Music* MenuBackgroundSound4 = Mix_LoadMUS("./Assets/Music/a-robust-crew.mp3");
    Mix_Chunk* ButtonClick = Mix_LoadWAV("./Assets/Music/ButtonSoundEffect.mp3");
    Mix_Music * BM[] = {MenuBackgroundSound1,MenuBackgroundSound2,MenuBackgroundSound3,MenuBackgroundSound4};
    int Button_w = 600, Button_h = 200 , dis_btwButtons = 50 ;
    SDL_Color text_color_Bu = { 250, 250, 250};
    SDL_Color text_color_Bs = {30,30,30} ;

    while(true)
    {
        SDL_RenderClear(m_renderer);
        bool onButton1 = false , onButton2 = false;

        int Mouse_x , Mouse_y ;
        SDL_GetMouseState(&Mouse_x,&Mouse_y);

        if((Mouse_x >= W/2-Button_w-dis_btwButtons/2 && Mouse_x <= W/2-dis_btwButtons/2)&&(Mouse_y >= H/2 - Button_h/2 - Button_h && Mouse_y <= H/2 + Button_h/2))
            onButton1 = true ;
        if((Mouse_x >= W/2+dis_btwButtons/2 && Mouse_x <= W/2+Button_w+dis_btwButtons/2)&&(Mouse_y >= H/2 - Button_h/2 - Button_h && Mouse_y <= H/2 + Button_h/2))
            onButton2 = true ;

        ShowImage(m_renderer,MenuBackground,0,0,W,H,0);
        ShowImage(m_renderer,WoodButton3,W/2-Button_w - dis_btwButtons/2 , H/2 - Button_h/2,Button_w,Button_h,0);
        ShowImage(m_renderer,WoodButton3,W/2+ dis_btwButtons/2, H/2 - Button_h/2,Button_w,Button_h,0);
        betterText(m_renderer,"Are you sure you want to quit?",W/2,H/2 - 100 ,text_color_Bu,255,50,"pythia");
        if(Mix_PlayingMusic()==0)
            Mix_PlayMusic(BM[rand()%4],1);


        if(onButton1)
            betterText(m_renderer,"YES",W/2-Button_w/2 - dis_btwButtons/2,H/2 ,text_color_Bs,255,50,"WarPriestCondensed-2Z8X");
        else
            betterText(m_renderer,"YES",W/2-Button_w/2 - dis_btwButtons/2,H/2 ,text_color_Bu,255,50,"WarPriestCondensed-2Z8X");

        if(onButton2)
            betterText(m_renderer,"NO",W/2+Button_w/2+dis_btwButtons/2,H/2 ,text_color_Bs,255,50,"WarPriestCondensed-2Z8X");
        else
            betterText(m_renderer,"NO",W/2+Button_w/2+dis_btwButtons/2,H/2 ,text_color_Bu,255,50,"WarPriestCondensed-2Z8X");

        SDL_RenderPresent(m_renderer);
        if(SDL_PollEvent(&event))
        {
            if(onButton1 && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                return true;
            }
            if(onButton2 && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                return false;
            }
        }
        SDL_Delay(10);
    }
}
//------------------------------------------------------------------------------------------------------------------
void PauseMenu(int W,int H,account* anAccount)
{
    SDL_Texture * MenuBackground = IMG_LoadTexture(m_renderer,"./Assets/Blured_background.png");
    SDL_Texture * WoodButton1 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton1.png");
    SDL_Texture * WoodButton2 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton2.png");

    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
    Mix_Music* MenuBackgroundSound1 = Mix_LoadMUS("./Assets/Music/The-Road-Home.mp3");
    Mix_Music* MenuBackgroundSound2 = Mix_LoadMUS("./Assets/Music/Minstrels-Song.mp3");
    Mix_Music* MenuBackgroundSound3 = Mix_LoadMUS("./Assets/Music/The-Pyre.mp3");
    Mix_Music* MenuBackgroundSound4 = Mix_LoadMUS("./Assets/Music/a-robust-crew.mp3");
    Mix_Chunk* ButtonClick = Mix_LoadWAV("./Assets/Music/ButtonSoundEffect.mp3");
    Mix_Music * BM[] = {MenuBackgroundSound1,MenuBackgroundSound2,MenuBackgroundSound3,MenuBackgroundSound4};

    int Button_w = 600, Button_h = 200 , dis_btwButtons = 50 ;
    SDL_Color text_color_Bu = { 250, 250, 250};
    SDL_Color text_color_Bs = {30,30,30} ;
    while(true)
    {
        SDL_RenderClear(m_renderer);
        if(Mix_PlayingMusic()==0 && MusicState)
            Mix_PlayMusic(BM[rand()%4],1);

        bool onButton1 = false , onButton2 = false, onButton3 = false;

        int Mouse_x , Mouse_y ;
        SDL_GetMouseState(&Mouse_x,&Mouse_y);

        ShowImage(m_renderer,MenuBackground,0,0,W,H,0);

        ShowImage(m_renderer,WoodButton1,W/2-Button_w/2,H/2-1.5*Button_h-dis_btwButtons,Button_w,Button_h,0);
        ShowImage(m_renderer,WoodButton2,W/2-Button_w/2,H/2-Button_h/2,Button_w,Button_h,0);
        ShowImage(m_renderer,WoodButton1,W/2-Button_w/2,H/2+dis_btwButtons+Button_h/2,Button_w,Button_h,0);

        if((Mouse_x >= W/2 - Button_w/2 && Mouse_x <= W/2 + Button_w/2)&&(Mouse_y >= H/2-1.5*Button_h-dis_btwButtons && Mouse_y <= H/2-Button_h/2-dis_btwButtons))
            onButton1 = true;
        if((Mouse_x >= W/2 - Button_w/2 && Mouse_x <= W/2 + Button_w/2)&&(Mouse_y >= H/2-Button_h/2 && Mouse_y <= H/2+Button_h/2))
            onButton2 = true;
        if((Mouse_x >= W/2 - Button_w/2 && Mouse_x <= W/2 + Button_w/2)&&(Mouse_y >= H/2+dis_btwButtons+Button_h/2 && Mouse_y <= H/2+dis_btwButtons+Button_h*1.5))
            onButton3 = true ;

        if(onButton1)
            betterText(m_renderer,"Resume",W/2,H/2 - dis_btwButtons - Button_h ,text_color_Bs,255,50,"WarPriestCondensed-2Z8X");
        else
            betterText(m_renderer,"Resume",W/2,H/2 - dis_btwButtons - Button_h ,text_color_Bu,255,50,"WarPriestCondensed-2Z8X");
        if(onButton2)
            betterText(m_renderer,"Settings",W/2,H/2-30,text_color_Bs,255,50,"WarPriestCondensed-2Z8X");
        else
            betterText(m_renderer,"Settings",W/2,H/2-30,text_color_Bu ,255,50,"WarPriestCondensed-2Z8X");
        if(onButton3)
            betterText(m_renderer,"Exit",W/2,H/2+dis_btwButtons+Button_h,text_color_Bs,255,50,"pythia");
        else
            betterText(m_renderer,"Exit",W/2,H/2+dis_btwButtons+Button_h,text_color_Bu,255,50,"pythia");

        SDL_RenderPresent(m_renderer);
        if(SDL_PollEvent(&event))
        {
            if(onButton1 && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                return;
            }
            if(onButton2 && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                SettingsMenu(W,H);
            }
            if(onButton3 && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                GameMenu(W,H,anAccount);
            }
        }
        SDL_Delay(10);
    }
}
//------------------------------------------------------------------------------------------------------------------
void LastMenu(int W,int H,account* anAccount,string score, int GameState, int mapnum,int mode)
{
    SDL_Texture * MenuBackground = IMG_LoadTexture(m_renderer,"./Assets/Blured_background.png");
    SDL_Texture * wood1 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton1.png");
    SDL_Texture * wood2 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton3.png");
    SDL_Texture * ClosedChest = IMG_LoadTexture(m_renderer,"./Assets/ClosedChest.png");
    SDL_Texture * OpenedChest = IMG_LoadTexture(m_renderer,"./Assets/OpenedChest.png");
    SDL_Texture * PwrFire = IMG_LoadTexture(m_renderer,"./Assets/fireball_0.png");
    SDL_Texture * PwrBomb = IMG_LoadTexture(m_renderer,"./Assets/bomb.png");
    SDL_Texture * PwrLightnin = IMG_LoadTexture(m_renderer,"./Assets/lightning.png");
    SDL_Texture * PwrColorBall = IMG_LoadTexture(m_renderer,"./Assets/Ball_4Colors.png");
    SDL_Texture * PwrMetiorite = IMG_LoadTexture(m_renderer,"./Assets/meteorite.png");

    float Button_w = 600, Button_h = 200 , dis_btwButtons = 10, prize_w = 800 , prize_h = 500 ;
    float rect_w = 600 ;
    SDL_Color text_color_Bu = { 250, 250, 250};
    SDL_Color text_color_Bs = {30,30,30};
    Mix_Music* MenuBackgroundSound1 = Mix_LoadMUS("./Assets/Music/The-Road-Home.mp3");
    Mix_Music* MenuBackgroundSound2 = Mix_LoadMUS("./Assets/Music/Minstrels-Song.mp3");
    Mix_Music* MenuBackgroundSound3 = Mix_LoadMUS("./Assets/Music/The-Pyre.mp3");
    Mix_Music* MenuBackgroundSound4 = Mix_LoadMUS("./Assets/Music/a-robust-crew.mp3");
    Mix_Chunk* ButtonClick = Mix_LoadWAV("./Assets/Music/ButtonSoundEffect.mp3");
    Mix_Music * BM[] = {MenuBackgroundSound1,MenuBackgroundSound2,MenuBackgroundSound3,MenuBackgroundSound4};

    while(true)
    {
        SDL_RenderClear(m_renderer);
        bool onRetry = false , onExit = false;

        ShowImage(m_renderer,MenuBackground,0,0,W,H,0);
        ShowImage(m_renderer,wood1,W/2-Button_w/2,H/2-rect_w/2-dis_btwButtons-Button_h,Button_w,Button_h,0);
        if(GameState)
            betterText(m_renderer,"You won!",W/2,H/2-rect_w/2-dis_btwButtons-Button_h/2,text_color_Bu,255,50,"pythia");
        else
            betterText(m_renderer,"You lost!",W/2,H/2-rect_w/2-dis_btwButtons-Button_h/2,text_color_Bu,255,50,"pythia");

        rect(m_renderer,W/2 - rect_w/2 ,H/2-rect_w/2 , rect_w, rect_w,50,50,50,1);
        rect(m_renderer,W/2 - rect_w/2 ,H/2-rect_w/2 , rect_w, rect_w,255,255,255,0);
        betterText(m_renderer,"Your score is: "+score,W/2,H/2,text_color_Bu,255,40,"WarPriestCondensed-2Z8X");

        ShowImage(m_renderer,wood2,W/2-dis_btwButtons/2-Button_w,H/2+rect_w/2+dis_btwButtons,Button_w,Button_h,0);
        ShowImage(m_renderer,wood2,W/2+dis_btwButtons/2,H/2+rect_w/2+dis_btwButtons,Button_w,Button_h,0);

        int Mouse_x , Mouse_y ;
        SDL_GetMouseState(&Mouse_x,&Mouse_y);

        if((Mouse_x >= W/2-dis_btwButtons/2-Button_w && Mouse_x <= W/2-dis_btwButtons/2)&&(Mouse_y >= H/2+rect_w/2+dis_btwButtons && Mouse_y <= H/2+rect_w/2+dis_btwButtons+Button_h))
            onRetry = true ;
        if((Mouse_x >= W/2+dis_btwButtons/2&& Mouse_x <= W/2+dis_btwButtons/2+Button_w)&&(Mouse_y >= H/2+rect_w/2+dis_btwButtons && Mouse_y <= H/2+rect_w/2+dis_btwButtons+Button_h))
            onExit = true ;

        if(onRetry)
            betterText(m_renderer,"RETRY",W/2-dis_btwButtons/2-Button_w/2,H/2+rect_w/2+dis_btwButtons+Button_h/2,text_color_Bs,255,50,"pythia");
        else
            betterText(m_renderer,"RETRY",W/2-dis_btwButtons/2-Button_w/2,H/2+rect_w/2+dis_btwButtons+Button_h/2,text_color_Bu,255,50,"pythia");

        if(onExit)
            betterText(m_renderer,"EXIT",W/2+dis_btwButtons/2+Button_w/2,H/2+rect_w/2+dis_btwButtons+Button_h/2,text_color_Bs,255,50,"pythia");
        else
            betterText(m_renderer,"EXIT",W/2+dis_btwButtons/2+Button_w/2,H/2+rect_w/2+dis_btwButtons+Button_h/2,text_color_Bu,255,50,"pythia");


        if(SDL_PollEvent(&event))
        {
            if(event.type == SDL_MOUSEBUTTONDOWN && onRetry)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                inGame(anAccount->pwr,W,H,mode,mapnum,anAccount);
            }
            if(event.type == SDL_MOUSEBUTTONDOWN && onExit)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                break;
            }
        }
        SDL_RenderPresent(m_renderer);
        SDL_Delay(10);
    }

    vector<int> choice_weight_pwr{40,30,15,10,5};
    srand(time(0));
    int selected = WRND(choice_weight_pwr);
    SDL_Texture * prize ;
    switch(selected)
    {
        case 0:
            prize = PwrFire;
            break;
        case 1:
            prize = PwrBomb;
            break;
        case 2:
            prize = PwrLightnin;
            break;
        case 3:
            prize = PwrColorBall;
            break;
        case 4:
            prize = PwrMetiorite;
            break;
    }

    anAccount->pwr = selected;
    update_Txt_file();
    bool onPrize = false , Clicked = false;

    while(true)
    {
        SDL_RenderClear(m_renderer);
        ShowImage(m_renderer,MenuBackground,0,0,W,H,0);
        if(Clicked == false)
            ShowImage(m_renderer,ClosedChest,W/2-prize_w/2,H/2-prize_h/2,prize_w,prize_h,0);
        else
        {
            ShowImage(m_renderer,OpenedChest,W/2-prize_w/2,H/2-prize_h/2,prize_w,prize_h,0);
            ShowImage(m_renderer,prize,W/2-100,H/2-300,200,200,0);
        }

        int Mouse_x , Mouse_y ;
        SDL_GetMouseState(&Mouse_x,&Mouse_y);

        if((Mouse_x >= W/2 - prize_w/2 && Mouse_x <= W/2 + prize_w/2)&&(Mouse_y >= H/2 - prize_h/2&&Mouse_y <= H/2 +prize_h/2))
            onPrize = true;

        if(SDL_PollEvent(&event))
        {
            if(event.type == SDL_MOUSEBUTTONDOWN && onPrize)
            {
                    if(SoundState)
                        Mix_PlayChannel(-1,ButtonClick,0);
                    Clicked = true;
            }
            if(event.type = SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_ESCAPE)
                {
                    if(SoundState)
                        Mix_PlayChannel(-1,ButtonClick,0);
                    GameMenu(W,H,anAccount);
                }
            }
        }



        SDL_RenderPresent(m_renderer);
        SDL_Delay(10);
    }
}
//------------------------------------------------------------------------------------------------------------------
void inGame(int Ultimate,int W,int H,int mode,int mapnum,account* anAccount)
{
    switch(mapnum)
    {
        case 0:
            srand(time(0));
            break;
        case 1:
            srand(100);
            break;
        case 2:
            srand(2000);
            break;
        case 3:
            srand(3);
            break;
        default:
            break;
    }
    SDL_Texture * background;
    SDL_Texture *canon ;
    SDL_Texture *Hole;
    Hole = IMG_LoadTexture(m_renderer,"./Assets/hole.png");
    background = IMG_LoadTexture(m_renderer,"./Assets/background.png");
    canon = IMG_LoadTexture(m_renderer,"./Assets/frog.png");
    bool flag = false , metAnimation = false;
    int canon_x , canon_y , canon_w = 100 , canon_h = 150 ;
    SDL_Point canon_center ;
    canon_x = rand()%(W-canon_w -100) + canon_w ; canon_y = rand()%(H-canon_h-100) + canon_h ;
    canon_center.x = canon_x + canon_w/2 ;
    canon_center.y = canon_y + canon_h/2 ;
    double t = 0 , x0 = 0 , y0 = H/2 ,
     a = rand()%10 - 1 , b = rand()%5 + 20 ,
     c = rand()%10 - 1 ,d= rand()%5+ 20 ,
     e = 10 , f = 5 ,
     x = x0 , y = y0 ;

    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
    Mix_Music* MenuBackgroundSound1 = Mix_LoadMUS("./Assets/Music/The-Road-Home.mp3");
    Mix_Music* MenuBackgroundSound2 = Mix_LoadMUS("./Assets/Music/Minstrels-Song.mp3");
    Mix_Music* MenuBackgroundSound3 = Mix_LoadMUS("./Assets/Music/The-Pyre.mp3");
    Mix_Music* MenuBackgroundSound4 = Mix_LoadMUS("./Assets/Music/a-robust-crew.mp3");
    Mix_Chunk* ButtonClick = Mix_LoadWAV("./Assets/Music/ButtonSoundEffect.mp3");
    Mix_Chunk* BallHit = Mix_LoadWAV("./Assets/Music/hit.mp3");
    Mix_Chunk* WinSound = Mix_LoadWAV("./Assets/Music/game-win.mp3");
    Mix_Chunk* LoseSound = Mix_LoadWAV("./Assets/Music/Lose.mp3");
    Mix_Chunk* BombExplosion = Mix_LoadWAV("./Assets/Music/bomb-explosion.mp3");

    Mix_Music * BM[] = {MenuBackgroundSound1,MenuBackgroundSound2,MenuBackgroundSound3,MenuBackgroundSound4};

    SDL_Texture* mapp = SDL_CreateTexture(m_renderer,SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET,W,H);
    SDL_SetRenderTarget(m_renderer,mapp);
    ShowImage(m_renderer,background,0,0,W,H,0);
    while(t<1000)
    {
        double dx = (e/10)*exp(-a*t/1000)*(b*sin(b*t/1000)+a*cos(b*t/1000)) ;
        double dy = -(f/10)*exp(-c*t/1000)*(c*sin(d*t/1000)-d*cos(d*t/1000)) ;
        filledEllipseRGBA(m_renderer,x,y,20,20,0,0,0,255);
        x += dx;
        y += dy;
        t++;
    }
    ShowImage(m_renderer,Hole,x-20,y-20,40,40,0);
    SDL_SetTextureBlendMode(mapp, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(m_renderer, NULL);
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);


    SDL_Texture *redTexture = IMG_LoadTexture(m_renderer,"./Assets/Ball_Red.png") ;
    SDL_Texture *greenTexture = IMG_LoadTexture(m_renderer,"./Assets/Ball_Green.png") ;
    SDL_Texture *blueTexture = IMG_LoadTexture(m_renderer,"./Assets/Ball_Blue.png") ;
    SDL_Texture *yellowTexture = IMG_LoadTexture(m_renderer,"./Assets/Ball_Yellow.png") ;

    SDL_Texture *redIceTexture = IMG_LoadTexture(m_renderer,"./Assets/ball_red_ice.png") ;
    SDL_Texture *greenIceTexture = IMG_LoadTexture(m_renderer,"./Assets/ball_green_ice.png") ;
    SDL_Texture *blueIceTexture = IMG_LoadTexture(m_renderer,"./Assets/ball_blue_ice.png") ;
    SDL_Texture *yellowIceTexture = IMG_LoadTexture(m_renderer,"./Assets/ball_yellow_ice.png") ;

    SDL_Texture *redRandomTexture = IMG_LoadTexture(m_renderer,"./Assets/Ball_Red_random.png") ;
    SDL_Texture *greenRandomTexture = IMG_LoadTexture(m_renderer,"./Assets/Ball_Green_random.png") ;
    SDL_Texture *blueRandomTexture = IMG_LoadTexture(m_renderer,"./Assets/Ball_Blue_random.png") ;
    SDL_Texture *yellowRandomTexture = IMG_LoadTexture(m_renderer,"./Assets/Ball_Yellow_random.png") ;
    SDL_Texture * randomTxt[4] = {redRandomTexture,greenRandomTexture,blueRandomTexture,yellowRandomTexture};
    SDL_Texture *LavaBallTexture = IMG_LoadTexture(m_renderer,"./Assets/lava_ball.png") ;

    SDL_Texture *fire0 = IMG_LoadTexture(m_renderer,"./Assets/fireball_0.png") ;
    SDL_Texture *fire1 = IMG_LoadTexture(m_renderer,"./Assets/fireball_1.png") ;
    SDL_Texture *fire2 = IMG_LoadTexture(m_renderer,"./Assets/fireball_2.png") ;
    SDL_Texture *fire3 = IMG_LoadTexture(m_renderer,"./Assets/fireball_3.png") ;
    SDL_Texture *fire4 = IMG_LoadTexture(m_renderer,"./Assets/fireball_4.png") ;
    SDL_Texture *fire5 = IMG_LoadTexture(m_renderer,"./Assets/fireball_5.png") ;
    SDL_Texture *fireballs[] = {fire0,fire1,fire2,fire3,fire4,fire5};

    SDL_Texture *exp0 = IMG_LoadTexture(m_renderer,"./Assets/exp0.png") ;
    SDL_Texture *exp1 = IMG_LoadTexture(m_renderer,"./Assets/exp1.png") ;
    SDL_Texture *exp2 = IMG_LoadTexture(m_renderer,"./Assets/exp2.png") ;
    SDL_Texture *exp3 = IMG_LoadTexture(m_renderer,"./Assets/exp3.png") ;
    SDL_Texture *exp4 = IMG_LoadTexture(m_renderer,"./Assets/exp4.png") ;
    SDL_Texture *exp5 = IMG_LoadTexture(m_renderer,"./Assets/exp5.png") ;
    SDL_Texture *exp6 = IMG_LoadTexture(m_renderer,"./Assets/exp6.png") ;
    SDL_Texture *exps[] = {exp0,exp1,exp2,exp3,exp4,exp5,exp6};


    SDL_Texture * bomb = IMG_LoadTexture(m_renderer,"./Assets/bomb.png") ;
    SDL_Texture * lightning = IMG_LoadTexture(m_renderer,"./Assets/lightning.png") ;
    SDL_Texture * AllColor = IMG_LoadTexture(m_renderer,"./Assets/Ball_4Colors.png") ;
    SDL_Texture * meteriote = IMG_LoadTexture(m_renderer,"./Assets/meteorite.png") ;

    int score = 0 ; SDL_Color score_color = {255,255,255};
    string scores ;

    vector<int> choice_weight_pwr{70,10,10,10} ,choice_weight_eff{90,4,4,2} , choice_weight_colorGr{5,50,30,10,5}  ;
    Ball aBaall ;

    /**Normal Ball creation**/
    balls.assign(80,aBaall);
    int groupcell = 0;
    for(int  i = 0 ; i < balls.size() ; i+=groupcell + 1) // ball creation
    {
        balls[79-i].P.x = x0 - 40*i ; balls[79-i].P.y = y0 ;
        balls[79-i].pwr = WRND(choice_weight_pwr);
        balls[79-i].eff = WRND(choice_weight_eff);
        balls[79-i].SampleKey = 79 - i ;
        if(i >= 4)
            groupcell = WRND(choice_weight_colorGr);
        else
            groupcell = (i);
        int color = rand()%4 ;
        while(color == balls[79-i+1].BallColor)
            color = rand()%4 ;
        balls[79-i].BallColor = color ;
        switch(balls[79-i].BallColor)
        {
                case 0:
                    balls[79-i].texture = redTexture;
                    break;
                case 1:
                    balls[79-i].texture = greenTexture;
                    break;
                case 2:
                    balls[79-i].texture = blueTexture;
                    break;
                case 3:
                    balls[79-i].texture = yellowTexture;
                    break;
            }
        if(balls[79-i].pwr == 2)
        {
            switch(balls[79-i].BallColor)
            {
                    case 0:
                        balls[79-i].texture = redIceTexture;
                        break;
                    case 1:
                        balls[79-i].texture = greenIceTexture;
                        break;
                    case 2:
                        balls[79-i].texture = blueIceTexture;
                        break;
                    case 3:
                        balls[79-i].texture = yellowIceTexture;
                        break;
                }
        }
        if(balls[79-i].pwr == 3)
            balls[79-i].texture = LavaBallTexture;

        for(int j = 1 ; j <= groupcell ; j++ )
        {
            balls[79-(i+j)].BallColor = balls[79-i].BallColor ;
            balls[79-(i+j)].P.x =x0 - 40*(i+j); balls[79-(i+j)].P.y = y0 ;
            balls[79-(i+j)].pwr = WRND(choice_weight_pwr);
            balls[79-(i+j)].eff = WRND(choice_weight_eff);
            balls[79-(i+j)].SampleKey = 79 - (i+j) ;
            switch(balls[79-(i+j)].BallColor)
            {
                case 0:
                    balls[79-(i+j)].texture = redTexture;
                    break;
                case 1:
                    balls[79-(i+j)].texture = greenTexture;
                    break;
                case 2:
                    balls[79-(i+j)].texture = blueTexture;
                    break;
                case 3:
                    balls[79-(i+j)].texture = yellowTexture;
                    break;
                default:
                    break;
            }
            if(balls[79-(i+j)].pwr == 2)
            {
                switch(balls[79-(i+j)].BallColor)
                {
                        case 0:
                            balls[79-(i+j)].texture = redIceTexture;
                            break;
                        case 1:
                            balls[79-(i+j)].texture = greenIceTexture;
                            break;
                        case 2:
                            balls[79-(i+j)].texture = blueIceTexture;
                            break;
                        case 3:
                            balls[79-(i+j)].texture = yellowIceTexture;
                            break;
                    }
            }
            if(balls[79-(i+j)].pwr == 3)
                balls[79-(i+j)].texture = LavaBallTexture;
        }
    }

    /** Saving sample points**/
    Point SamplePoints[1080];
    for(int i = 79 ; i >= 0 ; --i)
    {
        SamplePoints[i].x = -40*(79-i) ;
        SamplePoints[i].y = H/2 ;
    }
    for(int i = 80 ; i < 1080 ; i++)
    {
        SamplePoints[i].x = SamplePoints[i-1].x + (e/10)*exp(-a*(i-80)/1000)*(b*sin(b*(i-80)/1000)+a*cos(b*(i-80)/1000)) ;
        SamplePoints[i].y = SamplePoints[i-1].y + -(f/10)*exp(-c*(i-80)/1000)*(c*sin(d*(i-80)/1000)-d*cos(d*(i-80)/1000)) ;
    }
    int loopCount = 0 ;
    bool explosion = false ; int exp_x , exp_y , exp_w = 750 , exp_h = 400 , exp_c = 0 , exp_t = 0;
    bool flagg = true;
    int metcounter = 0 , met_x1 , met_y1,  met_x2 , met_y2, met_x3 , met_y3 , metT;

    while(true)
    {
        Ball aBall ;
        if(Mix_PlayingMusic()==0 && MusicState)
            Mix_PlayMusic(BM[rand()%4],1);
        SDL_SetRenderDrawColor(m_renderer,0,0,0,255);
        SDL_RenderClear(m_renderer);
        SDL_RenderCopy(m_renderer,mapp,NULL,NULL);
         int ballFlag = 0;

        rect(m_renderer,W - 300 ,0 , 300, 100,50,50,50,1); // score board
        rect(m_renderer,W - 300 ,0 , 300, 100,255,255,0,0);
        betterText(m_renderer,"SCORE: "+scores,W-150,50,score_color,255,40,"WarPriestCondensed-2Z8X");

        /**Speed of the push**/
        if(loopCount%15==0)
        {
            balls.front().P = SamplePoints[balls.front().SampleKey];
            balls.front().SampleKey++;
        }


        /** Showing shooted balls and Handling shooted balls collision with normal balls **/
        for(int i = 0 ; i < shootedBalls.size() && ballFlag == 0 ; i++)
        {
            if((shootedBalls[i].P.x > W) || (shootedBalls[i].P.x < 0) || (shootedBalls[i].P.y > H) || (shootedBalls[i].P.y < 0) ){
                shootedBalls.erase(shootedBalls.begin() + i);
            }
            shootedBalls[i].P.x += 30*cos(shootedBalls[i].angle*(PI/180)) ; shootedBalls[i].P.y += 30*sin(shootedBalls[i].angle*(PI/180)) ;
            ShowImage(m_renderer,shootedBalls[i].texture,shootedBalls[i].P.x -20 ,shootedBalls[i].P.y-20 ,40,40,0) ;

            for(int j = 0 ; j < balls.size() ; j++)
            {
                if(L(balls[j].P,shootedBalls[i].P)*L(balls[j].P,shootedBalls[i].P) <= 1600) // checks if the shooted ball collieded or not
                {
                    if(shootedBalls[i].pwr < 4)
                    {
                        if(SoundState)
                            Mix_PlayChannel(-1,BallHit,0);
                        int Alike_counter = 0 , t1= j , t2=j ;
                        if(balls[j].pwr == 3)
                        {
                            shootedBalls.erase(shootedBalls.begin() + i);
                            balls.erase(balls.begin()+j);
                            ballFlag++;
                            break;
                        }
                        if(balls[j].BallColor == shootedBalls[i].BallColor)
                        {
                            if(balls[j].pwr == 2)
                            {
                                balls[j].pwr = 0 ;
                                switch(balls[j].BallColor)
                                {
                                    case 0:
                                        balls[j].texture = redTexture;
                                        break;
                                    case 1:
                                        balls[j].texture = greenTexture;
                                        break;
                                    case 2:
                                        balls[j].texture = blueTexture;
                                        break;
                                    case 3:
                                        balls[j].texture = yellowTexture;
                                        break;
                                }
                                int counter = 0 , temp = balls[j].SampleKey ;
                                while(L(SamplePoints[temp] , balls[j].P)*L(SamplePoints[temp] , balls[j].P) < 1600 && counter < 4)// finds the next SamplePoint with 2R distance
                                {
                                    temp++;
                                    counter++;
                                }
                                shootedBalls[i].SampleKey = temp ;
                                shootedBalls[i].P = SamplePoints[temp] ;
                                balls.insert(balls.begin() + j +1,shootedBalls[i]);
                                shootedBalls.erase(shootedBalls.begin() + i);
                                break;
                            }

                            Alike_counter+=2;
                            while(balls[++t2].BallColor == shootedBalls[i].BallColor && balls[t2].pwr != 1 && balls[t2].pwr != 3 && t2<balls.size())
                            {
                                if(balls[t2].pwr == 2)
                                {
                                    balls[t2].pwr = 0;
                                    switch(balls[t2].BallColor)
                                    {
                                        case 0:
                                            balls[t2].texture = redTexture;
                                            break;
                                        case 1:
                                            balls[t2].texture = greenTexture;
                                            break;
                                        case 2:
                                            balls[t2].texture = blueTexture;
                                            break;
                                        case 3:
                                            balls[t2].texture = yellowTexture;
                                            break;
                                        }
                                    break;
                                }
                                Alike_counter++;
                            }
                            while(balls[--t1].BallColor == shootedBalls[i].BallColor && balls[t1].pwr != 1 && balls[t1].pwr != 3 && t1>0)
                            {
                                if(balls[t1].pwr == 2)
                                {
                                    balls[t1].pwr = 0;
                                    switch(balls[t1].BallColor)
                                            {
                                                    case 0:
                                                        balls[t1].texture = redTexture;
                                                        break;
                                                    case 1:
                                                        balls[t1].texture = greenTexture;
                                                        break;
                                                    case 2:
                                                        balls[t1].texture = blueTexture;
                                                        break;
                                                    case 3:
                                                        balls[t1].texture = yellowTexture;
                                                        break;
                                                }
                                    break;
                                }
                                Alike_counter++;
                            }
                        }
                        if(balls[t2].pwr == 1 && Alike_counter >= 3)
                        {
                            int t = balls[t2].SampleKey;
                            balls[t2] = balls[t2-1];
                            balls[t2].SampleKey = t ;
                        }
                        if(balls[t1].pwr == 1 && Alike_counter >= 3)
                        {
                            int t = balls[t1].SampleKey;
                            balls[t1] = balls[t1+1];
                            balls[t1].SampleKey = t ;
                        }
                        int counter = 0 , temp = balls[j].SampleKey ;
                        while(L(SamplePoints[temp] , balls[j].P)*L(SamplePoints[temp] , balls[j].P) < 1600 && counter < 4)// finds the next SamplePoint with 2R distance
                        {
                            temp++;
                            counter++;
                        }
                        shootedBalls[i].SampleKey = temp ;
                        shootedBalls[i].P = SamplePoints[temp] ;
                        balls.insert(balls.begin() + j +1,shootedBalls[i]);
                        shootedBalls.erase(shootedBalls.begin() + i);
                        score+=Alike_counter;
                        if(Alike_counter >= 3)  balls.erase(balls.begin()+t1+1,balls.begin()+t2+1);
                        ballFlag++;
                        break;
                    }
                    else if(shootedBalls[i].pwr == 4)
                    {
                        if(SoundState)
                            Mix_PlayChannel(-1,BallHit,0);
                        shootedBalls.erase(shootedBalls.begin()+i);
                        balls.erase(balls.begin()+j);
                        ballFlag++;
                        break;
                    }
                    else if(shootedBalls[i].pwr == 5) // bomb power bar
                    {
                        if(SoundState)
                            Mix_PlayChannel(-1,BombExplosion,0);
                        exp_x = balls[j].P.x - exp_w/2 ;
                        exp_y = balls[j].P.y - exp_w/2 ;
                        shootedBalls.erase(shootedBalls.begin()+i);
                        int destroyRange1 = 6 , destroyRange2 = 6;
                        while(j+destroyRange1 > balls.size() )
                            destroyRange1--;
                        while( j-destroyRange2 <  0)
                            destroyRange2--;
                        balls.erase(balls.begin()+j-(destroyRange2),balls.begin()+j+(destroyRange1));
                        score += destroyRange1+destroyRange2;
                        aBall.pwr = 0 ;
                        explosion = true ;
                        exp_t = 0 ;
                        ballFlag++;
                        break;
                    }
                    else if(shootedBalls[i].pwr == 7)
                    {
                        if(SoundState)
                            Mix_PlayChannel(-1,BallHit,0);
                        for(int k = 0 ; k < balls.size() ; k++)
                            if(balls[k].BallColor != balls[j].BallColor || balls[k].SampleKey <= 80)
                                balls2.push_back(balls[k]);
                        score += balls.size() - balls2.size();
                        balls.clear();
                        for(int k = 0 ; k < balls2.size() ; k++)
                            balls.push_back(balls2[k]);
                        shootedBalls.erase(shootedBalls.begin()+i);
                        balls2.clear();
                        ballFlag++;
                        break;
                    }
//                    else if(shootedBalls[i].pwr == 8)
//                    {
//                        shootedBalls.erase(shootedBalls.begin()+i);
//
//                    }
                }
            }

            scores = to_string(score) ;
            anAccount->highScore = max(anAccount->highScore,score);
            update_Txt_file();
            if(balls.size() <= 2)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,WinSound,0);
                LastMenu(W,H,anAccount,scores,1,mapnum,mode) ; // win menu
            }
        }


        if(explosion)
        {
            ShowImage(m_renderer,exps[(exp_t/10)%7],exp_x,exp_y,exp_w,exp_h,0);
            exp_c++;
            if(exp_c == 70)
            {
                explosion = false ;
                exp_c = 0 ;
            }
        }

        if(metAnimation)
        {
            int met_w = 800 ;
            ShowImage(m_renderer,meteriote,met_x1,met_y1,met_w,met_w,0);
            ShowImage(m_renderer,meteriote,met_x2,met_y2,met_w,met_w,0);
            ShowImage(m_renderer,meteriote,met_x3,met_y3,met_w,met_w,0);
            met_x1 -= 30; met_y1 += 30; met_x2 -= 30; met_y2 += 30; met_x3 -= 30; met_y3 += 30;
            metcounter++;
            if(metcounter == 100)
            {
                metAnimation = false;
                metcounter = 0 ;
            }
        }

        /** Moving balls on map **/
        for (int i = 1 ; i < balls.size()-1 ; i++)
        {
            if( L(balls[i-1].P,balls[i].P)*L(balls[i-1].P,balls[i].P) < 1600 )
            {
                int counter = 0 ;
                while(L(SamplePoints[balls[i].SampleKey] , balls[i].P)*L(SamplePoints[balls[i].SampleKey] , balls[i].P) < 1600 && counter < 4)
                {
                    balls[i].SampleKey++;
                    counter++;
                }
            }
            balls[i].P = SamplePoints[balls[i].SampleKey];
            if(balls[i].pwr != 1)
                ShowImage(m_renderer,balls[i].texture,balls[i].P.x-20,balls[i].P.y-20,40,40,0);
            else
                ShowImage(m_renderer,randomTxt[(loopCount/5)%4],balls[i].P.x-20,balls[i].P.y-20,40,40,0);
            if(balls[i].SampleKey == 1080)
            {
                anAccount->highScore = max(anAccount->highScore,score);
                if(SoundState)
                    Mix_PlayChannel(-1,LoseSound,0);
                LastMenu(W,H,anAccount,scores,0,mapnum,mode) ; // Lose menu
            }
        }

        /**Checks the ball colors in the map**/
        vector<int> ColorsInMap;
        for(int i = 0 ; i < balls.size() ; i++)
        {
            int counter = 0 ;
            if(i==0) ColorsInMap.push_back(balls[0].BallColor);
            for(int j =0 ; j < ColorsInMap.size() ; j++)
            {
                if(ColorsInMap[j] == balls[i].BallColor)
                    counter++;
            }
            if(counter==0)
                ColorsInMap.push_back(balls[i].BallColor);
        }

        /**Balls in canon**/
        if(!flag)
        {
            int selector = rand()%ColorsInMap.size();
            aBall.BallColor = ColorsInMap[selector];
            switch(ColorsInMap[selector])
            {
                case 0:
                    aBall.texture = redTexture;
                    break;
                case 1:
                    aBall.texture = greenTexture;
                    break;
                case 2:
                    aBall.texture = blueTexture;
                    break;
                case 3:
                    aBall.texture = yellowTexture;
                    break;
            }
            selector = rand()%ColorsInMap.size();
            aBaall.BallColor = ColorsInMap[selector];
            switch(ColorsInMap[selector])
            {
                case 0:
                    aBaall.texture = redTexture;
                    break;
                case 1:
                    aBaall.texture = greenTexture;
                    break;
                case 2:
                    aBaall.texture = blueTexture;
                    break;
                case 3:
                    aBaall.texture = yellowTexture;
                    break;
            }
            flag = true ;
        }


        int Mouse_x , Mouse_y ;
        SDL_GetMouseState(&Mouse_x,&Mouse_y);
        float angle = getAngle(canon_center.x,canon_center.y,Mouse_x,Mouse_y) -  60;

        ShowImage(m_renderer,canon,canon_x,canon_y,canon_w,canon_h,angle);
        ShowImage(m_renderer,aBall.texture,canon_center.x-20,canon_center.y-20,40,40,0);
        ShowImage(m_renderer,aBaall.texture,canon_center.x,canon_center.y,20,20,0);

        if(SDL_PollEvent(&event))
        {
            if(event.type == SDL_KEYDOWN) // swap balls
            {
                if(event.key.keysym.sym == SDLK_ESCAPE) // pause-menu
                {
                    PauseMenu(W,H,anAccount) ;
                    continue;
                }
                if(event.key.keysym.sym == SDLK_SPACE)
                    swap(aBall,aBaall);
                if(event.key.keysym.sym == SDLK_q)
                {
                    aBall.pwr = anAccount->pwr + 4;
                    switch(aBall.pwr)
                    {
                        case 4:
                            aBall.texture = fire0;
                            break;
                        case 5:
                            aBall.texture = bomb;
                            break;
                        case 6:
                            aBall.texture = lightning;
                            break;
                        case 7:
                            aBall.texture = AllColor;
                            break;
                        case 8:
                            int t;
                            for(int k = 0 ; k < balls.size() ; k++)
                            {
                                if(balls[k].SampleKey >= 80)
                                {
                                    t = k ;
                                    break;
                                }
                            }
                            score += balls.size()-1 - t;
                            balls.erase(balls.begin()+t+1,balls.begin()+balls.size()-1);
                            metAnimation = true;
                            metcounter = 0;
                            met_x1 = rand()%(W/2)+W/2 , met_y1 = rand()%(H/4)-H/2 ,
                            met_x2 = rand()%(W/2)+W/2 , met_y2 = rand()%(H/4)-H/2 ,
                            met_x3 = rand()%(W/2)+W/2 , met_y3 = rand()%(H/4)-H/2 ;
                            break;
                    }
                    f++;
                }
            }
            if(event.type == SDL_MOUSEBUTTONDOWN) // ball-shooting
            {
                if(f==0)
                    aBall.pwr = 0;
                f=0;
                aBall.P.x = canon_center.x ; aBall.P.y = canon_center.y;
                aBall.angle = getAngle(canon_center.x,canon_center.y,Mouse_x,Mouse_y) - 150 ;
                shootedBalls.push_back(aBall);
                flag = false ;
            }
        }
        SDL_Delay(25);
        SDL_RenderPresent(m_renderer);
        loopCount++;
        exp_t++;
    }

}
//------------------------------------------------------------------------------------------------------------------
void modeSelect(int W, int H, account* anAccount,int mapnum)
{
    SDL_Texture * MenuBackground = IMG_LoadTexture(m_renderer,"./Assets/Blured_background.png");
    SDL_Texture * Start = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton1.png");
    SDL_Texture * Settings = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton2.png");
    SDL_Texture * LeaderBoard = Start ;
    SDL_Texture * Exit = Settings ;

    float Button_w = 600, Button_h = 200 , dis_btwButtons = 10 ;
    SDL_Color text_color_Bu = { 250, 250, 250};
    SDL_Color text_color_Bs = {30,30,30};
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
    Mix_Music* MenuBackgroundSound1 = Mix_LoadMUS("./Assets/Music/The-Road-Home.mp3");
    Mix_Music* MenuBackgroundSound2 = Mix_LoadMUS("./Assets/Music/Minstrels-Song.mp3");
    Mix_Music* MenuBackgroundSound3 = Mix_LoadMUS("./Assets/Music/The-Pyre.mp3");
    Mix_Music* MenuBackgroundSound4 = Mix_LoadMUS("./Assets/Music/a-robust-crew.mp3");
    Mix_Chunk* ButtonClick = Mix_LoadWAV("./Assets/Music/ButtonSoundEffect.mp3");
    Mix_Music * BM[] = {MenuBackgroundSound1,MenuBackgroundSound2,MenuBackgroundSound3,MenuBackgroundSound4};

    while(true)
    {
        SDL_RenderClear(m_renderer);
        bool onNormal = false , onTime = false , onBird = false , onRock = false ;
        if(Mix_PlayingMusic()==0 && MusicState)
            Mix_PlayMusic(BM[rand()%4],1);

        ShowImage(m_renderer,MenuBackground,0,0,W,H,0);
        ShowImage(m_renderer,Start,W/2-Button_w/2,H/2-dis_btwButtons*1.5-Button_h*2,Button_w,Button_h,0);
        ShowImage(m_renderer,Settings,W/2-Button_w/2,H/2-dis_btwButtons/2-Button_h,Button_w,Button_h,0);
        ShowImage(m_renderer,LeaderBoard,W/2-Button_w/2,H/2+dis_btwButtons/2,Button_w,Button_h,0);
        ShowImage(m_renderer,Exit,W/2-Button_w/2,H/2+dis_btwButtons*1.5+Button_h,Button_w,Button_h,0);

        int Mouse_x , Mouse_y ;
        SDL_GetMouseState(&Mouse_x,&Mouse_y);

        if((Mouse_x >= W/2-Button_w/2 && Mouse_x <= W/2+Button_w/2)&&(Mouse_y >= H/2 - dis_btwButtons*1.5- 2*Button_h && Mouse_y <= H/2 - 1.5*dis_btwButtons-Button_h))
            onNormal = true ;
        if((Mouse_x >= W/2-Button_w/2 && Mouse_x <= W/2+Button_w/2)&&(Mouse_y >= H/2 - dis_btwButtons/2 - Button_h && Mouse_y <= H/2 - dis_btwButtons/2))
            onTime = true ;
        if((Mouse_x >= W/2-Button_w/2 && Mouse_x <= W/2+Button_w/2)&&(Mouse_y >= H/2 + dis_btwButtons/2 && Mouse_y <=H/2 + dis_btwButtons/2 +Button_h))
            onBird = true ;
        if((Mouse_x >= W/2-Button_w/2 && Mouse_x <= W/2+Button_w/2)&&(Mouse_y >= H/2 + dis_btwButtons*1.5+Button_h && Mouse_y <= H/2 + dis_btwButtons*1.5 + 2*Button_h))
            onRock = true ;

        if(onNormal)
            betterText(m_renderer,"Normal",W/2,H/2-1.5*dis_btwButtons-1.5*Button_h,text_color_Bs,255,50,"pythia");
        else
            betterText(m_renderer,"Normal",W/2,H/2-1.5*dis_btwButtons-1.5*Button_h,text_color_Bu,255,50,"pythia");
        if(onTime)
            betterText(m_renderer,"Time",W/2,H/2 - dis_btwButtons/2-Button_h/2-30,text_color_Bs,255,50,"pythia");
        else
            betterText(m_renderer,"Time",W/2,H/2 - dis_btwButtons/2-Button_h/2-30,text_color_Bu,255,50,"pythia");
        if(onBird)
            betterText(m_renderer,"Free The Bird",W/2,H/2 + dis_btwButtons/2 +Button_h/2,text_color_Bs,255,40,"pythia");
        else
            betterText(m_renderer,"Free The Bird",W/2,H/2 + dis_btwButtons/2 +Button_h/2,text_color_Bu,255,40,"pythia");
        if(onRock)
            betterText(m_renderer,"Dwayne Johnson",W/2,H/2 + dis_btwButtons*1.5 + Button_h*1.5-15,text_color_Bs,255,40,"pythia");
        else
            betterText(m_renderer,"Dwayne Johnson",W/2,H/2 + dis_btwButtons*1.5 + Button_h*1.5-15,text_color_Bu,255,40,"pythia");

         if(SDL_PollEvent(&event))
        {
            if(onNormal && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                inGame(anAccount->pwr,W,H,0,mapnum,anAccount);
            }
            else if(onTime && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                inGame(anAccount->pwr,W,H,1,mapnum,anAccount);
            }
            else if(onBird && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                inGame(anAccount->pwr,W,H,2,mapnum,anAccount);
            }
            else if(onRock && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                inGame(anAccount->pwr,W,H,3,mapnum,anAccount);
            }
        }
        SDL_RenderPresent(m_renderer);
        SDL_Delay(20);
        if(SDL_PollEvent(&event))
            if(event.type == SDL_KEYDOWN)
                if(event.key.keysym.sym == SDLK_ESCAPE)
                {
                    if(SoundState)
                        Mix_PlayChannel(-1,ButtonClick,0);
                    return;
                }
    }

}
//------------------------------------------------------------------------------------------------------------------
void mapSelect(int W,int H,account* anAccount)
{
    SDL_Texture * MenuBackground = IMG_LoadTexture(m_renderer,"./Assets/Blured_background.png");
    SDL_Texture * Map1 = IMG_LoadTexture(m_renderer,"./Assets/Map1.jpg");
    SDL_Texture * Map2 = IMG_LoadTexture(m_renderer,"./Assets/Map2.jpg");
    SDL_Texture * Map3 = IMG_LoadTexture(m_renderer,"./Assets/Map3.jpg");
    SDL_Texture * WoodButton1 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton1.png");
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
    Mix_Music* MenuBackgroundSound1 = Mix_LoadMUS("./Assets/Music/The-Road-Home.mp3");
    Mix_Music* MenuBackgroundSound2 = Mix_LoadMUS("./Assets/Music/Minstrels-Song.mp3");
    Mix_Music* MenuBackgroundSound3 = Mix_LoadMUS("./Assets/Music/The-Pyre.mp3");
    Mix_Music* MenuBackgroundSound4 = Mix_LoadMUS("./Assets/Music/a-robust-crew.mp3");
    Mix_Chunk* ButtonClick = Mix_LoadWAV("./Assets/Music/ButtonSoundEffect.mp3");
    Mix_Music * BM[] = {MenuBackgroundSound1,MenuBackgroundSound2,MenuBackgroundSound3,MenuBackgroundSound4};

    float rect_w = 600, rect_h = 400, dis_btwButtons = 50 ;
    SDL_Color text_color_Bu = { 250, 250, 250};
    SDL_Color text_color_Bs = {30,30,30} ;

    while(true)
    {
        SDL_RenderClear(m_renderer);
        bool onButton1 = false , onButton2 = false, onButton3 = false , onButton4 = false;
        if(Mix_PlayingMusic()==0)
            Mix_PlayMusic(BM[rand()%4],1);
        int Mouse_x , Mouse_y ;
        SDL_GetMouseState(&Mouse_x,&Mouse_y);

        if((Mouse_x >= W/2-rect_w-dis_btwButtons/2 && Mouse_x <= W/2-dis_btwButtons/2)&&(Mouse_y <= H/2 - dis_btwButtons/2 && Mouse_y >= H/2 - dis_btwButtons/2-rect_h))
            onButton1 = true ;
        if((Mouse_x >= W/2+dis_btwButtons/2 && Mouse_x <= W/2+rect_w+dis_btwButtons/2)&&(Mouse_y <= H/2 - dis_btwButtons/2 && Mouse_y >= H/2 - dis_btwButtons/2-rect_h))
            onButton2 = true ;
        if((Mouse_x >= W/2-rect_w-dis_btwButtons/2 && Mouse_x <= W/2-dis_btwButtons/2)&&(Mouse_y >= H/2 + dis_btwButtons/2 - rect_h && Mouse_y <= H/2 + dis_btwButtons/2 + rect_h))
            onButton3 = true ;
        if((Mouse_x >= W/2+dis_btwButtons/2 && Mouse_x <= W/2+rect_w+dis_btwButtons/2)&&(Mouse_y >= H/2 + dis_btwButtons/2 && Mouse_y <= H/2 + dis_btwButtons/2 + rect_h))
            onButton4 = true ;


        ShowImage(m_renderer,MenuBackground,0,0,W,H,0);
        betterText(m_renderer,"Choose a map",W/2,30,text_color_Bu,255,40,"WarPriestCondensed-2Z8X");
        ShowImage(m_renderer,Map1,W/2-dis_btwButtons/2-rect_w,H/2-dis_btwButtons/2-rect_h,rect_w,rect_h,0);
        ShowImage(m_renderer,Map2,W/2+dis_btwButtons/2,H/2-dis_btwButtons/2-rect_h,rect_w,rect_h,0);
        ShowImage(m_renderer,Map3,W/2-dis_btwButtons/2-rect_w,H/2+dis_btwButtons/2,rect_w,rect_h,0);

        ShowImage(m_renderer,WoodButton1,W/2+dis_btwButtons/2,H/2+dis_btwButtons/2,rect_w,rect_h,0);

        if(onButton4)
            betterText(m_renderer,"Random",W/2+dis_btwButtons/2+rect_w/2,H/2+dis_btwButtons/2+rect_h/2,text_color_Bs,255,50,"WarPriestCondensed-2Z8X");
        else
            betterText(m_renderer,"Random",W/2+dis_btwButtons/2+rect_w/2,H/2+dis_btwButtons/2+rect_h/2,text_color_Bu,255,50,"WarPriestCondensed-2Z8X");


        SDL_RenderPresent(m_renderer);
        if(SDL_PollEvent(&event))
        {
            if(onButton1 && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                modeSelect(W,H,anAccount,1);
            }
            else if(onButton2 && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                modeSelect(W,H,anAccount,2);
            }
            else if(onButton3 && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                modeSelect(W,H,anAccount,3);
            }
            else if(onButton4 && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                modeSelect(W,H,anAccount,0);
            }
        }
        SDL_Delay(10);
        if(SDL_PollEvent(&event))
            if(event.type == SDL_KEYDOWN)
                if(event.key.keysym.sym == SDLK_ESCAPE)
                {
                    if(SoundState)
                        Mix_PlayChannel(-1,ButtonClick,0);
                    return;
                }
    }
}
//------------------------------------------------------------------------------------------------------------------
void HelpMenu(int W, int H)
{
    SDL_Texture * MenuBackground = IMG_LoadTexture(m_renderer,"./Assets/Blured_background.png");
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
    Mix_Music* MenuBackgroundSound1 = Mix_LoadMUS("./Assets/Music/The-Road-Home.mp3");
    Mix_Music* MenuBackgroundSound2 = Mix_LoadMUS("./Assets/Music/Minstrels-Song.mp3");
    Mix_Music* MenuBackgroundSound3 = Mix_LoadMUS("./Assets/Music/The-Pyre.mp3");
    Mix_Music* MenuBackgroundSound4 = Mix_LoadMUS("./Assets/Music/a-robust-crew.mp3");
    Mix_Chunk* ButtonClick = Mix_LoadWAV("./Assets/Music/ButtonSoundEffect.mp3");
    Mix_Music * BM[] = {MenuBackgroundSound1,MenuBackgroundSound2,MenuBackgroundSound3,MenuBackgroundSound4};

    SDL_Texture * WoodButton3 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton3.png");

    int Button_w = 600, Button_h = 200 , dis_btwButtons = 50 ;
    SDL_Color text_color_Bu = { 250, 250, 250};
    SDL_Color text_color_Bs = {30,30,30} ;
    while(true)
    {
        if(Mix_PlayingMusic()==0 && MusicState)
            Mix_PlayMusic(BM[rand()%4],1);
        SDL_RenderClear(m_renderer);

        ShowImage(m_renderer,MenuBackground,0,0,W,H,0);
        ShowImage(m_renderer,WoodButton3,W/2-1000,H/2-300,2000,600,0);
        betterText(m_renderer,"Help",W/2,30,text_color_Bu,255,50,"pythia");
        betterText(m_renderer,"Use Mouse Click (LMB or RMB) to shoot from the canon",W/2,H/2,text_color_Bu,255,50,"pythia");
        betterText(m_renderer,"Use Space to swap the balls in the canon",W/2,H/2+50,text_color_Bu,255,50,"pythia");
        betterText(m_renderer,"Use Q to use your Ultimate/PowerBar",W/2,H/2+100,text_color_Bu,255,50,"pythia");


        SDL_RenderPresent(m_renderer);
        if(SDL_PollEvent(&event))
            if(event.type == SDL_KEYDOWN)
                if(event.key.keysym.sym == SDLK_ESCAPE)
                {
                    if(SoundState)
                        Mix_PlayChannel(-1,ButtonClick,0);
                    return;
                }
        SDL_Delay(10);
    }
}
//------------------------------------------------------------------------------------------------------------------
void SettingsMenu(int W,int H)
{
   SDL_Texture * MenuBackground = IMG_LoadTexture(m_renderer,"./Assets/Blured_background.png");
    SDL_Texture * Sound = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton3.png");
    SDL_Texture * Music = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton4.png");
    SDL_Texture * Help = Sound ;
    SDL_Texture * MusicIconPressed = IMG_LoadTexture(m_renderer,"./Assets/button_music_off_unpressed.png");
    SDL_Texture * MusicIconUnpressed = IMG_LoadTexture(m_renderer,"./Assets/button_music_on_unpressed.png");
    SDL_Texture * SoundIconPressed = IMG_LoadTexture(m_renderer,"./Assets/button_volume_off_unpressed.png");
    SDL_Texture * SoundIconUnpressed = IMG_LoadTexture(m_renderer,"./Assets/button_volume_on_unpressed.png");

    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
    Mix_Music* MenuBackgroundSound1 = Mix_LoadMUS("./Assets/Music/The-Road-Home.mp3");
    Mix_Music* MenuBackgroundSound2 = Mix_LoadMUS("./Assets/Music/Minstrels-Song.mp3");
    Mix_Music* MenuBackgroundSound3 = Mix_LoadMUS("./Assets/Music/The-Pyre.mp3");
    Mix_Music* MenuBackgroundSound4 = Mix_LoadMUS("./Assets/Music/a-robust-crew.mp3");
    Mix_Chunk* ButtonClick = Mix_LoadWAV("./Assets/Music/ButtonSoundEffect.mp3");
    Mix_Music * BM[] = {MenuBackgroundSound1,MenuBackgroundSound2,MenuBackgroundSound3,MenuBackgroundSound4};

    float Button_w = 600, Button_h = 200 , dis_btwButtons = 5 ;
    SDL_Color text_color_Bu = { 250, 250, 250};
    SDL_Color text_color_Bs = {30,30,30};

    while(true)
    {
        SDL_RenderClear(m_renderer);
        if(Mix_PlayingMusic()==0 && MusicState)
            Mix_PlayMusic(BM[rand()%4],1);

        bool onSound = false , onMusic = false , onHelp = false ;

        ShowImage(m_renderer,MenuBackground,0,0,W,H,0);
        ShowImage(m_renderer,Sound,W/2-Button_w/2,H/2-1.5*Button_h-dis_btwButtons,Button_w,Button_h,0);
        ShowImage(m_renderer,Music,W/2-Button_w/2,H/2-Button_h/2,Button_w,Button_h,0);
        ShowImage(m_renderer,Help,W/2-Button_w/2,H/2+dis_btwButtons+Button_h/2,Button_w,Button_h,0);

        int Mouse_x , Mouse_y ;
        SDL_GetMouseState(&Mouse_x,&Mouse_y);

        if((Mouse_x >= W/2 - Button_w/2 && Mouse_x <= W/2 + Button_w/2)&&(Mouse_y >= H/2-1.5*Button_h-dis_btwButtons && Mouse_y <= H/2-Button_h/2-dis_btwButtons))
            onSound = true;
        if((Mouse_x >= W/2 - Button_w/2 && Mouse_x <= W/2 + Button_w/2)&&(Mouse_y >= H/2-Button_h/2 && Mouse_y <= H/2+Button_h/2))
            onMusic = true;
        if((Mouse_x >= W/2 - Button_w/2 && Mouse_x <= W/2 + Button_w/2)&&(Mouse_y >= H/2+dis_btwButtons+Button_h/2 && Mouse_y <= H/2+dis_btwButtons+Button_h*1.5))
            onHelp = true ;

        if(onSound)
            ShowImage(m_renderer,SoundIconPressed,W/2-Button_w/2,H/2-1.5*Button_h-dis_btwButtons,Button_h,Button_h,0);
        else
            ShowImage(m_renderer,SoundIconUnpressed,W/2-Button_w/2,H/2-1.5*Button_h-dis_btwButtons,Button_h,Button_h,0);
        if(onMusic)
            ShowImage(m_renderer,MusicIconPressed,W/2-Button_w/2,H/2-Button_h/2,Button_h,Button_h,0);
        else
            ShowImage(m_renderer,MusicIconUnpressed,W/2-Button_w/2,H/2-Button_h/2,Button_h,Button_h,0);
        if(onHelp)
            betterText(m_renderer,"Help",W/2,H/2+dis_btwButtons+Button_h,text_color_Bs,255,50,"pythia");
        else
            betterText(m_renderer,"Help",W/2,H/2+dis_btwButtons+Button_h,text_color_Bu,255,50,"pythia");

        if(SoundState)
            betterText(m_renderer,"On",W/2,H/2-Button_h-dis_btwButtons,text_color_Bu,255,50,"pythia");
        else
            betterText(m_renderer,"Off",W/2,H/2-Button_h-dis_btwButtons,text_color_Bu,255,50,"pythia");

        if(MusicState)
            betterText(m_renderer,"On",W/2,H/2,text_color_Bu,255,50,"pythia");
        else
            betterText(m_renderer,"Off",W/2,H/2,text_color_Bu,255,50,"pythia");


        if(SDL_PollEvent(&event))
        {
            if(event.type == SDL_KEYDOWN)
                if(event.key.keysym.sym == SDLK_ESCAPE)
                {
                    if(SoundState)
                        Mix_PlayChannel(-1,ButtonClick,0);
                    return;
                }
            if(event.type == SDL_MOUSEBUTTONDOWN && onSound)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                SoundState = !SoundState;
            }
            if(event.type == SDL_MOUSEBUTTONDOWN && onMusic)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                MusicState = !MusicState;
                if(MusicState)
                    Mix_PlayMusic(BM[rand()%4],1);
                else
                    Mix_PauseMusic();
            }
            if(event.type == SDL_MOUSEBUTTONDOWN && onHelp)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                HelpMenu(W,H);
            }
        }

        SDL_RenderPresent(m_renderer);
        SDL_Delay(20);
    }

}
//------------------------------------------------------------------------------------------------------------------
void LBoard(int W,int H)
{
   SDL_Texture * MenuBackground = IMG_LoadTexture(m_renderer,"./Assets/Blured_background.png");
    SDL_Texture * WoodButton1 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton3.png");
    SDL_Texture * WoodButton2 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton4.png");
    SDL_Texture * WoodButton3 = WoodButton1 ;
    SDL_Texture * WoodButton4 = WoodButton2 ;
    SDL_Texture * WoodButton5 = WoodButton1 ;
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
    Mix_Music* MenuBackgroundSound1 = Mix_LoadMUS("./Assets/Music/The-Road-Home.mp3");
    Mix_Music* MenuBackgroundSound2 = Mix_LoadMUS("./Assets/Music/Minstrels-Song.mp3");
    Mix_Music* MenuBackgroundSound3 = Mix_LoadMUS("./Assets/Music/The-Pyre.mp3");
    Mix_Music* MenuBackgroundSound4 = Mix_LoadMUS("./Assets/Music/a-robust-crew.mp3");
    Mix_Chunk* ButtonClick = Mix_LoadWAV("./Assets/Music/ButtonSoundEffect.mp3");
    Mix_Music * BM[] = {MenuBackgroundSound1,MenuBackgroundSound2,MenuBackgroundSound3,MenuBackgroundSound4};

    float Button_w = 600, Button_h = 200 , dis_btwButtons = 5 ;
    SDL_Color text_color_Bu = { 250, 250, 250};
    SDL_Color text_color_R = {255,0,0};

    while(true)
    {
        SDL_RenderClear(m_renderer);
        if(Mix_PlayingMusic()==0 && MusicState)
            Mix_PlayMusic(BM[rand()%4],1);

        ShowImage(m_renderer,MenuBackground,0,0,W,H,0);

        ShowImage(m_renderer,WoodButton1,W/2-Button_w/2,H/2-dis_btwButtons*2-Button_h*2.5,Button_w,Button_h,0);
        ShowImage(m_renderer,WoodButton2,W/2-Button_w/2,H/2-dis_btwButtons-Button_h*1.5,Button_w,Button_h,0);
        ShowImage(m_renderer,WoodButton3,W/2-Button_w/2,H/2-Button_h/2,Button_w,Button_h,0);
        ShowImage(m_renderer,WoodButton4,W/2-Button_w/2,H/2+dis_btwButtons+Button_h/2,Button_w,Button_h,0);
        ShowImage(m_renderer,WoodButton5,W/2-Button_w/2,H/2+dis_btwButtons*2+Button_h*1.5,Button_w,Button_h,0);

        betterText(m_renderer,"LEADERBOARD",W/2,30,text_color_Bu,255,40,"pythia");
        sort(accounts.begin(),accounts.end(),cmp);

        betterText(m_renderer,accounts[0].username,W/2 - Button_w/4 ,H/2-dis_btwButtons*2-Button_h*2,text_color_Bu,255,50,"pythia");
        betterText(m_renderer,to_string(accounts[0].highScore),W/2 + Button_w/4 ,H/2-dis_btwButtons*2-Button_h*2,text_color_Bu,255,50,"pythia");

        if(accounts.size()>1)
        {
            betterText(m_renderer,accounts[1].username,W/2 - Button_w/4 ,H/2-dis_btwButtons-Button_h,text_color_Bu,255,50,"pythia");
            betterText(m_renderer,to_string(accounts[1].highScore),W/2 + Button_w/4 ,H/2-dis_btwButtons-Button_h,text_color_Bu,255,50,"pythia");
        }
        if(accounts.size()>2)
        {
            betterText(m_renderer,accounts[2].username,W/2 - Button_w/4 ,H/2,text_color_Bu,255,50,"pythia");
            betterText(m_renderer,to_string(accounts[2].highScore),W/2 + Button_w/4 ,H/2,text_color_Bu,255,50,"pythia");
        }
        if(accounts.size()>3)
        {
            betterText(m_renderer,accounts[3].username,W/2 - Button_w/4 ,H/2+dis_btwButtons+Button_h,text_color_Bu,255,50,"pythia");
            betterText(m_renderer,to_string(accounts[3].highScore),W/2 + Button_w/4 ,H/2+dis_btwButtons+Button_h,text_color_Bu,255,50,"pythia");
        }
        if(accounts.size()>4)
        {
            betterText(m_renderer,accounts[4].username,W/2 - Button_w/4 ,H/2+dis_btwButtons*2+Button_h*2,text_color_Bu,255,50,"pythia");
            betterText(m_renderer,to_string(accounts[4].highScore),W/2 + Button_w/4 ,H/2+dis_btwButtons*2+Button_h*2,text_color_Bu,255,50,"pythia");
        }


        if(SDL_PollEvent(&event))
            if(event.type == SDL_KEYDOWN)
                if(event.key.keysym.sym == SDLK_ESCAPE)
                {
                    if(SoundState)
                        Mix_PlayChannel(-1,ButtonClick,0);
                    return;
                }

        SDL_RenderPresent(m_renderer);
        SDL_Delay(20);
    }


}
//------------------------------------------------------------------------------------------------------------------
void GameMenu(int W,int H,account* anAccount)
{
    SDL_Texture * MenuBackground = IMG_LoadTexture(m_renderer,"./Assets/Blured_background.png");
    SDL_Texture * Start = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton1.png");
    SDL_Texture * Settings = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton2.png");
    SDL_Texture * LeaderBoard = Start ;
    SDL_Texture * Exit = Settings ;

    float Button_w = 600, Button_h = 200 , dis_btwButtons = 10 ;
    SDL_Color text_color_Bu = { 250, 250, 250};
    SDL_Color text_color_Bs = {30,30,30};
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
    Mix_Music* MenuBackgroundSound1 = Mix_LoadMUS("./Assets/Music/The-Road-Home.mp3");
    Mix_Music* MenuBackgroundSound2 = Mix_LoadMUS("./Assets/Music/Minstrels-Song.mp3");
    Mix_Music* MenuBackgroundSound3 = Mix_LoadMUS("./Assets/Music/The-Pyre.mp3");
    Mix_Music* MenuBackgroundSound4 = Mix_LoadMUS("./Assets/Music/a-robust-crew.mp3");
    Mix_Chunk* ButtonClick = Mix_LoadWAV("./Assets/Music/ButtonSoundEffect.mp3");
    Mix_Music * BM[] = {MenuBackgroundSound1,MenuBackgroundSound2,MenuBackgroundSound3,MenuBackgroundSound4};

    while(true)
    {
        SDL_RenderClear(m_renderer);
        bool onStart = false , onSettings = false , onLeaderboard = false , onExit = false ;
        if(Mix_PlayingMusic()==0 && MusicState)
            Mix_PlayMusic(BM[rand()%4],1);

        ShowImage(m_renderer,MenuBackground,0,0,W,H,0);
        ShowImage(m_renderer,Start,W/2-Button_w/2,H/2-dis_btwButtons*1.5-Button_h*2,Button_w,Button_h,0);
        ShowImage(m_renderer,Settings,W/2-Button_w/2,H/2-dis_btwButtons/2-Button_h,Button_w,Button_h,0);
        ShowImage(m_renderer,LeaderBoard,W/2-Button_w/2,H/2+dis_btwButtons/2,Button_w,Button_h,0);
        ShowImage(m_renderer,Exit,W/2-Button_w/2,H/2+dis_btwButtons*1.5+Button_h,Button_w,Button_h,0);

        int Mouse_x , Mouse_y ;
        SDL_GetMouseState(&Mouse_x,&Mouse_y);

        if((Mouse_x >= W/2-Button_w/2 && Mouse_x <= W/2+Button_w/2)&&(Mouse_y >= H/2 - dis_btwButtons*1.5- 2*Button_h && Mouse_y <= H/2 - 1.5*dis_btwButtons-Button_h))
            onStart = true ;
        if((Mouse_x >= W/2-Button_w/2 && Mouse_x <= W/2+Button_w/2)&&(Mouse_y >= H/2 - dis_btwButtons/2 - Button_h && Mouse_y <= H/2 - dis_btwButtons/2))
            onSettings = true ;
        if((Mouse_x >= W/2-Button_w/2 && Mouse_x <= W/2+Button_w/2)&&(Mouse_y >= H/2 + dis_btwButtons/2 && Mouse_y <=H/2 + dis_btwButtons/2 +Button_h))
            onLeaderboard = true ;
        if((Mouse_x >= W/2-Button_w/2 && Mouse_x <= W/2+Button_w/2)&&(Mouse_y >= H/2 + dis_btwButtons*1.5+Button_h && Mouse_y <= H/2 + dis_btwButtons*1.5 + 2*Button_h))
            onExit = true ;

        if(onStart)
            betterText(m_renderer,"Start",W/2,H/2-1.5*dis_btwButtons-1.5*Button_h,text_color_Bs,255,50,"pythia");
        else
            betterText(m_renderer,"Start",W/2,H/2-1.5*dis_btwButtons-1.5*Button_h,text_color_Bu,255,50,"pythia");
        if(onSettings)
            betterText(m_renderer,"Settings",W/2,H/2 - dis_btwButtons/2-Button_h/2-30,text_color_Bs,255,50,"pythia");
        else
            betterText(m_renderer,"Settings",W/2,H/2 - dis_btwButtons/2-Button_h/2-30,text_color_Bu,255,50,"pythia");
        if(onLeaderboard)
            betterText(m_renderer,"LeaderBoard",W/2,H/2 + dis_btwButtons/2 +Button_h/2,text_color_Bs,255,50,"pythia");
        else
            betterText(m_renderer,"LeaderBoard",W/2,H/2 + dis_btwButtons/2 +Button_h/2,text_color_Bu,255,50,"pythia");
        if(onExit)
            betterText(m_renderer,"Exit",W/2,H/2 + dis_btwButtons*1.5 + Button_h*1.5-30,text_color_Bs,255,50,"pythia");
        else
            betterText(m_renderer,"Exit",W/2,H/2 + dis_btwButtons*1.5 + Button_h*1.5-30,text_color_Bu,255,50,"pythia");

         if(SDL_PollEvent(&event))
        {
            if(onStart && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                mapSelect(W,H,anAccount);
            }
            else if(onSettings && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                SettingsMenu(W,H);
            }
            else if(onLeaderboard && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                LBoard(W,H);
            }
            else if(onExit && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                 if(AreYouSureMenu(W,H))
                 {
                    SDL_DestroyWindow( m_window );
                    SDL_DestroyRenderer( m_renderer );
                    IMG_Quit();
                    SDL_Quit();
                 }
                else
                    continue;
            }
        }
        SDL_RenderPresent(m_renderer);
        SDL_Delay(20);
    }

}
//------------------------------------------------------------------------------------------------------------------
void LoginMenu(int W,int H)
{
   SDL_Texture * MenuBackground = IMG_LoadTexture(m_renderer,"./Assets/Blured_background.png");
    SDL_Texture * WoodButton1 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton4.png");
    SDL_Texture * WoodButton2 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton4.png");
    string UserName = "" , Password = "" ;
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
    Mix_Music* MenuBackgroundSound1 = Mix_LoadMUS("./Assets/Music/The-Road-Home.mp3");
    Mix_Music* MenuBackgroundSound2 = Mix_LoadMUS("./Assets/Music/Minstrels-Song.mp3");
    Mix_Music* MenuBackgroundSound3 = Mix_LoadMUS("./Assets/Music/The-Pyre.mp3");
    Mix_Music* MenuBackgroundSound4 = Mix_LoadMUS("./Assets/Music/a-robust-crew.mp3");
    Mix_Chunk* ButtonClick = Mix_LoadWAV("./Assets/Music/ButtonSoundEffect.mp3");
    Mix_Music * BM[] = {MenuBackgroundSound1,MenuBackgroundSound2,MenuBackgroundSound3,MenuBackgroundSound4};

    int Button_w = 600, Button_h = 200 , dis_btwButtons = 5 ;
    SDL_Color text_color_Bu = { 250, 250, 250};
    SDL_Color text_color_R = {255,0,0};

    SDL_Texture* registerMenuTxt = SDL_CreateTexture(m_renderer,SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET,W,H);
    SDL_SetRenderTarget(m_renderer,registerMenuTxt);

    ShowImage(m_renderer,MenuBackground,0,0,W,H,0);
    ShowImage(m_renderer,WoodButton1,W/2-Button_w/2,H/2 - dis_btwButtons/2 - Button_h,Button_w,Button_h,0);
    ShowImage(m_renderer,WoodButton2,W/2-Button_w/2,H/2 + dis_btwButtons/2,Button_w,Button_h,0);
    betterText(m_renderer,"Enter USERNAME: ",W/2,H/2 - dis_btwButtons/2 - Button_h/2 - 30 ,text_color_Bu,255,25,"pythia");
    betterText(m_renderer,"Enter Password: ",W/2,H/2 + dis_btwButtons/2 + Button_h/2 - 30 ,text_color_Bu,255,25,"pythia");

    SDL_SetTextureBlendMode(registerMenuTxt, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(m_renderer, NULL);
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

    bool flag0 = false , flag1 = false , flag2 = false , flag3 = false ;

    while(!flag3)
    {
        if(Mix_PlayingMusic()==0 && MusicState)
            Mix_PlayMusic(BM[rand()%4],1);
        SDL_RenderClear(m_renderer);
        bool onButton1 = false , onButton2 = false;
        SDL_RenderCopy(m_renderer,registerMenuTxt,NULL,NULL);

        int Mouse_x , Mouse_y ;
        SDL_GetMouseState(&Mouse_x,&Mouse_y);

        if((Mouse_x >= W/2-Button_w/2 && Mouse_x <= W/2+Button_w/2)&&(Mouse_y >= H/2 - dis_btwButtons/2 - Button_h && Mouse_y <= H/2 - dis_btwButtons/2))
            onButton1 = true ;
        if((Mouse_x >= W/2-Button_w/2 && Mouse_x <= W/2+Button_w/2)&&(Mouse_y >= H/2 + dis_btwButtons/2 && Mouse_y <= H/2 + dis_btwButtons/2 + Button_h))
            onButton2 = true ;

        if(flag2)
            betterText(m_renderer,"THE ACCOUNT DOESN'T EXIST",W/2,H/2 ,text_color_R,255,25,"pythia");


        SDL_RenderPresent(m_renderer);
        if(SDL_PollEvent(&event))
        {
            if(onButton1 && event.type == SDL_MOUSEBUTTONDOWN)
                UserName = TextBox(W/2,H/2-dis_btwButtons/2 - Button_h/2 ,registerMenuTxt);
            if(onButton2 && event.type == SDL_MOUSEBUTTONDOWN)
                Password = PasswordTextBox(W/2,H/2 + dis_btwButtons/2 + Button_h/2 ,registerMenuTxt);
        }

        if(UserName != "" && Password != "")
            flag0 = true;
        SDL_Delay(10);

        if(flag0)
        {
            flag1 = false; flag2 = false;
            switch(checking_user_and_password_of_input(UserName , Password))
            {
                case 2:
                    //true situation
                    for(int i = 0 ; i < accounts.size()  ; i++)
                    {
                        if(accounts[i].username == UserName)
                        {
                            GameMenu(W,H,&accounts[i]);
                            flag3 = true ;
                            break;
                        }
                    }
                    break;
                case 1:
                    //false password situation
                    flag2 = true ;
                    break;

                case 0:
                    //account doesn't exist situation
                    flag2 = true ;
                    break;
            }
        }
        if(event.type == SDL_KEYDOWN)
            if(event.key.keysym.sym == SDLK_ESCAPE)
                return ;
    }
}
//------------------------------------------------------------------------------------------------------------------
void RegisterMenu(int W, int H)
{
    SDL_Texture * MenuBackground = IMG_LoadTexture(m_renderer,"./Assets/Blured_background.png");
    SDL_Texture * WoodButton1 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton4.png");
    SDL_Texture * WoodButton2 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton4.png");
    string UserName = "" , Password = "" ;
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
    Mix_Music* MenuBackgroundSound1 = Mix_LoadMUS("./Assets/Music/The-Road-Home.mp3");
    Mix_Music* MenuBackgroundSound2 = Mix_LoadMUS("./Assets/Music/Minstrels-Song.mp3");
    Mix_Music* MenuBackgroundSound3 = Mix_LoadMUS("./Assets/Music/The-Pyre.mp3");
    Mix_Music* MenuBackgroundSound4 = Mix_LoadMUS("./Assets/Music/a-robust-crew.mp3");
    Mix_Chunk* ButtonClick = Mix_LoadWAV("./Assets/Music/ButtonSoundEffect.mp3");
    Mix_Music * BM[] = {MenuBackgroundSound1,MenuBackgroundSound2,MenuBackgroundSound3,MenuBackgroundSound4};

    int Button_w = 600, Button_h = 200 , dis_btwButtons = 5 ;
    SDL_Color text_color_Bu = { 250, 250, 250};

    SDL_Texture* registerMenuTxt = SDL_CreateTexture(m_renderer,SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_TARGET,W,H);
    SDL_SetRenderTarget(m_renderer,registerMenuTxt);

    ShowImage(m_renderer,MenuBackground,0,0,W,H,0);
    ShowImage(m_renderer,WoodButton1,W/2-Button_w/2,H/2 - dis_btwButtons/2 - Button_h,Button_w,Button_h,0);
    ShowImage(m_renderer,WoodButton2,W/2-Button_w/2,H/2 + dis_btwButtons/2,Button_w,Button_h,0);
    betterText(m_renderer,"Enter Preferred USERNAME: ",W/2,H/2 - dis_btwButtons/2 - Button_h/2 - 30 ,text_color_Bu,255,25,"pythia");
    betterText(m_renderer,"Enter Preferred Password: ",W/2,H/2 + dis_btwButtons/2 + Button_h/2 - 30 ,text_color_Bu,255,25,"pythia");

    SDL_SetTextureBlendMode(registerMenuTxt, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(m_renderer, NULL);
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);


    while(true)
    {
        SDL_RenderClear(m_renderer);
        if(Mix_PlayingMusic()==0 && MusicState)
            Mix_PlayMusic(BM[rand()%4],1);
        bool onButton1 = false , onButton2 = false;
        SDL_RenderCopy(m_renderer,registerMenuTxt,NULL,NULL);

        int Mouse_x , Mouse_y ;
        SDL_GetMouseState(&Mouse_x,&Mouse_y);

        if((Mouse_x >= W/2-Button_w/2 && Mouse_x <= W/2+Button_w/2)&&(Mouse_y >= H/2 - dis_btwButtons/2 - Button_h && Mouse_y <= H/2 - dis_btwButtons/2))
            onButton1 = true ;
        if((Mouse_x >= W/2-Button_w/2 && Mouse_x <= W/2+Button_w/2)&&(Mouse_y >= H/2 + dis_btwButtons/2 && Mouse_y <= H/2 + dis_btwButtons/2 + Button_h))
            onButton2 = true ;


        SDL_RenderPresent(m_renderer);
        if(SDL_PollEvent(&event))
        {
            if(onButton1 && event.type == SDL_MOUSEBUTTONDOWN)
                UserName = TextBox(W/2,H/2-dis_btwButtons/2 - Button_h/2 ,registerMenuTxt);
            if(onButton2 && event.type == SDL_MOUSEBUTTONDOWN)
                Password = PasswordTextBox(W/2,H/2 + dis_btwButtons/2 + Button_h/2 ,registerMenuTxt);
            if(event.type == SDL_KEYDOWN)
                if(event.key.keysym.sym == SDLK_ESCAPE)
                    return ;
        }

        if(UserName != "" && Password != "")
            break;
        SDL_Delay(10);
    }
    if(checking_user_and_password_of_input(UserName , Password) == 0)
    {
        add_new_account_to_vector(UserName , Password) ;
        update_Txt_file();
    }
}
//------------------------------------------------------------------------------------------------------------------
void inMenu(int W, int H)
{
    SDL_Texture * MenuBackground = IMG_LoadTexture(m_renderer,"./Assets/Blured_background.png");
    SDL_Texture * WoodButton1 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton1.png");
    SDL_Texture * WoodButton2 = IMG_LoadTexture(m_renderer,"./Assets/WoodenButton2.png");
    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048);
    Mix_Music* MenuBackgroundSound1 = Mix_LoadMUS("./Assets/Music/The-Road-Home.mp3");
    Mix_Music* MenuBackgroundSound2 = Mix_LoadMUS("./Assets/Music/Minstrels-Song.mp3");
    Mix_Music* MenuBackgroundSound3 = Mix_LoadMUS("./Assets/Music/The-Pyre.mp3");
    Mix_Music* MenuBackgroundSound4 = Mix_LoadMUS("./Assets/Music/a-robust-crew.mp3");
    Mix_Chunk* ButtonClick = Mix_LoadWAV("./Assets/Music/ButtonSoundEffect.mp3");
    Mix_Music * BM[] = {MenuBackgroundSound1,MenuBackgroundSound2,MenuBackgroundSound3,MenuBackgroundSound4};

    //Mix_Chunk * ButtonSelectEffect = Mix_LoadWAV()

    int Button_w = 600, Button_h = 200 , dis_btwButtons = 50 ;
    SDL_Color text_color_Bu = { 250, 250, 250};
    SDL_Color text_color_Bs = {30,30,30} ;
    while(true)
    {
        if(Mix_PlayingMusic()==0 && MusicState)
            Mix_PlayMusic(BM[rand()%4],1);

        SDL_RenderClear(m_renderer);
        bool onButton1 = false , onButton2 = false;

        int Mouse_x , Mouse_y ;
        SDL_GetMouseState(&Mouse_x,&Mouse_y);

        if((Mouse_x >= W/2-Button_w/2 && Mouse_x <= W/2+Button_w/2)&&(Mouse_y >= H/2 - dis_btwButtons/2 - Button_h && Mouse_y <= H/2 - dis_btwButtons/2))
            onButton1 = true ;
        if((Mouse_x >= W/2-Button_w/2 && Mouse_x <= W/2+Button_w/2)&&(Mouse_y >= H/2 + dis_btwButtons/2 && Mouse_y <= H/2 + dis_btwButtons/2 + Button_h))
            onButton2 = true ;

        ShowImage(m_renderer,MenuBackground,0,0,W,H,0);

        ShowImage(m_renderer,WoodButton1,W/2-Button_w/2,H/2 - dis_btwButtons/2 - Button_h,Button_w,Button_h,0);
        ShowImage(m_renderer,WoodButton2,W/2-Button_w/2,H/2 + dis_btwButtons/2,Button_w,Button_h,0);

        if(onButton1)
            betterText(m_renderer,"LOGIN",W/2,H/2 - dis_btwButtons/2 - Button_h/2 - 15 ,text_color_Bs,255,50,"WarPriestCondensed-2Z8X");
        else
            betterText(m_renderer,"LOGIN",W/2,H/2 - dis_btwButtons/2 - Button_h/2 -15 ,text_color_Bu,255,50,"WarPriestCondensed-2Z8X");

        if(onButton2)
            betterText(m_renderer,"REGISTER",W/2,H/2 + dis_btwButtons/2 + Button_h/2 - 30 ,text_color_Bs,255,50,"WarPriestCondensed-2Z8X");
        else
            betterText(m_renderer,"REGISTER",W/2,H/2 + dis_btwButtons/2 + Button_h/2 - 30 ,text_color_Bu ,255,50,"WarPriestCondensed-2Z8X");

        SDL_RenderPresent(m_renderer);
        if(SDL_PollEvent(&event))
        {
            if(onButton1 && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                LoginMenu(W,H);
            }
            else if(onButton2 && event.type == SDL_MOUSEBUTTONDOWN)
            {
                if(SoundState)
                    Mix_PlayChannel(-1,ButtonClick,0);
                RegisterMenu(W,H);
            }
            else if(event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.sym == SDLK_ESCAPE)
                {
                    if(SoundState)
                        Mix_PlayChannel(-1,ButtonClick,0);
                    if(AreYouSureMenu(W,H))
                        return;
                    else
                        continue;
                }
            }
        }
        SDL_Delay(10);
    }
}

#include "wolf.h"

int         get_map(t_mlx *stuff, int nb)
{
    int     ret = 0;
    if (stuff->map.matrix)
    {
        free(stuff->map.matrix);
        stuff->map.matrix = NULL;
    }
    std::string files[1] = { "wolf3d_maps/good.map" };
    if (nb > -1 && nb < 1)
        ret = create_map(stuff, files[nb]);
    if (ret != 1)
        return (0);
    return (1);
}

t_RGB color_converter (int hexValue)
{
    t_RGB rgbColor;
    rgbColor.r = hexValue >> 16;//((hexValue >> 16) & 0xFF) / 255.0; // Extract the RR byte
    rgbColor.g = (hexValue & 0x00ff00) >> 8;//((hexValue >> 8) & 0xFF) / 255.0; // Extract the GG byte
    rgbColor.b = (hexValue & 0x0000ff);//((hexValue) & 0xFF) / 255.0; // Extract the BB byte
    return (rgbColor); 
}

/*
void draw_line(t_mlx *stuff, SDL_Renderer *renderer, int x1, int y1, int x2, int y2)
{
    t_RGB c = color_converter(0);
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
    //SDL_RenderDrawLine(renderer, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}
*/

int main(int argc, char *argv[])
{
    SDL_Event       event;
    SDL_Window      *window;
    SDL_Renderer    *renderer;
    t_mlx           stuff;

    int done = 0, w = 1280, h = 720;
    // mandatory at least on switch, else gfx is not properly closed
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        SDL_Log("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    // create an SDL window (OpenGL ES2 always enabled)
    // when SDL_FULLSCREEN flag is not set, viewport is automatically handled by SDL (use SDL_SetWindowSize to "change resolution")
    // available switch SDL2 video modes :
    // 1920 x 1080 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
    // 1280 x 720 @ 32 bpp (SDL_PIXELFORMAT_RGBA8888)
    window = SDL_CreateWindow("sdl2_gles2", 0, 0, 1280, 720, 0);
    if (!window) {
        SDL_Log("SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // create a renderer (OpenGL ES2)
    renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // open CONTROLLER_PLAYER_1 and CONTROLLER_PLAYER_2
    // when railed, both joycons are mapped to joystick #0,
    // else joycons are individually mapped to joystick #0, joystick #1, ...
    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L45
    for (int i = 0; i < 2; i++) {
        if (SDL_JoystickOpen(i) == NULL) {
            SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
            SDL_Quit();
            return -1;
        }
    }
    int chosen_map = 0;
    stuff.h = h;
    stuff.w = w;
    stuff.renderer = renderer;
    stuff.map.matrix = NULL;
    stuff.player.cam.x = 0.378560f;
	stuff.player.cam.y = -0.540640f;
	stuff.player.dir.x = 0.819152f;
	stuff.player.dir.y = 0.573576f;
	stuff.player.movespeed = 0.1f;
    if (!(get_map(&stuff, chosen_map)))
        done = 1;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_JOYAXISMOTION:
                    SDL_Log("Joystick %d axis %d value: %d\n",
                            event.jaxis.which,
                            event.jaxis.axis, event.jaxis.value);
                    if( event.jaxis.which == 0 )
                    {
                        const int JOYSTICK_DEAD_ZONE = 8000;
                        //X axis motion
                        if( event.jaxis.axis == 0 )
                        {
                            //Left of dead zone
                            if( event.jaxis.value < -JOYSTICK_DEAD_ZONE )
                            {
                                rotate_player(5.0f / 180.0f * M_PI, &stuff);
                            }
                            //Right of dead zone
                            else if( event.jaxis.value > JOYSTICK_DEAD_ZONE )
                            {
                                rotate_player(-5.0f / 180.0f * M_PI, &stuff);
                            }
                        }
                        else if( event.jaxis.axis == 1 )
                        {
                                //Below of dead zone
                                if( event.jaxis.value < -JOYSTICK_DEAD_ZONE )
                                {
                                    move_player(&stuff, -stuff.player.movespeed);
                                }
                                //Above of dead zone
                                else if( event.jaxis.value > JOYSTICK_DEAD_ZONE )
                                {
                                    move_player(&stuff, stuff.player.movespeed);
                                }
                        }
                    }

                    break;

                case SDL_JOYBUTTONDOWN:
                    SDL_Log("Joystick %d button %d down\n",
                            event.jbutton.which, event.jbutton.button);
                    // https://github.com/devkitPro/SDL/blob/switch-sdl2/src/joystick/switch/SDL_sysjoystick.c#L52
                    // seek for joystick #0
                    if (event.jbutton.which == 0) {
                        if (event.jbutton.button == 0) {
                            // (A) button down, switch resolution
                            if(w == 1920) {
                                SDL_SetWindowSize(window, 1280, 720);
                            }
                            else {
                                SDL_SetWindowSize(window, 1920, 1080);
                            }
                            stuff.h = h;
                            stuff.w = w;
                        }
                        else if (event.jbutton.button == 10) {
                            // (+) button down
                            done = 1;
                        }
                        else if (event.jbutton.button == 11) {
                            // (-) button down
                            chosen_map = (chosen_map + 1 >= MAP_NB) ? 0 : chosen_map + 1;
                            if (!(get_map(&stuff, chosen_map)))
                            {
                                if (stuff.map.matrix)
                                    free(stuff.map.matrix);
                                SDL_DestroyRenderer(renderer);
                                SDL_DestroyWindow(window);
                                SDL_Quit();
                                exit(0);
                            }
                        }
                    }
                    break;

                default:
                    break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // fill window bounds
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_GetWindowSize(window, &w, &h);
        SDL_Rect f = {0, 0, w, h};
        SDL_RenderFillRect(renderer, &f);
        wolf_draw(&stuff);
        SDL_RenderPresent(renderer);
    }
    if (stuff.map.matrix)
        free(stuff.map.matrix);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

#include "wolf.h"

int		colors(int i)
{
	static int rgb[62] = { RED, ORANGE, LIME, BLUE, YELLOW, CYAN, MAGENTA,
	SILVER, GRAY, MAROON, OLIVE, GREEN, PURPLE, TEAL, NAVY, CRIMSON, CORAL,
	INDIAN_RED, SALMON, ORANGE_RED, GOLD, GOLDEN_ROD, SADDLEBROWN, LAWN_GREEN,
	DARK_GREEN, FOREST_GREEN, PALE_GREEN, SPRING_GREEN, SEA_GREEN,
	LIGHT_SEA_GREEN, DARK_SLATE_GRAY, POWDER_BLUE, AQUA_MARINE, STEEL_BLUE,
	SKY_BLUE, MIDNIGHT_BLUE, INDIGO, DARK_MAGENTA, DEEP_PINK, HOT_PINK, ORCHID,
	BEIGE, ANTIQUE_WHITE, WHEAT, CORN_SILK, LAVENDER, FLORAL_WHITE, ALICE_BLUE,
	GHOST_WHITE, HONEYDEW, IVORY, AZURE, SNOW, MISTY_ROSE, DARK_GRAY, GAINSBORO,
	WHITE_SMOKE, DISCO, BRICK_RED, FLAMINGO, JAFFA, SAFFRON };

	return (rgb[i % 62]);
}

uint32_t    make_color(int hexValue)
{
    uint32_t ret;
    ret = hexValue >> 16; // r
    ret <<= 16;
    ret |= (hexValue & 0x00ff00) >> 8; // g
    ret <<= 16;
    ret |= (hexValue & 0x0000ff); //b
    ret |= 255; // a
    /*
    rgbColor.r = hexValue >> 16;//((hexValue >> 16) & 0xFF) / 255.0; // Extract the RR byte
    rgbColor.g = (hexValue & 0x00ff00) >> 8;//((hexValue >> 8) & 0xFF) / 255.0; // Extract the GG byte
    rgbColor.b = (hexValue & 0x0000ff);//((hexValue) & 0xFF) / 255.0; // Extract the BB byte
    */
    return (ret);
}

void	dda(t_mlx *mlx, t_vect_3 *line, int x, t_ray *ray)
{
	int	y;

	y = line->x;
	while (y < line->y)
	{
        /*
		if (ray->texture)
		{
			ray->tex_pos.y = ((y - WIN_HEIGHT * 0.5f + ray->height * 0.5f) *
				ray->texture->height) / ray->height;
			image_set_pixel(mlx->image, x, y, texture_pixel(ray->tex_pos.x,
				ray->tex_pos.y, ray));
		}
		else
        */
        if (x < mlx->w && x >= 0 && y < mlx->h && y >= 0)
		{
			mlx->pixels[mlx->w * y + x] = make_color(colors(ray->hit));
			//mlx->pixels[mlx->w * y + x] = colors(ray->hit);
            //t_RGB c = color_converter(colors(ray->hit));
            //SDL_SetRenderDrawColor(mlx->renderer, c.r, c.g, c.b, 255);
            //SDL_RenderDrawPoint(mlx->renderer, x, y);
        }
        y++;
	}
	//SDL_SetRenderDrawColor(mlx->renderer, 0, 0, 0, 255);
}

void	draw_column(int x, t_mlx *mlx, t_ray *ray)
{
	t_vect_3	line;
	int			start;
	int			end;

	ray->height = (int)floor(mlx->h / ray->dist);
	start = (mlx->h - ray->height) / 2;
	if (start < 0)
		start = 0;
	end = start + ray->height;
	if (end > mlx->h - 1)
		end = mlx->h - 1;
	line.x = start;
	line.y = end;
	dda(mlx, &line, x, ray);
}

void				step_ray(t_ray *ray, t_player *p, t_mlx *mlx)
{
	ray->hit = 0;
	while (ray->hit == 0 || ray->hit == 6 || ray->hit == 7)
	{
		if (ray->sidex < ray->sidey)
		{
			ray->sidex += ray->deltax;
			ray->mx += ray->stepx;
			ray->side = 0;
		}
		else
		{
			ray->sidey += ray->deltay;
			ray->my += ray->stepy;
			ray->side = 1;
		}
		ray->hit = *(mlx->map.matrix + (mlx->map.width *
			ray->my + ray->mx));
	}
	ray->dist = ray->side == 0 ? (ray->mx - p->x + (1.0 - ray->stepx) / 2.0)
	/ ray->dirx : (ray->my - p->y + (1.0 - ray->stepy) / 2.0) / ray->diry;
	ray->wall = (ray->side ? p->x + ray->dist * ray->dirx :
		p->y + ray->dist * ray->diry);
	ray->texture = NULL;//USE_TEX && ray->hit && ray->hit < 6
	//	? mlx->tex[ray->hit] : NULL;
}

void				cast(int col, t_mlx *mlx)
{
	t_ray		ray;
	t_player	*p;
	double		camera;

	p = &mlx->player;
	ray.mx = p->x;
	ray.my = p->y;
	camera = 2.0f * col / (double)mlx->w - 1.0f;
	ray.dirx = mlx->player.dir.x + p->cam.x * camera;
	ray.diry = mlx->player.dir.y + p->cam.y * camera;
	ray.deltax = sqrt((ray.diry * ray.diry) / (ray.dirx * ray.dirx) + 1);
	ray.deltay = sqrt((ray.dirx * ray.dirx) / (ray.diry * ray.diry) + 1);
	ray.stepx = (ray.dirx < 0 ? -1 : 1);
	ray.stepy = (ray.diry < 0 ? -1 : 1);
	ray.sidex = ray.dirx < 0 ? (p->x - ray.mx) * ray.deltax :
		(ray.mx + 1.0f - p->x) * ray.deltax;
	ray.sidey = ray.diry < 0 ? (p->y - ray.my) * ray.deltay :
		(ray.my + 1.0f - p->y) * ray.deltay;
	step_ray(&ray, p, mlx);
	ray.wall -= floor(ray.wall);
	//if (ray.texture)
	//	ray.tex_pos.x = (int)(ray.wall * ray.texture->width);
	draw_column(col, mlx, &ray);
}

int         wolf_draw(t_mlx *stuff)
{
    for (int x = 0; x < stuff->w; x++)
    {
        cast(x, stuff);
    }
	return (1);
}
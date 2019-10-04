/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wolf.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: calamber <calamber@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/07/15 17:44:17 by calamber          #+#    #+#             */
/*   Updated: 2019/10/04 04:17:11 by calamber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WOLF_H
# define WOLF_H


# include "colors.h"

# include <math.h>
# include <fcntl.h>
# include <stdlib.h>
# include <stdbool.h>

#include <SDL.h>
# include <limits.h>
# include <string.h>
# include <stdio.h>
#include <fstream>
#include <iostream>

#define MAP_NB 1
# define USE_TEX 0
# define BOUNDS 1
# define FT_MIN(A, B) (((A) < (B)) ? (A) : (B))
# define TEXTURE_NB 8

typedef struct				s_mlx t_mlx;

typedef struct s_RGB
{
        double r;
        double g;
        double b;
}				t_RGB;

typedef struct				s_map
{
	int	width;
	int	height;
	int	*matrix;
}							t_map;

typedef struct				s_vect_2
{
	int x;
	int y;
}							t_vect_2;

typedef struct				s_vect_3
{
	double					x;
	double					y;
	double					z;
}							t_vect_3;

typedef struct				s_image
{
	void					*image;
	char					*ptr;
	int						bpp;
	int						stride;
	int						width;
	int						height;
	int						endian;
}							t_image;

typedef struct				s_ray
{
	int						hit;
	double					dirx;
	double					diry;
	double					stepx;
	double					stepy;
	double					sidex;
	double					sidey;
	int						mx;
	int						my;
	double					deltax;
	double					deltay;
	double					dist;
	double					wall;
	int						height;
	int						side;
	t_image					*texture;
	t_vect_3				tex_pos;
}							t_ray;

typedef struct				s_player
{
	double					x;
	double					y;
	t_vect_3				dir;
	t_vect_3				cam;
	double					movespeed;
}							t_player;

struct				s_mlx
{
	int						w;
	int						h;
	t_image					*tex[TEXTURE_NB];
	uint32_t				*pixels;
	SDL_Renderer			*renderer;
	t_player				player;
	t_map					map;
};

t_RGB color_converter (int hexValue);
void			move_player(t_mlx *mlx, double amount);
int			create_map(t_mlx *stuff, std::string name);
void			rotate_player(double angle, t_mlx *mlx);
#endif

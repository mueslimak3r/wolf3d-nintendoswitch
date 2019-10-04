#include "wolf.h"

static void			rotate_v(double angle, t_vect_3 *v)
{
	double x;
	double c;
	double s;

	x = v->x;
	c = cos(angle);
	s = sin(angle);
	v->x = v->x * c - v->y * s;
	v->y = x * s + v->y * c;
}

static void			rotate_player(double angle, t_mlx *mlx)
{
	rotate_v(angle, &mlx->player.dir);
	rotate_v(angle, &mlx->player.cam);
}

void			move_player(t_mlx *mlx, double amount)
{
	t_vect_3	*pdir;
	t_vect_3	n;

	pdir = &mlx->player.dir;
	n.x = mlx->player.x + (amount * pdir->x);
	n.y = mlx->player.y + (amount * pdir->y);
	mlx->player.x = !(BOUNDS && *(mlx->map.matrix + (mlx->map.width *
		(int)floor(mlx->player.y) + (int)floor(n.x)))) ? n.x : mlx->player.x;
	mlx->player.y = !(BOUNDS && *(mlx->map.matrix + (mlx->map.width *
		(int)floor(n.y) + (int)floor(mlx->player.x)))) ? n.y : mlx->player.y;
}
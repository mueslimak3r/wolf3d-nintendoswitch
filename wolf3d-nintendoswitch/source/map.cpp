#include "wolf.h"

static int	ft_cntwords(const char *t, char c)
{
	int i;
	int count;

	i = -1;
	i += 1;
	count = -1;
	count += 1;
	while (t[i])
	{
		while (t[i] == c)
			i++;
		if (t[i] && t[i] != c)
			count++;
		while (t[i] && t[i] != c)
			i++;
	}
	return (count);
}

static char	**ft_strsplit(char const *s, char c)
{
	int		h;
	int		i;
	int		j;
	char	**p;

	h = 0;
	i = -1;
	if (!s)
		return (NULL);
	if (!(p = (char **)malloc(sizeof(char *) * (ft_cntwords(s, c) + 1))))
		return (NULL);
	while (!(p[h] = NULL) && (++i) < (int)strlen(s))
	{
		while (s[i] && s[i] == c)
			i++;
		j = i;
		while (s[i] && s[i] != c)
			i++;
		if (j < i)
		{
			p[h] = (char*)malloc(i - j + 1);
            bzero(p[h], i - j + 1);
			strncpy(p[h++], (char *)s + j, (i - j));
		}
	}
	return (p);
}

static int	map_allot(t_map *map)
{
	if (!(map->matrix = (int*)malloc(sizeof(int) * map->height * map->width)))
		return (-1);
	return (1);
}

static int	check_map(t_mlx *stuff, std::string name)
{
    std::ifstream   file;
	std::string     line;
	t_map			*map = &stuff->map;

	file.open(name);
	bzero(map, sizeof(t_map));
    if (!file.is_open())
        return (0);
    if (!(std::getline(file, line)))
        return (0);
    const char *cstr = line.c_str();
    map->width = ft_cntwords(cstr, ' ');
	map->height++;
	while (std::getline(file, line))
	{
        cstr = line.c_str();
		if (!(map->width == ft_cntwords(cstr, ' ')))
		{
			file.close();
			return (0);
		}
		map->height++;
	}
	file.close();
	return (1);
}

static int	splint(t_mlx *mlx, int row, char **strs)
{
	int		i;
	bool	fail;

	fail = false;
	i = -1;
	if (row != 0 && row != mlx->map.height - 1 &&
		(*strs[0] != '1' || *(strs[mlx->map.width - 1]) != '1'))
		fail = true;
	while (++i < mlx->map.width)
	{
		//map->matrix[map->width * row + i] = atoi(strs[i]);

		*((mlx->map.matrix) + (mlx->map.width * row + i)) = atoi(strs[i]);
		if ((row == 0 || row == mlx->map.height - 1) && *((mlx->map.matrix)
			+ (mlx->map.width * row + i)) != 1)
			fail = true;
		if (*((mlx->map.matrix) + (mlx->map.width * row + i)) == 0 &&
			mlx->player.x == 0 && mlx->player.y == 0)
		{
			mlx->player.x = i + 0.5f;
			mlx->player.y = row + 0.5f;
		}

		free(strs[i]);
	}
	free(strs);
	return (fail ? 0 : 1);
}

void		map_destroy(t_map *map)
{
	if (map && map->matrix)
	{
		free(map->matrix);
	}
}

int			create_map(t_mlx *stuff, std::string name)
{
	std::ifstream   file;
	std::string     line;
	t_map			*map = &stuff->map;
	int		i;

	file.open(name);
	if (!check_map(stuff, name))
		return (0);
	if (0 > map_allot(map))
		return (-1);
    if (!file.is_open())
    {
        return (-1);
    }
    i = 0;
	while (i < map->height)
	{
		//getline(file, );
        std::getline(file, line);
        const char *cstr = line.c_str();
		//ft_printf("%s\n", line);
		if (!(splint(stuff, i, ft_strsplit(cstr, ' '))))
		{
			free(stuff->map.matrix);
			file.close();
			return (0);
		}
		i++;
	}
	file.close();
	return (1);
}
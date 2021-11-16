/*
    LIBFT FUNCTIONS
*/

#include "ft_ping.h"

float   ft_abs(float x)
{
	return x < 0 ? -x : x;
}

size_t  ft_strlenbin(char *param)
{
	size_t	            count;
    unsigned long long  *str = (unsigned long long *)param;

	count = 0;
	while (1)
		if ((++count && !(*str & 0x00000000000000FF)) ||\
			(++count && !(*str & 0x000000000000FF00)) ||\
			(++count && !(*str & 0x0000000000FF0000)) ||\
			(++count && !(*str & 0x00000000FF000000)) ||\
			(++count && !(*str & 0x000000FF00000000)) ||\
			(++count && !(*str & 0x0000FF0000000000)) ||\
			(++count && !(*str & 0x00FF000000000000)) ||\
			(++count && !(*str++ & 0xFF00000000000000)))
			return (count - 1);
}

int     ft_atoi(const char *str)
{
	long	nb;
	int		sign;
	int		i;

	i = 0;
	nb = 0;
	sign = 1;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == ' ')
		i++;
	if (str[i] == '+')
		i++;
	else if (str[i] == '-')
	{
		sign = -1;
		i++;
	}
	while (str[i] >= '0' && str[i] <= '9')
		nb = nb * 10 + str[i++] - '0';
	return ((int)(nb * sign));
}

void	ft_bzero(void *s, size_t n)
{
	size_t  i;
    char    *cast = (char *)s;

	i = 0;
	while (i < n)
		cast[i++] = '\0';
}

void	*ft_memcpy(void *dest, const void *src, size_t n)
{
	char	*castsrc;
	char	*castdest;
	size_t	len1;
	size_t	i;

	castsrc = (char *)src;
	castdest = (char *)dest;
	len1 = ft_strlenbin(castsrc);
	i = 0;
	while (i < n && i < len1)
	{
		castdest[i] = castsrc[i];
		i++;
	}
	return (castdest);
}

int     ft_strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2 && *s1 && *s2)
	{
		s1++;
		s2++;
	}
	return ((unsigned char)(*s1) - (unsigned char)(*s2));
}

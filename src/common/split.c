#include <blade.h>

int	in(char *charset, char c)
{
	while (*charset)
	{
		if (*charset == c)
			return (1);
		charset++;
	}
	return (0);
}

int	count_words(char *str, char *charset)
{
	int	words_len;
	int	word_len;
	int	i;

	i = 0;
	words_len = 0;
	word_len = 0;
	while (str[i])
	{
		if (in(charset, str[i]))
		{
			if (word_len)
			{
				words_len++;
				word_len = 0;
			}
			i++;
			continue ;
		}
		word_len++;
		i++;
	}
	if (word_len > 0)
		words_len++;
	return (words_len);
}

int	length_until(char *str, int idx, char *charset)
{
	int	len;

	len = 0;
	while (!in(charset, str[idx + len]) && str[idx + len])
		len++;
	return (len);
}

void	ft_split_private(char **vec, char *str, char *charset)
{
	int		wlength;
	int		i;
	int		k;
	int		vec_idx;

	k = 0;
	i = 0;
	vec_idx = 0;
	while (str[i])
	{
		while (in(charset, str[i]))
			i++;
		wlength = length_until(str, i, charset);
		if (wlength > 0)
		{
			vec[vec_idx] = malloc(wlength + 1);
			while (k < wlength && str[i])
				vec[vec_idx][k++] = str[i++];
			vec[vec_idx++][k++] = 0;
			k = 0;
		}
	}
	vec[vec_idx] = NULL;
}

char	**split(char *str, char *charset, size_t *count)
{
	char	**vec;
	*count = (count_words(str, charset));
	vec = malloc(sizeof(char *) * (*count + 1));
	ft_split_private(vec, str, charset);
	return (vec);
}

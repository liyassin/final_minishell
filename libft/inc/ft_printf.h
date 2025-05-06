/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anassih <anassih@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 17:16:24 by anassih           #+#    #+#             */
/*   Updated: 2025/01/23 18:14:38 by anassih          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

# include <stdarg.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

int	ft_printf(const char *format, ...);
int	ft_handle_char(va_list ap);
int	ft_handle_string(va_list ap);
int	ft_handle_int(va_list ap);
int	ft_handle_unsigned(va_list ap);
int	ft_handle_pointer(va_list ap);
int	ft_handle_hexlower(va_list ap);
int	ft_handle_hexupper(va_list ap);
int	ft_handle_percent(void);
int	ft_putchar(char c);
int	ft_putnbr_base(unsigned long long nb, const char *base);

#endif
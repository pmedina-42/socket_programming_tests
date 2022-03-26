# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pmedina- <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/03/23 22:21:34 by pmedina-          #+#    #+#              #
#    Updated: 2022/03/23 22:26:43 by pmedina-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SNAME	=	server
SSRC	=	srcs/main.cpp
CNAME	=	client
CSRC	=	srcs/client.cpp
CXX		=	g++ -fsanitize=address -g3
RM		=	rm -f

NAME:
	@$(CXX) $(SSRC) -o $(SNAME)
	@$(CXX) $(CSRC) -o $(CNAME)

clean:
	@$(RM) $(SNAME) $(CNAME)
	

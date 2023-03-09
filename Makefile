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

SNAME = server
SSRC = srcs/main.cpp
CNAME = client
CSRC = srcs/client.cpp
CXX = g++
RM = rm -f
SOBJS = $(SSRC:.cpp=.o)
COBJS = $(CSRC:.cpp=.o)

%.o: %.cpp
	$(CXX) -c $< -o $@

all: $(SNAME) $(CNAME)

$(SNAME): $(SOBJS)
	$(CXX) $(SOBJS) -o $(SNAME)

$(CNAME): $(COBJS)
	$(CXX) $(COBJS) -o $(CNAME)

clean:
	@$(RM) $(SNAME) $(CNAME) $(SOBJS) $(COBJS)	

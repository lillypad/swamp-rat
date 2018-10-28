##########################################################################
# Copyright (C) 2018  Lilly Chalupowski                                  #
#                                                                        #
# This program is free software: you can redistribute it and/or modify   #
# it under the terms of the GNU General Public License as published by   #
# the Free Software Foundation, either version 3 of the License, or      #
# (at your option) any later version.                                    #
#                                                                        #
# This program is distributed in the hope that it will be useful,        #
# but WITHOUT ANY WARRANTY; without even the implied warranty of         #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          #
# GNU General Public License for more details.                           #
#                                                                        #
# You should have received a copy of the GNU General Public License      #
# along with this program.  If not, see <https://www.gnu.org/licenses/>. #
##########################################################################

.PHONY: docs
.PHONY: build
.PHONY: all

all: build docs

build:
	mkdir -p bin/
	gcc -Wall -Werror -g -v src/main.c -o bin/swamp-rat -pthread -lcurl -lncurses -lform -lmenu -luuid
	gcc -v src/stub.c -o bin/stub -pthread -lcurl -luuid
	objcopy bin/swamp-rat --add-section rodata=bin/stub
	cp bin/swamp-rat swamp-rat

docs:
	cd docs/ && \
		pdflatex -shell-escape swamp-rat.tex && \
		pdflatex -shell-escape swamp-rat.tex && \
		rm -f *.log && \
		rm -f *.aux && \
		rm -f *.toc && \
		rm -f *.out && \
		rm -f *.nav && \
		rm -f *.snm && \
		rm -f *.vrb && \
		rm -rf _minted-swamp-rat/ 

clean:
	rm -f swamp-rat
	rm -rf bin/
	rm -rf ssl/

install:
	cp bin/swamp-rat /usr/bin/swamp-rat

uninstall:
	rm -f /usr/bin/swamp-rat

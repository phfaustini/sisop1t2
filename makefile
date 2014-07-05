all: libt2fs.a

libt2fs.a: ./bin/t2fs.o ./bin/funcoesAuxiliares.o ./lib/apidisk.o 
	ar crs ./lib/libt2fs.a ./bin/t2fs.o ./bin/funcoesAuxiliares.o ./lib/apidisk.o 

./bin/t2fs.o: ./src/t2fs.c 
	gcc -c ./src/t2fs.c -Wall -o ./bin/t2fs.o 

./bin/funcoesAuxiliares.o: ./src/funcoesAuxiliares.c
	gcc -c ./src/funcoesAuxiliares.c -Wall -o ./bin/funcoesAuxiliares.o

./lib/apidisk.o: 
	cd lib && ar -x ./libapidisk.a 
	mv ./lib/apidisk*.o ./lib/apidisk.o

clean: 
	rm -f ./lib/libt2fs.a ./bin/* ./lib/apidisk.o

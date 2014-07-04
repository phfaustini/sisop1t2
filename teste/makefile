all: teste

teste: teste.c libt2fs.a
	gcc -o teste teste.c -L ../lib -lt2fs -Wall

libt2fs.a: ../bin/t2fs.o ../bin/funcoesAuxiliares.o ../lib/apidisk.o 
	ar crs ../lib/libt2fs.a ../bin/t2fs.o ../bin/funcoesAuxiliares.o ../lib/apidisk.o 

../bin/t2fs.o: ../src/t2fs.c 
	gcc -c ../src/t2fs.c -Wall -o ../bin/t2fs.o 

../bin/funcoesAuxiliares.o: ../src/funcoesAuxiliares.c
	gcc -c ../src/funcoesAuxiliares.c -Wall -o ../bin/funcoesAuxiliares.o

../lib/apidisk.o: 
	cd ../lib && ar x ./libapidisk.a && cd ../teste/
	mv ../lib/apidisk*.o ../lib/apidisk.o

clean: 
	rm -f ../lib/libt2fs.a ../bin/*.o ../lib/apidisk.o

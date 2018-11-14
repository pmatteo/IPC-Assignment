#!/bin/bash

make --file=Makefile_thr clean && make --file=Makefile_thr
make clean && make 

for (( i = 0; i < $1; i++ )); do
    echo "" > "test/matAR"
    echo "" > "test/matBR"
    echo "" > "test/matC"
    echo "" > "test/log/log.txt"
    
    # CALCOLO RANDOM LA DIM E IL NUMERO DI PROC
    dim=$((RANDOM % 12 + 5 ))
    proc=$((RANDOM % 35 + 5 ))
    # GENERO MAT RANDOM DI DIMENSIONE $dim
    python test/rand_mat.py $dim

    # PROC 
    echo "#################################################"
	echo "dim: $dim"
	echo "n_proc: $proc"
	echo -n "Proc stats:"
    time ./calcola.x 	test/matAR test/matBR test/matC $dim $proc >> test/log/log.txt 
    diff test/matAR test/matC

    echo 
    
    # TRHEAD
 	echo -n "Thr stats:"
    time ./calcola_thr.x test/matAR test/matBR test/matC $dim >> test/log/log.txt
    diff test/matAR test/matC

    # CONTROLLO CHE LE SOMME SIANO UGUALI
    arr=(`cat "test/log/log.txt" | grep "sum =" | cut -d= -f2-`)
	if (( ${arr[1]} == ${arr[0]} )); then
		echo "SUM OK"
	else
		echo "SUM NOT OK: sum_prco = ${arr[0]}, sum_thr = ${arr[1]}" 
		exit 1
	fi
done
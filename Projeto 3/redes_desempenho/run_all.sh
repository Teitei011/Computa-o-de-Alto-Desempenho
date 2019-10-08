#!/bin/bash
# rm "time_for_all.txt"
for f in *.edgelist
do
  tempo=""
  texto=""
  tempo=$(./vector.exe $f)
  echo "Arquivo $f demorou: $tempo s"
  echo "Arquivo $f demorou:  $tempo" >> "time_for_all.txt"
done

rm *.TO_REMOVE

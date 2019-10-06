#!/bin/bash
#Run all *.edgelist
for f in *.edgelist
do
    $(./vector.exe $f)
done

#Compare everyone now
files=""
replace=".trg"
for f in *.TO_REMOVE
do
  files="$($f/.TO_REMOVE/$replace)"
  echo "Files is $files"
  # echo $(cmp --silent f $files && echo '### SUCCESS: Files Are Identical! ###' || echo '### WARNING: Files Are Different! ###')
done

# rm *.TO_REMOVE

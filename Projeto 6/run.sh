#!/bin/bash
# cmp --silent $1 $2 && echo '### SUCCESS: Files Are Identical! ###' || echo '### WARNING: Files Are Different! ###'

#
#Run all *.edgelist
for f in *.edgelist
do
    $(./vector $f)
done

#Compare everyone now
files=""
replace=".trg"
for f in *.TO_REMOVE
do
  files=$f
  echo ${$files/.TO_REMOVE/$replace}
  $(cmp --silent f $files && echo '### SUCCESS: Files Are Identical! ###' || echo '### WARNING: Files Are Different! ###'
)
done

rm *.TO_REMOVE

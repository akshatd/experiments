#!/bin/bash

hosts=( "cenwat" "whatwho" )

printf "\n***\nTo see the progress of the push, in a new terminal run this command:\nwatch -c -n 1 tail -n 10 "
printf "./tmp/%s.log " "${hosts[@]}"
printf "\n***\n"

for i in ${hosts[@]}; do echo $i; done
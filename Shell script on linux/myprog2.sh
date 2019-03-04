#!/bin/bash

echo "Enter a sequence of numbers followed by 'end' "

maxNumber=0 #initialize maximum number as 0
while true; #read inputs in a loop
do
  read input
  if [[ "$input" == "end" ]]; then
    echo "maximum : $maxNumber" #if input is equal to "end" terminate loop and print maximum number
    break
  else
    if [ $input -gt $maxNumber ]; then
      maxNumber=$input #if current input is greater than previous one,update maxNumber
    fi
  fi
done

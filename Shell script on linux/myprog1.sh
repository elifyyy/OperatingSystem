#!/bin/bash
FILE=$1 #Takes the file containing an intger per line as an command line argument
str="*" #String that will be printed according to corresponding integer from the file

if [ -f $FILE ]; then #Checks it the file exists.
  while IFS='' read -r line || [[ -n "$line" ]]; #Read file line by line
  do
      star=" " #Create an empty string
      for ((number=0;number < $line;number++)){ #loop until iteration number reaches the number readed from the file.
        star="${star}$str" #Create the string that will be printed.(Append '*' to string in for loop.)
      }
      echo "$star"
  done < "$1"
fi

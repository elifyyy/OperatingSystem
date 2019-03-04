#!/bin/bash

dir=$1 #take the directory as a command line argument


if [[ "$dir" == "" ]]; then
  var=0
  echo "number of deleted files from current directory : "
  find ./ -size 0 | wc -l #count 0 size files in the current directory
  find ./ -size 0 -delete #if no directory is given,then delete files from current directory



else

  echo "number of deleted files from $dir : "
  find $dir -size 0 | wc -l #count 0 size files in the given directory
  find $dir -type f -size 0 -delete #else delete files from given directory
fi

#!/bin/bash
WINDOWS=1
OSX=1
RELEASE=0
TEST=0
SYNTAX=0
SANITISE=0
while [[ $# -gt 0 ]]; do
  case $1 in
    -w|--windows)
      WINDOWS=0
      shift # past argument
      ;;
    -o|--osx)
      OSX=0
      shift
      ;;
    -r|--release)
      RELEASE=1
      shift
      ;;
    -t|--test)
      TEST=1
      shift
      ;;
    -s|--syntax)
      SYNTAX=1
      shift
      ;;
    --sanitise)
      SANITISE=1
      shift
      ;;
    -*|--*)
      echo "Unknown option $1"
      exit 1
      ;;
    *)
      POSITIONAL_ARGS+=("$1") # save positional arg
      shift # past argument
      ;;
  esac
done


for file in build CMakeFiles cmake_install.cmake CMakeCache.txt Makefile Jerboa
do
  if [ -d $file ];
  then
    rm -rf $file
  fi
  if [ -f $file ];
  then
    rm $file
  fi
done

echo "release ${RELEASE}"

if [[ $WINDOWS -eq 0 ]];
then 
  cmake -E make_directory build
  cmake -E chdir build cmake .. -D WINDOWS=ON -D RELEASE=$RELEASE -D TEST_SUITE=$TEST -D SYNTAX_ONLY=$SYNTAX -D SANITISE=$SANITISE -D CMAKE_TOOLCHAIN_FILE=./windows.cmake && make -j 8 -C build
elif [[ $OSX -eq 0 ]];
then
  cmake -E make_directory build
  cmake -E chdir build cmake .. -D OSX=ON -D RELEASE=$RELEASE -D TEST_SUITE=$TEST -D SYNTAX_ONLY=$SYNTAX -D SANITISE=$SANITISE -D CMAKE_TOOLCHAIN_FILE=./osx.cmake && make -j 8 -C build
else
  cmake -E make_directory build
  cmake -E chdir build cmake -D RELEASE=$RELEASE -D TEST_SUITE=$TEST -D SANITISE=$SANITISE -D SYNTAX_ONLY=$SYNTAX .. && make -j 8 -C build
fi

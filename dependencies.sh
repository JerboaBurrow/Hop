#!/bin/bash
WINDOWS=1
while [[ $# -gt 0 ]]; do
  case $1 in
    -w|--windows)
      WINDOWS=0
      shift # past argument
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

cd demo/include
if [ -d SFML-2.5.1 ];
then
	echo "Directory $DEP exists, overwrite?"
	read -p "[y/n]: " I
	if [ "$I" = "n" ];
	then
		echo "Quitting"
		exit
	fi
fi

rm -rf SFML-2.5.1
tar -xvf SFML-2.5.1.tar.gz
cd SFML-2.5.1
mkdir build && cd build
cmake .. -D BUILD_SHARED_LIBS=FALSE && make
export SFML_DIR="../../SFML-2.5.1/build"


# TODO build shared libs for SFML, FREETYPE undefined??
# if [[ $WINDOWS -eq 0 ]];
# then 
# 	cd ..
# 	mkdir build-windows && cd build-windows
# 	cmake .. -D BUILD_SHARED_LIBS=TRUE -D CMAKE_TOOLCHAIN_FILE=../../../windows.cmake && make
# fi

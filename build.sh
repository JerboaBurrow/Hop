#!/bin/bash

function findAndCopyDLL() 
{
    for i in "${paths[@]}"
    do
        FILE="$i/$1"
        if [ -f $FILE ]; then
           echo -e "\033[1;34mFound DLL $FILE\033[0m"
           cp $FILE build/
           return 0
        fi
    done

    return 1
}

function mergeLibs()
{
  pushd .
  cd $1

  for lib in *.a
  do
      cp $lib $lib.tmp
      echo -e "\033[1;34mMerging $lib\033[0m"
      if [[ $WINDOWS -eq 0 ]];
      then 
        mkdir "$lib-o"
        ar -x $lib
        mv *.obj "$lib-o"
        ar -r libMerged.a "$lib-o/"*
        rm -rf "$lib-o"
      else
        mkdir "$lib-o"
        ar -x $lib
        mv *.o "$lib-o"
        ar -r libMerged.a "$lib-o/"*
        rm -rf "$lib-o"
      fi
      mv $lib.tmp $lib
  done

  mv libMerged.a $2

  popd
}

function buildAndroid()
{

  cmake -E make_directory build-$1
  # 24 required for vulkan https://github.com/nihui/ncnn-android-yolov5/issues/10#issuecomment-800374356
  cmake -E chdir build-$1 cmake .. -D ANDROID=ON -D ANDROID_PLATFORM=24 -D ANDROID_ABI=$1 -D BUILD_DEMOS=$DEMO -D RELEASE=$RELEASE -D TEST_SUITE=$TEST -D SYNTAX_ONLY=$SYNTAX -D SANITISE=$SANITISE -D CMAKE_TOOLCHAIN_FILE=$TOOL_CHAIN && make -j 4 -C build-$1
  STATUS=$?
  if [[ $MERGE -eq 1 ]]
  then
    mergeLibs "build-$1/include/jGL" libjGL.a
    mergeLibs "build-$1" libHop.a 
  fi
  mv build-$1/include/jGL/libjGL.a build/libjGL-$1.a
  mv build-$1/libHop.a build/libHop-$1.a
  rm -rf build-$1
}

WINDOWS=1
OSX=1
RELEASE=0
TEST=0
SYNTAX=0
SANITISE=0
DEMO=0
ANDROID_NDK=""
VK_SDK="$(pwd)/include/jGL/include/vendored/VulkanSDK"
BENCHMARK=0
STANDALONE=0
CLEAN=1
PROFILE=0
MERGE=1

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
    -s|--standalone)
      STANDALONE=1
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
    -d|--demo)
      DEMO=1
      shift
      ;;
    --sanitise)
      SANITISE=1
      shift
      ;;
    --android)
      ANDROID_NDK=$2
      shift
      shift
      ;;
    --vk)
      VK_SDK=$2
      shift
      shift
      ;;
    -b|--benchmark)
      BENCHMARK=1
      shift
      ;;
    -c|--continue)
      CLEAN=0
      shift
      ;;
    --no-merge)
    MERGE=0
    shift
    ;;
    -p|--profile)
      PROFILE=1
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

export VULKAN_SDK=$VK_SDK
export VULKAN_LIBRARY="$VK_SDK/Linux/Lib"
export VULKAN_INCLUDE_DIR="$VK_SDK/Include"

if [[ $CLEAN -eq 1 ]]; 
then
  if [ -d build ];
  then
    rm -rf build
  fi
  mkdir build

  STATUS=0

  if [[ $WINDOWS -eq 0 ]];
  then 
    export VULKAN_SDK=$VK_SDK/Windows
    export VULKAN_LIBRARY="$VK_SDK/Windows/Lib"
    export VULKAN_INCLUDE_DIR="$VK_SDK/Windows/Include" 
    ln -s "$VK_SDK/Windows/Lib" "$VK_SDK/Windows/lib"
    ln -s "$VK_SDK/Include" "$VK_SDK/Windows/Include"
    ln -s "$VK_SDK/Windows/Include" "$VK_SDK/Windows/include"
    cd build
    cmake .. -D WINDOWS=ON -D STANDALONE=$STANDALONE -D BUILD_DEMOS=$DEMO -D RELEASE=$RELEASE -D BENCHMARK=$BENCHMARK -D TEST_SUITE=$TEST -D SYNTAX_ONLY=$SYNTAX -D SANITISE=$SANITISE -D CMAKE_TOOLCHAIN_FILE=./windows.cmake && make -j 4
    STATUS=$?
    cd ..

    # now copy dlls
    PREFIX="x86_64-w64-mingw32"

    paths=("/usr/local/mingw64/bin"
      "/usr/local/mingw64/bin/x64"
      "/usr/$PREFIX/bin"
      "/usr/$PREFIX/lib"
    )

    for p in /usr/lib/gcc/$PREFIX/*
    do 
      paths+=($p)
    done

    echo -e "\n###############\nChecking Paths: \n"
    for p in "${paths[@]}"
    do
      echo -e "$p\n"
    done 
    echo -e "###############\n"

    dll=("libgcc_s_seh-1.dll"
      "libstdc++-6.dll"
      "libwinpthread-1.dll"
    )
    for j in "${dll[@]}"
    do
      findAndCopyDLL $j || echo "Could not find $j"
    done
  elif [[ $OSX -eq 0 ]];
  then
    cd build
    cmake .. -D OSX=ON -D STANDALONE=$STANDALONE -D BUILD_DEMOS=$DEMO -D RELEASE=$RELEASE -D BENCHMARK=$BENCHMARK -D TEST_SUITE=$TEST -D SYNTAX_ONLY=$SYNTAX -D SANITISE=$SANITISE -D CMAKE_TOOLCHAIN_FILE=./osx.cmake && make -j 4
    STATUS=$?
    cd ..
  elif [[ ! -z "$ANDROID_NDK" ]]
  then
    TOOL_CHAIN="$ANDROID_NDK/build/cmake/android.toolchain.cmake"
    if [[ ! -f "$TOOL_CHAIN" ]]
    then
      echo -e "\nCould not find tool chain file at $TOOL_CHAIN\n"
      exit 1
    fi
    echo -e "\nFound Android tool chain $TOOL_CHAIN\n"
    mkdir build

    buildAndroid arm64-v8a
    buildAndroid armeabi-v7a
    buildAndroid x86
    buildAndroid x86_64

  else
    cd build
    cmake -D BUILD_DEMOS=$DEMO -D PROFILE=$PROFILE -D STANDALONE=$STANDALONE -D RELEASE=$RELEASE -D BENCHMARK=$BENCHMARK -D TEST_SUITE=$TEST -D SANITISE=$SANITISE -D SYNTAX_ONLY=$SYNTAX .. && make -j 4
    STATUS=$?
    cd ..
  fi
else 
  cd build && make -j 4 
  STATUS=$?
  cd ..
fi

if [[ -z "$ANDROID_NDK" ]] && [[ $MERGE -eq 1 ]]
then 
  mergeLibs "build" libHop.a
  mergeLibs "build/include/jGL" libjGL.a
  cp build/include/jGL/libjGL.a build
fi

exit $STATUS
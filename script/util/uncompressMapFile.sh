#!/bin/bash
FILE=""
while [ $# -gt 0 ]; do
  case $1 in
    -f|--file)
      FILE=$2
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

if [ "$FILE" == "" ]; then
    echo "No file specified"
    exit 1
fi

if [ -f $FILE ]; then
    # ignore header (line one is text, line two is bytes in uncompressed data)
    tail -n +3 $FILE > tmp.hmap.z
    # magic https://unix.stackexchange.com/questions/22834/how-to-uncompress-zlib-data-in-unix
    printf "\x1f\x8b\x08\x00\x00\x00\x00\x00" |cat - tmp.hmap.z |gzip -dc > tmp.hmap
    rm tmp.mapz
    echo "Uncompressed to tmp.hmap!"
else
    echo "No such file $FILE"
fi
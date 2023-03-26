FILE=""
HEADER=1
while [ $# -gt 0 ]; do
  case $1 in
    -f|--file)
      FILE=$2
      shift # past argument
      ;;
    -h|--header)
      HEADER=0
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

if [ "$FILE" == "" ]; then
    echo "No file specified"
    exit 1
fi

NAME=${FILE##*/}
NAME=${NAME%.*}
echo $NAME

if [[ $HEADER -eq 0 ]]
then
    echo -e "static const unsigned char\n$NAME={" > $NAME.byte
    hexdump -ve '1/1 "0x%.2x, "' $FILE >> $NAME.byte
    echo -e "\n};" >> $NAME.byte
else
    hexdump -ve '1/1 "0x%.2x, "' $FILE > $NAME.byte
fi

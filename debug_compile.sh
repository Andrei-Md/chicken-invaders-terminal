#!/bin/bash
#input:
# arg1: folder
# arg2: nume final

CFLAGS="-DDEBUG -g -O3 -Wall"
LIBS="-pthread"
function compilare_obj()
{
    OUTPUT=$(dirname $1)/$(basename $1 .c).o
    CFILE=$1
    # echo "fisier sursa $(basename $1 .c).c: gcc $CFLAGS -c -o $OUTPUT $CFILE"
    echo gcc $CFLAGS -c -o $OUTPUT $CFILE
    gcc $CFLAGS -c -o $OUTPUT $CFILE
}

function afisare()
{
    echo "Continutul fisierului $1 este:"
    cat $1
}

function  compilare_final()
{
  unset OBJECTS
  unset TMP
  PROJ_NAME=$2
  cnt=0
  for f in $(ls -A $1)
  do
    if [ ${f: -2} == ".o" -a $cnt -eq 0 ];
    then
      TMP=$1/$f
      OBJECTS+=$TMP
      cnt+=1
      # echo $TMP
    elif [ ${f: -2} == ".o" -a $cnt -gt 0 ];
    then
      TMP=$1/$f
      OBJECTS+=" $TMP"
      # echo $TMP
    elif [ ${f: -2} == ".o" ];
    then
      echo "eroare la concatenare fis obj $1"
    fi
  done

  if [ -z ${OBJECTS+word} ];
  then
    echo "no obj files"
    echo $OBJECTS
  else
    echo "final: $PROJ_NAME"
    echo "gcc -o $PROJ_NAME $OBJECTS $LIBS"
    gcc -o $1/$PROJ_NAME $OBJECTS $LIBS
  fi
}


# Validarea datelor de intrare

if [ $# -eq 0 ]
then
    echo "Utilizare: $0 director"
    exit 1  # Fail!
fi

if [ ! -d $1 -o ! -r $1  ]
then
    echo "Eroare: Primul argument, $1, nu este director, sau nu aveti drept de citire a acestuia!"
    exit 2  # Fail!
fi

for f in $(ls -A $1)
do
    case $1/$f in
      *.c   ) compilare_obj $1/$f ;;
      # *.txt ) echo "fisier text: $f"    ; afisare $1/$f ;;
    esac
done

compilare_final $1 $2;

exit 0  # Succes!
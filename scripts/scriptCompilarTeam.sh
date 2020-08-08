
echo " 
::: Comienzo compilacion Team ::: 
"
cd /home/utnso/workspace/tp-2020-1c-ManaOS-/Team/
mkdir Debug
cd Debug
mkdir src
gcc -I"/home/utnso/workspace/tp-2020-1c-ManaOS-/ComunParaTodos" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/Team.d" -MT"src/Team.o" -o "src/Team.o" "../src/Team.c"
gcc -L"/home/utnso/workspace/tp-2020-1c-ManaOS-/ComunParaTodos/Debug" -o "Team"  ./src/Team.o   -lcommons -lpthread -lComunParaTodos

echo "
 ::: Compile Team ::: 
 "
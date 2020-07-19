
echo "Comienzo compilacion GameCard"
cd /home/utnso/workspace/tp-2020-1c-ManaOS-/GameCard/
mkdir Debug
cd Debug
mkdir src
gcc -I"/home/utnso/workspace/tp-2020-1c-ManaOS-/ComunParaTodos" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/GameCard.d" -MT"src/GameCard.o" -o "src/GameCard.o" "../src/GameCard.c"
gcc -L"/home/utnso/workspace/tp-2020-1c-ManaOS-/ComunParaTodos/Debug" -o "GameCard"  ./src/GameCard.o   -lComunParaTodos -lm -lpthread -lcommons
echo "Compile GameCard"
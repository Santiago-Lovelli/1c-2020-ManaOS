
echo "
 ::: Comienzo compilacion gameboy :::
 "
cd /home/utnso/workspace/tp-2020-1c-ManaOS-/Gameboy/
mkdir Debug
cd Debug
mkdir src
gcc -I"/home/utnso/workspace/tp-2020-1c-ManaOS-/ComunParaTodos" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/GameBoy.d" -MT"src/GameBoy.o" -o "src/GameBoy.o" "../src/GameBoy.c"
gcc -L"/home/utnso/workspace/tp-2020-1c-ManaOS-/ComunParaTodos/Debug" -o "gameboy"  ./src/GameBoy.o   -lComunParaTodos -lpthread -lcommons
echo "
::: Compile gameboy :::
"
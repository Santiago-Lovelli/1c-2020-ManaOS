for i in {1..100}
do
./gameboy BROKER CAUGHT_POKEMON 1 OK
./gameboy BROKER CAUGHT_POKEMON 2 FAIL
./gameboy BROKER NEW_POKEMON Pikachu 2 3 1
./gameboy BROKER CATCH_POKEMON Onyx 4 5
./gameboy BROKER CATCH_POKEMON Charmander 4 5
./gameboy BROKER CATCH_POKEMON Pikachu 9 3 
./gameboy BROKER CATCH_POKEMON Squirtle 9 3 
./gameboy BROKER CAUGHT_POKEMON 10 OK
./gameboy BROKER CAUGHT_POKEMON 11 FAIL
./gameboy BROKER CATCH_POKEMON Bulbasaur 1 7
./gameboy BROKER CATCH_POKEMON Charmander 1 7 
./gameboy BROKER GET_POKEMON Pichu
./gameboy BROKER GET_POKEMON Raichu
./gameboy BROKER GET_POKEMON Pikachu
./gameboy BROKER GET_POKEMON Charmander
done
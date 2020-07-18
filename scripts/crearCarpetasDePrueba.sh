cd /home/utnso/Escritorio;
mkdir tall-grass;
cd tall-grass;
mkdir Metadata;
mkdir Files;
mkdir Blocks;
cd Metadata;
echo "BLOCK_SIZE=10
BLOCKS=5192
MAGIC_NUMBER=TALL_GRASS
" >> Metadata.bin;
echo "" >> Bitmap.bin;
echo "Lista las carpetas base"
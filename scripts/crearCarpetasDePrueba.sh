cd /home/utnso/Escritorio;
mkdir tall-grass;
cd tall-grass;
mkdir Metadata;
mkdir Files;
mkdir Blocks;
cd Metadata;
echo "BLOCK_SIZE=64
BLOCKS=1024
MAGIC_NUMBER=TALL_GRASS
" >> Metadata.bin;
echo "" >> Bitmap.bin;
echo "Lista las carpetas base"

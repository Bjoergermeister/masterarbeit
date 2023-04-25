mkdir -p Filesystem/Regular

# Create filesystem for ext4
mkdir -p Filesystem/Regular/dir
pushd Filesystem/Regular/dir > /dev/null

block_size=$((1024 * 1024)) # one megaybte
for count in 1 128 256 512 1024 2048
do
    dd if=/dev/urandom of=./$count.txt count=$count bs=$block_size status=none
done
popd > /dev/null

# Create filesystem for overlay
pushd Filesystem > /dev/null

mkdir upper workdir merged

popd > /dev/null
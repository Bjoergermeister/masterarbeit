mkdir -p Filesystem/Regular Filesystem/Overlay

# Create filesystem for ext4
mkdir -p Filesystem/Regular/dir
pushd Filesystem/Regular/dir > /dev/null

for i in {0..15}
do
    touch $i.txt
    dd if=/dev/urandom of=./$i.txt count=8 status=none
done
popd > /dev/null

# Create filesystem for overlay
mkdir -p Filesystem/Overlay
pushd Filesystem/Overlay > /dev/null

mkdir upper workdir merged

for i in {0..15}
do 
    mkdir -p Layer$i/dir
    touch Layer$i/dir/$i.txt
    dd if=/dev/urandom of=Layer$i/dir/$i.txt count=8 status=none
done
popd > /dev/null
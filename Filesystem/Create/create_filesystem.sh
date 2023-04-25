# Create filesystem for ext4
mkdir -p Filesystem/Regular

# Create filesystem for overlay
mkdir -p Filesystem/Overlay
pushd Filesystem/Overlay > /dev/null

mkdir lower upper workdir merged

popd > /dev/null
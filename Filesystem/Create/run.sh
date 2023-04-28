#!/bin/bash

# Run tests with a regular process on a container filesystem
function run_manual(){
    result_dir="../../../../../Results" 

    # C version   
    sudo mount -t overlay overlay -o lowerdir=lower,upperdir=upper,workdir=workdir merged

    cd merged
    
    ../../../C/create "new.txt" "$result_dir/Filesystem_Create_C_Manual.txt"

    cd ..

    sudo umount overlay
    rm upper/new.txt

    # Java version   
    sudo mount -t overlay overlay -o lowerdir=lower,upperdir=upper,workdir=workdir merged

    cd merged
    
    java -cp ../../../Java/ Main "new.txt" "$result_dir/Filesystem_Create_Java_Manual.txt"

    cd ..

    sudo umount overlay
    rm upper/new.txt
}

# Prepare and run regular tests
function regular() {
    echo "Executing tests in regular mode"

    pushd Filesystem/Regular > /dev/null

    result_dir="../../../../Results"
    for i in {0..99}
    do
        echo "$i von 99"

        # C version
        ../../C/create "new.txt" "${result_dir}/Filesystem_Create_C_Regular.txt"
        rm new.txt
    
        # Java version
        java -cp ../../Java Main "new.txt" "${result_dir}/Filesystem_Create_Java_Regular.txt"
        rm new.txt
    done

    popd > /dev/null
}

# Prepare and run manual tests
function manual() {
    echo "Executing tests in manual mode"

    pushd Filesystem/Overlay > /dev/null

    for i in {0..99}
    do
        echo "$i von 99"
        run_manual
    done

    popd > /dev/null
}

# Prepare and run container tests
function container() {

    pushd ../.. > /dev/null

    volume="type=bind,source=$(pwd)/Results,target=/Results"
    c_image="masterarbeit-filesystem-create-c"
    java_image="masterarbeit-filesystem-create-java"
    c_output_file="Results/Filesystem_Create_C_Container.txt"
    java_output_file="Results/Filesystem_Create_Java_Container.txt"

    for i in {0..99}
    do
        echo "$i von 99"
        docker run --name "$c_image" -it --rm --mount "$volume" "$c_image" ./create "new.txt" "$c_output_file" 
        docker run --name "$java_image" -it --rm --mount "$volume" "$java_image" java Main "new.txt" "$java_output_file"                 
    done

    popd > /dev/null
}

# Prepare and execute tests

if [ "$1" = "regular"  ] || [ "$1" = "" ] 
then 
    regular
fi

if [ "$1" = "manual"  ] || [ "$1" = "" ] 
then 
    manual
fi

if [ "$1" = "container"  ] || [ "$1" = "" ] 
then 
    container
fi

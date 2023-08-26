function regular(){
    echo "Running experiments in $1 mode"

    for i in {0..99}
    do
        echo "$((i + 1)) von 100"
        ./GameOfLife/gameoflife "../Results/GameOfLife_$1.txt"
        ./qsort/qsort "../Results/Qsort_$1.txt"
        ./n-body/nbody 1000000 "../Results/Nbody_$1.txt"
        ./binary-trees/binary-trees 22 "../Results/BinaryTrees_$1.txt"
    done
}

function container(){
    echo "Running experiments in container mode"
    
    base_image="masterarbeit-containervm"
    volume="type=bind,source=$(pwd)/../Results,target=/Results"
    for i in {0..99}
    do
        echo "$((i + 1)) von 100"
        docker run --name GameOfLife --rm --mount "$volume" $base_image-gameoflife ./gameoflife "Results/GameOfLife_Container.txt" 
        docker run --name Qsort --rm --mount "$volume" $base_image-qsort ./qsort "/Results/Qsort_Container.txt"                
        docker run --name Nbody --rm --mount "$volume" $base_image-nbody ./nbody 1000000 "/Results/Nbody_Container.txt"                
        docker run --name BinaryTrees --rm --mount "$volume" $base_image-binarytrees ./binary-trees 22 "/Results/BinaryTrees_Container.txt"                
    done
}

if [ "$1" = "regular"  ]; then regular "Regular"; fi

if [ "$1" = "vm"  ]; then regular "VM"; fi

if [ "$1" = "container"  ]; then container; fi


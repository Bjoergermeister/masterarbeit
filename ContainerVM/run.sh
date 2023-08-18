function regular(){
    for i in {0..99}
    do
        echo "$((i + 1)) von 100"
        ./GameOfLife/gameoflife "GameOfLife_$1.txt"
        ./qsort/qsort "Qsort_$1.txt"
    done
}

function container(){
    base_image="masterarbeit-containervm"
    volume="type=bind,source=$(pwd)/../Results,target=/Results"
    for i in {0..99}
    do
        echo "$i von 99"
        docker run --name GameOfLife --rm --mount "$volume" $base_image-gameoflife ./gameoflife "GameOfLife_Container.txt" 
        docker run --name Qsort --rm --mount "$volume" $base_image-qsort ./qsort "Qsort_Container.txt"                
    done
}

if [ "$1" = "regular"  ]; then regular "Regular"; fi

if [ "$1" = "vm"  ]; then regular "VM"; fi

if [ "$1" = "container"  ]; then container; fi


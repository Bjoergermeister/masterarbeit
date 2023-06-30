#!/bin/bash

# Prepare and run regular tests
function regular() {
    echo "Executing tests in regular mode"

    result_dir="../../Results"

    for i in {0..24}
    do
        echo "$((i + 1)) von 25"
        #C/client $DESTINATION_IP "${result_dir}/Network_Connect_C_Regular.txt"
        java -cp Java/ Client $DESTINATION_IP "${result_dir}/Network_Connect_Java_Regular.txt"
    done
}

# Prepare and run manual tests
function manual() {
    echo "Executing tests in manual mode"

    # Setup host part of networking
    sudo ip netns add container # Add network namespace
    sudo ip link add if-host type veth peer name if-container # Create VETH pair
    sudo ip link set dev if-host up # Up host interface
    sudo ip link set if-host master masterarbeit # Add host interface to network bridge
    sudo iptables --insert DOCKER-USER 1 -j ACCEPT
    sudo iptables -t nat --append POSTROUTING -s 172.19.0.0/16 -j MASQUERADE

    # Setup container part of networking
    sudo ip link set if-container netns container # Add container interface to network namespace
    sudo ip netns exec container ip addr add 172.19.0.2/16 dev if-container
    sudo ip netns exec container ip link set dev if-container up
    sudo ip netns exec container route add default gw 172.19.0.1 if-container # Add default route to namespace

    # Execute the ping programs
    result_dir="../../Results"
    # C version
    for i in {0..24}
    do
        echo "$((i + 1)) von 25"
        #sudo ip netns exec container C/client $DESTINATION_IP "$result_dir/Network_Connect_C_Manual.txt"
        sudo ip netns exec container java -cp Java Client $DESTINATION_IP "$result_dir/Network_Connect_Java_Manual.txt"
    done

    # Remove container networking
    sudo ip netns delete container
    sudo ip link delete if-host

}

# Prepare and run container tests
function container() {
    volume="type=bind,source=$(pwd)/../../Results,target=/Results"
    c_image="masterarbeit-network-connect-c"
    java_image="masterarbeit-network-connect-java"

    c_result_file="Results/Network_Connect_C_Container.txt"
    java_result_file="Results/Network_Connect_Java_Container.txt"

    for i in {0..24}
    do
        echo "$((i + 1)) von 25"
        #docker run --name "$c_image" --rm -p 5000:5000 --mount "$volume" --ip 172.17.0.2 "$c_image" ./client "$DESTINATION_IP" "$c_result_file"
        docker run --name "$java_image" -p 3000:3000 --rm --mount "$volume" "$java_image" java "Client" "$DESTINATION_IP" "$java_result_file"
    done
}

# Prepare and run container tests in privileged mode
function privileged() {
    volume="type=bind,source=$(pwd)/../../Results,target=/Results"
    c_image="masterarbeit-network-connect-c"
    java_image="masterarbeit-network-connect-java"

    c_result_file="Results/Network_Connect_C_Privileged.txt"
    java_result_file="Results/Network_Connect_Java_Privileged.txt"

    for i in {0..24}
    do
        echo "$((i + 1)) von 25"
        #docker run --name "$c_image" --rm -p 5000:5000 --privileged --mount "$volume" --ip 172.17.0.2 "$c_image" ./client "$DESTINATION_IP" "$c_result_file"
        docker run --name "$java_image" --rm -p 3000:3000 --privileged --mount "$volume" "$java_image" java "Client" "$DESTINATION_IP" "$java_result_file"
    done
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

if [ "$1" = "privileged"  ] || [ "$1" = "" ] 
then
    privileged
fi

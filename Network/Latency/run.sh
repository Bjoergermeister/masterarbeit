#!/bin/bash

# Prepare and run regular tests
function regular() {
    echo "Executing tests in regular mode"

    result_dir="../../Results"
    sudo C/latency 192.168.178.37 192.168.178.1 "${result_dir}/Network_Latency_C_Regular.txt"
    sudo java -cp Java/ Main 192.168.178.37 192.168.178.1 "${result_dir}/Network_Latency_Java_Regular.txt"
}

# Prepare and run manual tests
function manual() {
    echo "Executing tests in manual mode"

    # Setup host part of networking
    sudo ip link add masterarbeit type bridge
    sudo ip addr add 172.19.0.1/16 dev masterarbeit
    sudo ip netns add container # Add network namespace
    sudo ip link add if-host type veth peer name if-container # Create VETH pair
    sudo ip link set dev if-host up # Up host interface
    sudo ip link set if-host master masterarbeit # Add host interface to network bridge
    sudo iptables --insert DOCKER-USER 1 -j ACCEPT
    sudo iptables -t nat --append POSTROUTING -s 172.19.0.0/16 -j MASQUERADE
    sudo ip link set dev masterarbeit up

    # Setup container part of networking
    sudo ip link set if-container netns container # Add container interface to network namespace
    sudo ip netns exec container ip addr add 172.19.0.2/16 dev if-container
    sudo ip netns exec container ip link set dev if-container up
    sudo ip netns exec container route add default gw 172.19.0.1 if-container # Add default route to namespace

    # Execute the ping programs
    result_dir="../../Results"

    sudo ip netns exec container C/latency 172.19.0.2 192.168.178.1 "$result_dir/Network_Latency_C_Manual.txt"
    sudo ip netns exec container java -cp Java Main 172.19.0.2 192.168.178.1 "$result_dir/Network_Latency_Java_Manual.txt"

    # Remove container networking
    sudo ip netns delete container
    sudo ip link delete if-host
    sudo ip link delete masterarbeit
}

# Prepare and run container tests
function container() {
    echo "Executing tests in container mode"

    volume="type=bind,source=$(pwd)/../../Results,target=/Results"
    c_image="masterarbeit-network-latency-c"
    java_image="masterarbeit-network-latency-java"
    c_result_file="Results/Network_Latency_C_Container.txt"
    java_result_file="Results/Network_Latency_Java_Container.txt"

    docker run --name "$c_image" --rm --mount "$volume" "$c_image" ./latency "172.17.0.2" "192.168.178.1" "$c_result_file"
    docker run --name "$java_image" --rm --mount "$volume" "$java_image" java "Main" "172.17.0.2" "192.168.178.1" "$java_result_file"
}

# Prepare and run container tests in privileged mode
function privileged() {
   echo "Executing tests in privileged mode"

    volume="type=bind,source=$(pwd)/../../Results,target=/Results"
    c_image="masterarbeit-network-latency-c"
    java_image="masterarbeit-network-latency-java"
    c_result_file="Results/Network_Latency_C_Privileged.txt"
    java_result_file="Results/Network_Latency_Java_Privileged.txt"

    docker run --name "$c_image" --rm --privileged --mount "$volume" --ip 172.17.0.2 -it "$c_image" ./latency "172.17.0.2" "192.168.178.1" "$c_result_file"
    docker run --name "$java_image" --rm --privileged --mount "$volume" "$java_image" java "Main" "172.17.0.2" "192.168.178.1" "$java_result_file"
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
#!/bin/bash

# Prepare and run regular tests
function regular() {
    echo "Executing tests in regular mode"

    result_dir="../../Results"

    for i in {0..25}
    do
        echo "$((i + 1)) von 25"
        C/client $SOURCE_IP $DESTINATION_IP "${result_dir}/Network_Throughput_C_Regular.txt"
        java -cp Java/ Client 192.168.178.37 192.168.178.27 "${result_dir}/Network_Throughput_Java_Regular.txt"
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
    for i in {0..25}
    do
        echo "$((i + 1)) von 25"
        sudo ip netns exec container C/client 172.19.0.2 $DESTINATION_IP "$result_dir/Network_Throughput_C_Manual.txt"
        sudo ip netns exec container java -cp Java Client 172.19.0.2 $DESTINATION_IP "$result_dir/Network_Throughput_Java_Manual.txt"
    done

    # Remove container networking
    sudo ip netns delete container
    sudo ip link delete if-host

}

# Prepare and run container tests
function container() {
    volume="type=bind,source=$(pwd)/../../Results,target=/Results"
    c_image="masterarbeit-network-throughput-c"
    java_image="masterarbeit-network-throughput-java"

    c_result_file="Results/Network_Throughput_C_Container.txt"
    java_result_file="Results/Network_Throughput_Java_Container.txt"

    for i in {0..25}
    do
        echo "$((i + 1)) von 25"
        docker run --name "$c_image" --rm -p 5000:5000 --mount "$volume" --ip 172.17.0.2 "$c_image" ./client "172.17.0.2" "$DESTINATION_IP" "$c_result_file"
        docker run --name "$java_image" -p 3000:3000 --rm --mount "$volume" "$java_image" java "Client" "172.17.0.2" "192.168.178.27" "$java_result_file"
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

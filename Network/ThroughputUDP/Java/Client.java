import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class Client {

    public static final int PORT = 3000;
    public static final int BYTES_IN_ONE_GIGABYTE = 1024 * 1024 * 1024;

    public static void main(String[] args) {
        byte[] buffer = new byte[1460];
        for (int i = 0; i < buffer.length; i++) {
            buffer[i] = (byte) 'A';
        }

        InetAddress source = null;
        InetAddress destination = null;
        try {
            source = InetAddress.getByName(args[0]);
            destination = InetAddress.getByName(args[1]);
        } catch (UnknownHostException e) {
            System.out.println(String.format("UnknownHostException: %s", e.getMessage()));
            System.exit(-1);
        }

        DatagramSocket socket = null;
        try {
            socket = new DatagramSocket(PORT, source);
        } catch (SocketException e) {
            System.out.println(e.getMessage());
            System.exit(-1);
        }

        long totalBytesSend = 0;
        DatagramPacket packet = new DatagramPacket(buffer, buffer.length, destination, PORT);
        while (totalBytesSend < BYTES_IN_ONE_GIGABYTE) {
            if (BYTES_IN_ONE_GIGABYTE - totalBytesSend < buffer.length) {
                for (int j = 0; j < 8; j++) {
                    buffer[j] = '\0';
                }
            }

            trySendMessage(socket, packet);

            totalBytesSend += buffer.length;

        }

        byte[] response = new byte[50];
        DatagramPacket responsePacket = new DatagramPacket(response, response.length, destination, PORT);
        tryReceiveMessage(socket, responsePacket);
        float throughput = Float.parseFloat(new String(responsePacket.getData()));

        save(args[2], throughput);
    }

    static void tryReceiveMessage(DatagramSocket socket, DatagramPacket packet) {
        try {
            socket.receive(packet);
        } catch (IOException e) {
            System.out.println(e.getMessage());
            System.exit(-1);
        }
    }

    static void trySendMessage(DatagramSocket socket, DatagramPacket packet) {
        try {
            socket.send(packet);
        } catch (IOException e) {
            System.out.println(e.getMessage());
            System.exit(-1);
        }
    }

    static void save(String fileName, float result) {
        try {
            FileWriter fileWriter = new FileWriter(fileName, true);
            BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);

            bufferedWriter.append(Float.toString(result) + "\n");
            bufferedWriter.close();
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
        }
    }

}
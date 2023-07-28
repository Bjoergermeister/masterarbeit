import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class Server {

    public static final int PORT = 3000;
    public static final String STOP_FLAG = "\0\0\0\0\0\0\0\0";

    public static void main(String[] args) {
        byte[] buffer = new byte[1460];

        InetAddress source = null;
        InetAddress destination = null;
        try {
            source = InetAddress.getByName(args[0]);
            destination = InetAddress.getByName(args[1]);
        } catch (UnknownHostException e) {
            System.out.println(String.format("UnknownHostException: %s", e.getMessage()));
            System.exit(-1);
        }

        DatagramSocket socket = tryOpenSocket(source);
        DatagramPacket packet = new DatagramPacket(buffer, buffer.length);

        while (true) {
            long totalReceivedBytes = 0;
            long receiveCount = 0;
            long startTime = 0;
            long endTime = 0;
            boolean isFirstPacket = true;

            do {
                tryReceivePacket(socket, packet);
                receiveCount++;
                System.out.printf("%d, ", receiveCount);

                if (isFirstPacket) {
                    isFirstPacket = false;
                    startTime = System.nanoTime();
                }

                int length = packet.getLength();
                totalReceivedBytes += length;
            } while (isStopFlag(packet.getData()) == false);

            endTime = System.nanoTime();

            // Calculate throughput
            float timeDifferenceInSeconds = (endTime - startTime) / 1000000000.0f;
            float receivedMegabytes = totalReceivedBytes / (float) (1024 * 1024);
            float throughput = receivedMegabytes / (float) timeDifferenceInSeconds;

            System.out.printf("Received %d bytes, throughput: %f\n", totalReceivedBytes, throughput);

            // Send result back to client
            byte[] response = String.format("%f#%d", throughput, receiveCount).getBytes();
            DatagramPacket responsePacket = new DatagramPacket(response, response.length, destination, PORT);
            trySendPacket(socket, responsePacket);
        }
    }

    static DatagramSocket tryOpenSocket(InetAddress sourceAddress) {
        DatagramSocket socket = null;
        try {
            socket = new DatagramSocket(PORT, sourceAddress);
        } catch (SocketException e) {
            System.out.println(e.getMessage());
            System.exit(-1);
        }
        return socket;
    }

    static void tryReceivePacket(DatagramSocket socket, DatagramPacket packet) {
        try {
            socket.receive(packet);
        } catch (IOException e) {
            System.out.println(e.getMessage());
            System.exit(-1);
        }
    }

    static void trySendPacket(DatagramSocket socket, DatagramPacket packet) {
        try {
            socket.send(packet);
        } catch (IOException e) {
            System.out.println(e.getMessage());
            System.exit(-1);
        }
    }

    static boolean isStopFlag(byte[] message) {
        // String messageAsString = new String(message);
        // System.out.printf("%c, ", messageAsString.charAt(0));
        for (int i = 0; i < 8; i++) {
            if (message[i] != '0')
                return false;
        }

        return true;
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

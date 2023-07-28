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

        DatagramSocket socket = tryOpenSocket(source);

        long sendCount = 0;
        long totalBytesSend = 0;
        // DatagramPacket packet = new DatagramPacket(buffer, buffer.length,
        // destination, PORT);
        while (totalBytesSend < BYTES_IN_ONE_GIGABYTE) {
            if (buffer[0] == 'Z') {
                buffer[0] = 'A';
            } else {
                buffer[0]++;
            }

            if (BYTES_IN_ONE_GIGABYTE - totalBytesSend < buffer.length) {
                for (int j = 0; j < 8; j++) {
                    buffer[j] = '0';
                }
            }

            System.out.printf("%c, ", buffer[0]);

            DatagramPacket packet = new DatagramPacket(buffer, buffer.length, destination, PORT);
            trySendMessage(socket, packet);

            totalBytesSend += buffer.length;
            sendCount++;
        }

        byte[] response = new byte[50];
        DatagramPacket responsePacket = new DatagramPacket(response, response.length, destination, PORT);
        tryReceiveMessage(socket, responsePacket);

        String responseString = new String(responsePacket.getData());
        int dividerIndex = responseString.indexOf('#');
        System.out.println(responseString.substring(0, dividerIndex));
        System.out.println(responseString.substring(dividerIndex, responseString.length() - 1));
        float throughput = Float.parseFloat(responseString.substring(0, dividerIndex));
        long receiveCount = Long.parseLong(responseString.substring(dividerIndex + 2, responseString.length() - 1));
        float receive_success_percentage = (100.0f / sendCount) * receiveCount;

        save(args[2], throughput, receive_success_percentage);
    }

    static DatagramSocket tryOpenSocket(InetAddress source) {
        DatagramSocket socket = null;
        try {
            socket = new DatagramSocket(PORT, source);
        } catch (SocketException e) {
            System.out.println(e.getMessage());
            System.exit(-1);
        }

        return socket;
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

    static void save(String fileName, float throughput, float success_percentage) {
        try {
            FileWriter fileWriter = new FileWriter(fileName, true);
            BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);

            String result = String.format("%f, %f\n", throughput, success_percentage);
            bufferedWriter.append(result);
            bufferedWriter.close();
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
        }
    }

}

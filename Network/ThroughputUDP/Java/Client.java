import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
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
        DatagramPacket packet = new DatagramPacket(buffer, buffer.length, destination, PORT);
        while (totalBytesSend < BYTES_IN_ONE_GIGABYTE - buffer.length) {
            trySendMessage(socket, packet);

            totalBytesSend += buffer.length;
            sendCount++;
        }

        byte[] response = new byte[50];
        DatagramPacket responsePacket = new DatagramPacket(response, response.length, destination, PORT);

        int iteration = 1;
        boolean success = false;
        while (!success) {
            buffer[0] = (byte) (iteration + '0');
            trySendMessage(socket, packet);

            trySetTimeout(socket, 5000);
            success = tryReceiveMessage(socket, responsePacket);

            iteration++;
        }

        String responseString = new String(responsePacket.getData());
        int dividerIndex = responseString.indexOf('#');

        float throughput = Float.parseFloat(responseString.substring(0, dividerIndex));
        long receiveCount = Long.parseLong(responseString.substring(dividerIndex + 1, responsePacket.getLength()));
        float receive_success_percentage = (100.0f / sendCount) * receiveCount;

        save(args[2], throughput, receive_success_percentage);
    }

    static void trySetTimeout(DatagramSocket socket, int timeout) {
        try {
            socket.setSoTimeout(timeout);
        } catch (SocketException e) {
            System.out.println(e.getMessage());
            System.exit(-1);
        }
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

    static boolean tryReceiveMessage(DatagramSocket socket, DatagramPacket packet) {
        try {
            socket.receive(packet);
        } catch (SocketTimeoutException timeoutExeption) {
            return false;
        } catch (IOException e) {
            System.out.println(e.getMessage());
            System.exit(-1);
        }

        return true;
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

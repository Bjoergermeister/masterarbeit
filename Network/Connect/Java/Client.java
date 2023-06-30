import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

public class Client {

    public static final int PORT = 3000;
    public static final float NANOSECONDS_IN_ONE_MICROSECOND = 1000.0f;

    public static void main(String[] args) {
        InetAddress destination = null;
        try {
            destination = InetAddress.getByName(args[0]);
        } catch (UnknownHostException e) {
            System.out.println(String.format("UnknownHostException: %s", e.getMessage()));
            System.exit(-1);
        }

        long startTime = System.nanoTime();
        Socket socket = tryOpenSocket(destination);
        long endTime = System.nanoTime();
        tryCloseSocket(socket);

        float difference = (endTime - startTime) / NANOSECONDS_IN_ONE_MICROSECOND;

        save(args[1], difference);
    }

    static void tryCloseSocket(Socket socket) {
        try {
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(-1);
        }
    }

    static Socket tryOpenSocket(InetAddress address) {
        Socket socket = null;
        try {
            socket = new Socket(address, PORT);
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(-1);
        }
        return socket;
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
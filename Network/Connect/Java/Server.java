import java.io.*;
import java.net.*;

public class Server {

    public static final int PORT = 3000;

    public static void main(String[] args) {
        InetAddress source = null;
        try {
            source = InetAddress.getByName(args[0]);
        } catch (UnknownHostException e) {
            System.out.println(String.format("UnknownHostException: %s", e.getMessage()));
            System.exit(-1);
        }

        ServerSocket socket = tryOpenSocket(source);

        while (true) {
            tryAccept(socket);
        }
    }

    static Socket tryAccept(ServerSocket socket) {
        Socket acceptedSocket = null;
        try {
            acceptedSocket = socket.accept();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(-1);
        }
        return acceptedSocket;
    }

    static ServerSocket tryOpenSocket(InetAddress sourceAddress) {
        ServerSocket socket = null;
        try {
            socket = new ServerSocket(PORT, 3, sourceAddress);
        } catch (IOException e) {
            System.out.println(e.getMessage());
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
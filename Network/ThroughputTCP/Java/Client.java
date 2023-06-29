import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

public class Client {

    public static final int PORT = 3000;
    public static final int BYTES_IN_ONE_GIGABYTE = 1024 * 1024 * 1024;
    public static final int MESSAGE_SIZE = 1460;

    public static void main(String[] args) {
        char[] buffer = new char[MESSAGE_SIZE];
        for (int i = 0; i < buffer.length; i++) {
            buffer[i] = 'A';
        }

        InetAddress destination = null;
        try {
            destination = InetAddress.getByName(args[0]);
        } catch (UnknownHostException e) {
            System.out.println(String.format("UnknownHostException: %s", e.getMessage()));
            System.exit(-1);
        }

        Socket socket = tryOpenSocket(destination);
        BufferedWriter writer = tryGetOutputStream(socket);
        InputStream inputStream = tryGetInputStream(socket);

        long totalSendBytes = 0;
        while (totalSendBytes < BYTES_IN_ONE_GIGABYTE) {
            if (BYTES_IN_ONE_GIGABYTE - totalSendBytes <= MESSAGE_SIZE) {
                for (int i = 1; i < 9; i++) {
                    buffer[buffer.length - i] = '0';
                }
            }

            trySendMessage(writer, buffer);
            totalSendBytes += MESSAGE_SIZE;
        }

        byte[] responseBuffer = new byte[100];
        tryReadStream(inputStream, responseBuffer);
        String response = new String(responseBuffer);

        tryCloseSocket(socket);

        save(args[1], Float.parseFloat(response));
    }

    static void tryCloseSocket(Socket socket) {
        try {
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(-1);
        }
    }

    static void trySendMessage(BufferedWriter writer, char[] message) {
        try {
            writer.write(message);
            writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(-1);
        }
    }

    static int tryReadStream(InputStream stream, byte[] buffer) {
        int receivedBytes = 0;
        try {
            receivedBytes = stream.read(buffer);
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(-1);
        }

        return receivedBytes;
    }

    static InputStream tryGetInputStream(Socket socket) {
        InputStream stream = null;
        try {
            stream = socket.getInputStream();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(-1);
        }
        return stream;
    }

    static BufferedWriter tryGetOutputStream(Socket socket) {
        BufferedWriter writer = null;
        OutputStream stream;
        try {
            stream = socket.getOutputStream();
            writer = new BufferedWriter(new OutputStreamWriter(stream));
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(-1);
        }
        return writer;
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
import java.io.*;
import java.net.*;

public class Server {

    public static final int PORT = 3000;
    public static final float NANOSECONDS_IN_ONE_SECOND = 1000000000.0f;
    public static final float BYTES_IN_ONE_MEGABYTE = (float) (1024 * 1024);

    public static void main(String[] args) {
        InetAddress source = null;
        try {
            source = InetAddress.getByName(args[0]);
        } catch (UnknownHostException e) {
            System.out.println(String.format("UnknownHostException: %s", e.getMessage()));
            System.exit(-1);
        }

        ServerSocket socket = tryOpenSocket(source);

        byte[] buffer = new byte[1460];
        while (true) {
            Socket acceptedSocket = tryAccept(socket);
            InputStream inputStream = tryGetInputStream(acceptedSocket);
            OutputStream outputStream = tryGetOutputStream(acceptedSocket);

            long totalReceivedBytes = 0;
            long startTime = 0;
            long endTime = 0;
            int receivedBytes = 0;
            boolean isFirstPacket = true;
            do {
                receivedBytes = tryReadStream(inputStream, buffer);

                if (isFirstPacket) {
                    startTime = System.nanoTime();
                    isFirstPacket = false;
                }

                totalReceivedBytes += receivedBytes;
            } while (hasEndFlag(buffer, receivedBytes) == false);

            endTime = System.nanoTime();

            float timeDifferenceInSeconds = (endTime - startTime) / NANOSECONDS_IN_ONE_SECOND;
            float megabytes = totalReceivedBytes / BYTES_IN_ONE_MEGABYTE;
            float throughput = megabytes / timeDifferenceInSeconds;

            System.out.printf("Received %d bytes in %f seconds (%f mb/s)\n", totalReceivedBytes,
                    timeDifferenceInSeconds, throughput);
            String response = String.format("%f", throughput);
            trySendMessage(outputStream, response.getBytes());
        }
    }

    static boolean hasEndFlag(byte[] buffer, int receivedBytes) {
        for (int i = 1; i < 9; i++) {
            if (buffer[receivedBytes - i] != (byte) '0')
                return false;
        }
        return true;
    }

    static void trySendMessage(OutputStream stream, byte[] buffer) {
        try {
            stream.write(buffer);
            stream.flush();
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

    static OutputStream tryGetOutputStream(Socket socket) {
        OutputStream stream = null;
        try {
            stream = socket.getOutputStream();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(-1);
        }
        return stream;
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

    static String tryReadLine(BufferedReader reader) {
        String result = null;
        try {
            result = reader.readLine();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(-1);
        }
        return result;
    }

    static void tryCloseSocket(Socket socket) {
        try {
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(-1);
        }
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
        for (int i = 0; i < 8; i++) {
            if (message[i] != '\0')
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
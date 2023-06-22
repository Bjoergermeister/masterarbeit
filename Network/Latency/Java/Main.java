import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.UnknownHostException;

public class Main {

    public static void main(String[] args) {
        String saveFileName = args[2];

        InetAddress source = null;
        InetAddress destination = null;

        try {
            source = InetAddress.getByName(args[0]);
            destination = InetAddress.getByName(args[1]);
        } catch (UnknownHostException e) {
            System.out.println(String.format("UnknownHostException: %s", e.getMessage()));
            System.exit(-1);
        }

        try {
            for (int i = 0; i < 100; i++) {
                NetworkInterface networkInterface = NetworkInterface.getByInetAddress(source);

                long start = System.nanoTime();
                boolean success = destination.isReachable(networkInterface, 255, 1000);
                long end = System.nanoTime();

                if (success == false) {
                    System.out.println("Ping not successful");
                    System.exit(-1);
                }

                float result = (float) (end - start) / 1000.0f;
                save(saveFileName, result);

                try {
                    Thread.sleep(1000);
                } catch (Exception e) {

                }
            }
        } catch (SocketException e) {
            System.out.println(String.format("SocketException: %s", e.getMessage()));
            System.exit(-1);
        } catch (IOException e) {
            System.out.println(String.format("IOException: %s", e.getMessage()));
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
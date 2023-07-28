import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.RandomAccessFile;

public class Main {

    public static void main(String[] args) {
        if (args.length < 2) {
            System.out.println("Zu wenig Argumente!");
            System.exit(-1);
        }

        String writeFile = args[0];
        String saveFile = args[1];

        byte[] buffer = new byte[4096];
        for (int i = 0; i < 4096; i++) {
            buffer[i] = 'A';
        }

        try {
            RandomAccessFile file = new RandomAccessFile(writeFile, "rw");

            long result1 = writeAndMeasure(file, buffer);
            long result2 = writeAndMeasure(file, buffer);

            save(saveFile, result1, result2);
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
        }
    }

    static long writeAndMeasure(RandomAccessFile file, byte[] buffer) {
        try {
            file.seek(0);
            long start = System.nanoTime();
            file.write(buffer);
            long end = System.nanoTime();
            return (end - start) / 1000; // Convert to microseconds
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
            System.exit(-1);
        }
        return 0;
    }

    static void save(String saveFile, long result1, long result2) {
        try {
            FileWriter fileWriter = new FileWriter(saveFile, true);
            BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);

            bufferedWriter.append(Long.toString(result1) + ", " + Long.toString(result2) + "\n");
            bufferedWriter.close();
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
        }
    }
}

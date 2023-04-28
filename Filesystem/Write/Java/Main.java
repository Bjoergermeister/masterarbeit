import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;

public class Main {

    public static void main(String[] args) {
        if (args.length < 2) {
            System.out.println("Zu wenig Argumente!");
            System.exit(-1);
        }

        String writeFile = args[0];
        String saveFile = args[1];
        try {
            FileWriter writer = new FileWriter(writeFile, true);

            long result1 = writeAndMeasure(writer);
            long result2 = writeAndMeasure(writer);

            save(saveFile, result1, result2);
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
        }
    }

    static long writeAndMeasure(FileWriter writer) {
        try {
            long start = System.nanoTime();
            writer.append('a');
            long end = System.nanoTime();
            return end - start;
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
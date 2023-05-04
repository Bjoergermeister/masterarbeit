import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class Main {

    public static void main(String[] args) {
        if (args.length < 2) {
            System.out.println("Zu wenig Argumente!");
            System.exit(-1);
        }

        String readFile = args[0];
        String saveFile = args[1];

        FileReader fileReader;
        try {
            File file = new File(readFile);
            long length = file.length();
            fileReader = new FileReader(args[0]);

            char buffer[] = new char[(int) length];

            long start = System.nanoTime();
            fileReader.read(buffer, 0, (int) length);
            long end = System.nanoTime();

            long result = (end - start) / 1000; // Convert to microseconds
            save(saveFile, result);
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
        }
    }

    public static void save(String saveFile, long result) {
        try {
            FileWriter fileWriter = new FileWriter(saveFile, true);
            BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);

            bufferedWriter.append(Long.toString(result) + "\n");
            bufferedWriter.close();
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
        }
    }
}
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;

public class Main {

    public static void main(String[] args) {
        if (args.length < 2) {
            System.out.println("Zu wenig Argumente!");
            System.exit(-1);
        }

        String openFile = args[0];
        String saveFile = args[1];

        File file; // Create a file instance here so that the lib is already loaded before time is
                   // measured

        long start = System.nanoTime();
        file = new File(openFile);
        long end = System.nanoTime();

        long result = (end - start) / 1000; // Convert to microseconds
        save(saveFile, result);
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
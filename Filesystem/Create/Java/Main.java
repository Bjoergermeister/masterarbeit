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

        String newFile = args[0];
        String saveFile = args[1];

        File file = new File(newFile);

        try {
            long start = System.nanoTime();
            file.createNewFile();
            long end = System.nanoTime();

            long result = end - start;
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
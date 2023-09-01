import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class Main {

    static final int ARRAY_SIZE = 1024 * 1024 * 10;

    public static void main(String[] args) {
        FileReader reader = tryGetFile("array");
        Map<Integer, Integer> count = new HashMap<Integer, Integer>();

        long start = System.nanoTime();
        for (int i = 0; i < 10; i++) {
            char[] array = new char[ARRAY_SIZE];
            try {
                reader.read(array, 0, array.length);
            } catch (IOException e) {
                e.printStackTrace();
            }

            for (int j = 0; j < array.length; j++) {
                int number = (int) array[j];
                int currentValue = (count.containsKey(number)) ? count.get(number) : 0;
                count.put(number, currentValue + 1);
            }
        }

        long end = System.nanoTime();
        long difference = (end - start) / 1000000;
        System.out.println(difference);

        try {
            reader.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    static FileReader tryGetFile(String filename) {
        try {
            return new FileReader(new File(filename));
        } catch (FileNotFoundException ex) {
            System.out.println(ex.getLocalizedMessage());
            System.exit(-1);
        }
        return null;
    }
}

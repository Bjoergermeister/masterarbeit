import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;

public class Main {

    private static final int PAGE_SIZE = 4096;
    private static final long THREE_HUNDRED_MEGABYTES = 1024 * 1024 * 300;

    public static void main(String[] args) throws NoSuchFieldException, IllegalAccessException {
        String filePrefix = args[0];

        long total_allocated_memory = 0;
        long allocationCounter = 0;
        long timeSum = 0;
        ArrayList<byte[]> arrays = new ArrayList<byte[]>();

        while (total_allocated_memory < THREE_HUNDRED_MEGABYTES) {
            long start = System.nanoTime();
            byte[] array = new byte[PAGE_SIZE];
            long end = System.nanoTime();

            // Do something with the array so the compiler doesn't optimize it away
            for (int i = 0; i < 4096; i++) {
                array[i] = 1;
            }

            timeSum += (end - start);

            total_allocated_memory += 4096;
            allocationCounter++;
            arrays.add(array);

            if (allocationCounter % 600 == 0 && allocationCounter > 0) {
                save(
                        filePrefix,
                        allocationCounter / 600,
                        timeSum / 600);
                timeSum = 0;
            
            }

        }
    }

    public static void save(String prefix, long count, long result) {
        String fileName = String.format("%s_%d.txt", prefix, count);
        try {
            FileWriter fileWriter = new FileWriter(fileName, true);
            BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);

            bufferedWriter.append(Long.toString(result) + "\n");
            bufferedWriter.close();
        } catch (IOException ex) {
            System.out.println(ex.getMessage());
        }
    }
}
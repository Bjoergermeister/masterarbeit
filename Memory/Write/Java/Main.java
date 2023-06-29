import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;

public class Main {

    private static final int PAGE_SIZE = 4096;
    private static final long THREE_HUNDRED_MEGABYTES = 1024 * 1024 * 300;
    private static final long NANOSECONDS_IN_ONE_MICROSECOND = 1000;

    volatile static byte[] array;

    public static void main(String[] args) {
        String filePrefix = args[0];

        long total_allocated_memory = 0;
        long allocationCounter = 0;
        float timeSum = 0;

        while (total_allocated_memory < THREE_HUNDRED_MEGABYTES) {
            array = new byte[PAGE_SIZE];

            long start = System.nanoTime();
            for (int i = 0; i < 4096; i++) {
                array[i] = 1;
            }
            long end = System.nanoTime();

            timeSum += (end - start) / (float) NANOSECONDS_IN_ONE_MICROSECOND;

            total_allocated_memory += 4096;
            allocationCounter++;

            if (allocationCounter % 600 == 0 && allocationCounter > 0) {
                save(
                        filePrefix,
                        allocationCounter / 600,
                        timeSum / 600);
                timeSum = 0;
            }
        }
    }

    public static void save(String prefix, long count, float result) {
        String fileName = String.format("%s_%d.txt", prefix, count);
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
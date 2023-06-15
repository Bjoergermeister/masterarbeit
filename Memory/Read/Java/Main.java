import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;

public class Main {

    private static final int PAGE_SIZE = 4096;
    private static final long THREE_HUNDRED_MEGABYTES = 1024 * 1024 * 300;
    private static final long NANOSECONDS_IN_ONE_MICROSECOND = 1000;
    private static final ArrayList<byte[]> ARRAYS = new ArrayList<byte[]>();

    public static void main(String[] args) throws NoSuchFieldException, IllegalAccessException {
        String filePrefix = args[0];

        long timeSum = 0;

        allocate_and_fill();

        for (int i = ARRAYS.size() - 1; i >= 0; i--) {
            int sum = 0;
            long start = System.nanoTime();
            byte[] array = ARRAYS.get(i);
            for (int j = 0; j < PAGE_SIZE; j++) {
                sum += array[j];
            }
            long end = System.nanoTime();

            timeSum += (end - start) / NANOSECONDS_IN_ONE_MICROSECOND;

            if (i % 600 == 0) {
                long count = 128 - (i / 600);
                save(
                        filePrefix,
                        count,
                        timeSum / 600);
                timeSum = 0;
            }
        }
    }

    static void allocate_and_fill() {
        long total_allocated_memory = 0;
        while (total_allocated_memory < THREE_HUNDRED_MEGABYTES) {
            byte[] array = new byte[PAGE_SIZE];

            for (int i = 0; i < 4096; i++) {
                array[i] = 1;
            }

            ARRAYS.add(array);
            total_allocated_memory += PAGE_SIZE;
        }
    }

    static void save(String prefix, long count, long result) {
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
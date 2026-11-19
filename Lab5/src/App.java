import java.io.IOException;
import java.util.List;
import java.util.Scanner;

public class App {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        int maxWidth = 0;

        while (true) {
            System.out.print("Введите ширину строки: ");
            if (scanner.hasNextInt()) {
                maxWidth = scanner.nextInt();
                if (maxWidth > 0) {
                    break;
                }
                System.out.println("Ошибка: ширина должна быть больше нуля.");
            } else {
                System.out.println("Ошибка: введите корректное целое число.");
                scanner.next();
            }
        }

        FileHandler fileHandler = new FileHandler("input.txt", "output.txt");
        TextForm formatter = new TextForm();

        try {
            List<String> lines = fileHandler.readLines();
            String result = formatter.buildString(lines, maxWidth);
            fileHandler.writeText(result);
            System.out.println("Результат записан в output.txt");
        } catch (IOException e) {
            System.err.println("Ошибка при работе с файлами: " + e.getMessage());
        } finally {
            scanner.close();
        }
    }
}

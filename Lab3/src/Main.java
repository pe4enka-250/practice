import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;

public class Main {
    public static void main(String[] args) {
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        StringBuilder textBuilder = new StringBuilder();

        System.out.print("Введите текст: ");
        try {
            while (true) {
                String line = br.readLine();
                if (line == null || line.isEmpty()) {
                    break;
                }
                textBuilder.append(line).append(" ");
            }
        } catch (IOException e) {
            System.out.println("Ошибка чтения с клавиатуры");
            return;
        }

        SentenceAnalyzer analyzer = new SentenceAnalyzer();
        analyzer.analyze(textBuilder.toString());
    }
}
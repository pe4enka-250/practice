public class SentenceAnalyzer {
    public void analyze(String text) {
        String[] sentences = text.split("[.!?]+");
        String vowels = "аеёиоуыэюяАЕЁИОУЫЭЮЯaeiouyAEIOUY";

        for (String sentence : sentences) {
            String trimmedSentence = sentence.trim();
            if (trimmedSentence.isEmpty()) {
                continue;
            }

            int vowelsCount = 0;
            int consonantsCount = 0;

            for (char c : trimmedSentence.toCharArray()) {
                if (Character.isLetter(c)) {
                    if (vowels.indexOf(c) != -1) {
                        vowelsCount++;
                    } else {
                        consonantsCount++;
                    }
                }
            }

            System.out.println(trimmedSentence);
            if (vowelsCount > consonantsCount) {
                System.out.println("Гласных больше: " + vowelsCount + " (Согласных: " + consonantsCount + ")\n");
            } else if (consonantsCount > vowelsCount) {
                System.out.println("Согласных больше: " + consonantsCount + " (Гласных: " + vowelsCount + ")\n");
            } else {
                System.out.println("Гласных и согласных поровну: по " + vowelsCount + "\n");
            }
        }
    }
}
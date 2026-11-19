import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class TextForm {

    public String buildString(List<String> fileLines, int width) {
        List<String> resultLines = new ArrayList<>();
        List<String> wordBuffer = new ArrayList<>(); // Наш динамический массив слов
        String currentIndent = "";
        boolean isFirstLineOfParagraph = true;

        for (String line : fileLines) {
            boolean hasIndent = !line.isEmpty() && Character.isWhitespace(line.charAt(0));
            if (line.trim().isEmpty() || hasIndent) {
                if (!wordBuffer.isEmpty()) {
                    resultLines.addAll(flushBuffer(wordBuffer, width, currentIndent, isFirstLineOfParagraph, true));
                    wordBuffer.clear();
                }
                if (line.trim().isEmpty()) {
                    resultLines.add("");
                    continue;
                }
                currentIndent = extractIndent(line);
                isFirstLineOfParagraph = true;
            }

            String[] newWords = line.trim().split("\\s+");
            wordBuffer.addAll(Arrays.asList(newWords));

            List<String> readyLines = flushBuffer(wordBuffer, width, currentIndent, isFirstLineOfParagraph, false);
            if (!readyLines.isEmpty()) {
                resultLines.addAll(readyLines);
                isFirstLineOfParagraph = false;
                currentIndent = "";
            }
        }

        if (!wordBuffer.isEmpty()) {
            resultLines.addAll(flushBuffer(wordBuffer, width, currentIndent, isFirstLineOfParagraph, true));
        }

        return String.join(System.lineSeparator(), resultLines);
    }

    private List<String> flushBuffer(List<String> buffer, int width, String indent, boolean isFirstLine, boolean isEndOfParagraph) {
        List<String> lines = new ArrayList<>();

        while (!buffer.isEmpty()) {
            String activeIndent = isFirstLine ? indent : "";
            int currentWidth = activeIndent.length() + buffer.get(0).length();
            int count = 1; 

            while (count < buffer.size()) {
                if (currentWidth + 1 + buffer.get(count).length() > width) {
                    break;
                }
                currentWidth += 1 + buffer.get(count).length();
                count++;
            }

            if (count == buffer.size() && !isEndOfParagraph) {
                break;
            }

            StringBuilder sb = new StringBuilder(activeIndent);
            int gaps = count - 1;

            if (count == buffer.size() || gaps == 0) {
                for (int i = 0; i < count; i++) {
                    sb.append(buffer.get(i));
                    if (i < count - 1) sb.append(" ");
                }
            } else {
                int wordsChars = activeIndent.length();
                for (int i = 0; i < count; i++) {
                    wordsChars += buffer.get(i).length();
                }
                int totalSpaces = width - wordsChars;
                int spacesPerGap = totalSpaces / gaps;
                int extraSpaces = totalSpaces % gaps;

                for (int i = 0; i < count; i++) {
                    sb.append(buffer.get(i));
                    if (i < count - 1) {
                        int spaces = spacesPerGap + (i < extraSpaces ? 1 : 0);
                        sb.append(" ".repeat(spaces));
                    }
                }
            }

            lines.add(sb.toString());
            buffer.subList(0, count).clear();
            isFirstLine = false;
        }

        return lines;
    }

    private String extractIndent(String line) {
        int i = 0;
        while (i < line.length() && Character.isWhitespace(line.charAt(i))) {
            i++;
        }
        return line.substring(0, i).replace("\t", "    ");
    }
}

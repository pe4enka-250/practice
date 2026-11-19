import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;

public class FileHandler {
    private final Path inputPath;
    private final Path outputPath;

    public FileHandler(String inputFileName, String outputFileName) {
        this.inputPath = Path.of(inputFileName);
        this.outputPath = Path.of(outputFileName);
    }

    public List<String> readLines() throws IOException {
        return Files.readAllLines(inputPath, StandardCharsets.UTF_8);
    }

    public void writeText(String content) throws IOException {
        Files.writeString(outputPath, content, StandardCharsets.UTF_8);
    }
}

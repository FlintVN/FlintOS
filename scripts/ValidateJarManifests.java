import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.jar.Attributes;
import java.util.jar.JarFile;
import java.util.stream.Stream;

public final class ValidateJarManifests {
    private static final String[] REQUIRED = {
        "Specification-Title", "Specification-Version", "Specification-Vendor",
        "Implementation-Title", "Implementation-Version", "Implementation-Vendor"
    };

    public static void main(String[] args) throws Exception {
        if (args.length == 0) throw new IllegalArgumentException("Pass at least one JAR file or directory");
        List<Path> jars = new ArrayList<>();
        for (String arg : args) {
            Path path = Path.of(arg);
            if (Files.isDirectory(path)) {
                try (Stream<Path> files = Files.walk(path)) {
                    files.filter(Files::isRegularFile)
                        .filter(file -> file.getFileName().toString().toLowerCase().endsWith(".jar"))
                        .forEach(jars::add);
                }
            } else if (arg.toLowerCase().endsWith(".jar")) {
                jars.add(path);
            }
        }
        if (jars.isEmpty()) throw new IllegalStateException("No JAR files found");
        List<String> failures = new ArrayList<>();
        for (Path jar : jars.stream().sorted().toList()) validate(jar, failures);
        if (!failures.isEmpty()) {
            failures.forEach(System.err::println);
            throw new IllegalStateException(failures.size() + " manifest validation error(s)");
        }
        System.out.println("Validated " + jars.size() + " JAR manifest(s).");
    }

    private static void validate(Path path, List<String> failures) {
        try (JarFile jar = new JarFile(path.toFile())) {
            var entry = jar.getJarEntry(JarFile.MANIFEST_NAME);
            if (entry == null || jar.getManifest() == null) {
                failures.add(path + ": missing META-INF/MANIFEST.MF");
                return;
            }
            byte[] raw;
            try (var input = jar.getInputStream(entry)) { raw = input.readAllBytes(); }
            String text = new String(raw, StandardCharsets.UTF_8);
            if (!text.startsWith("Manifest-Version: 1.0\r\n"))
                failures.add(path + ": Manifest-Version: 1.0 must be the first line");
            if (!text.endsWith("\r\n\r\n"))
                failures.add(path + ": main section must end with a blank CRLF line");
            Attributes attributes = jar.getManifest().getMainAttributes();
            for (String name : REQUIRED) {
                String value = attributes.getValue(name);
                if (value == null || value.isBlank()) failures.add(path + ": missing " + name);
            }
        } catch (IOException exception) {
            failures.add(path + ": unreadable JAR: " + exception.getMessage());
        }
    }
}

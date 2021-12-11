package utils.ssl;

import java.io.*;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.attribute.FileAttribute;
import java.nio.file.attribute.PosixFilePermission;
import java.nio.file.attribute.PosixFilePermissions;
import java.security.KeyStore;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.util.Set;

public class SSLCertificate {
    private static final Path certRootDir = Path.of("certs");
    /**
     * The method downloads the SSL Certificate for Amazon RDS
     * for the specified region
     *
     * @param region the region (ex: us-east-1).
     * @return the full path of the certificate as string.
     * @throws IOException
     * */
    public static String downloadsForRds(String region) throws IOException {

        if (!Files.exists(certRootDir)) {
//            Files.createDirectory(Path.of("certs"));
            Runtime.getRuntime().exec("mkdir " + certRootDir);
            Runtime.getRuntime().exec("chmod g+w " + certRootDir);
            Runtime.getRuntime().exec("chmod o+w " + certRootDir);
        }

        String RDS_CERTIFICATE_URL = "https://truststore.pki.rds.amazonaws.com/" + region + "/" + region +"-bundle.pem";
        String RDS_CERTIFICATE_PATH = certRootDir + File.separator + "rds" + region + ".pem";

        File file = new File(RDS_CERTIFICATE_PATH);
        file.createNewFile();

        try (BufferedInputStream in = new BufferedInputStream(new URL(RDS_CERTIFICATE_URL).openStream())){
            FileOutputStream out = new FileOutputStream(file);

            byte[] dataBuffer = new byte[1024];
            int bytesRead;
            while ((bytesRead = in.read(dataBuffer, 0, 1024)) != -1) {
                out.write(dataBuffer, 0, bytesRead);
            }

        }
        return file.getAbsolutePath();
    }

    /**
     * This method generates the SSL Certificate
     *
     * @param certificate_path String
     * @return X509Certificate object
     * @throws Exception
     * */
    public static X509Certificate createCertificate(String certificate_path) throws Exception {
        CertificateFactory certFactory = CertificateFactory.getInstance("X.509");
        URL url = new File(certificate_path).toURI().toURL();
        if (url == null) {
            throw new Exception();
        }
        try (InputStream certInputStream = url.openStream()) {
            return (X509Certificate) certFactory.generateCertificate(certInputStream);
        }
    }

    /**
     * This method creates the Key Store File
     * @param rootX509Certificate - the SSL certificate to be stored in the KeyStore
     * @return File object
     * @throws Exception
     */
    public static File createKeyStoreFile(X509Certificate rootX509Certificate, String password) throws Exception {
        File keyStoreFile = File.createTempFile(certRootDir + File.separator
                + "sys-connect-via-ssl-test-cacerts", ".jks");
        try (FileOutputStream fos = new FileOutputStream(keyStoreFile.getPath())) {
            KeyStore ks = KeyStore.getInstance("JKS", "SUN");
            ks.load(null);
            ks.setCertificateEntry("rootCaCertificate", rootX509Certificate);
            ks.store(fos, password.toCharArray());
        }
        return keyStoreFile;
    }
}

package db;

import utils.ssl.SSLCertificate;

import java.sql.Connection;
import java.sql.DriverManager;
import java.util.Properties;

public class IAMDatabase implements SQLDb, SSLCert {

    // Configuration
    private final StringBuilder JDBC_URL = new StringBuilder();
    private final String REGION;
    private final String USERNAME;
    private final RDSAuthToken rdsAuthToken;

    private final String DEFAULT_KEY_STORE_PASSWORD;

    public IAMDatabase(String db_instance_hostname,
                       int db_instance_port, String region, String username,
                       RDSAuthToken rdsAuthToken, String default_key_store_password) {
        REGION = region;
        USERNAME = username;
        this.rdsAuthToken = rdsAuthToken;
        DEFAULT_KEY_STORE_PASSWORD = default_key_store_password;

        JDBC_URL.append("jdbc:mysql://");
        JDBC_URL.append(db_instance_hostname);
        JDBC_URL.append(":");
        JDBC_URL.append(db_instance_port);
    }

    @Override
    public Connection getDBConnection() throws Exception {
        setSslProperties();
        return DriverManager.getConnection(JDBC_URL.toString(), setMySqlConnectionProperties());
    }

    /**
     * This method clears the SSL properties.
     * @throws Exception
     */
    @Override
    public void clearSslProperties() throws Exception {
        System.clearProperty("javax.net.ssl.trustStore");
        System.clearProperty("javax.net.ssl.trustStoreType");
        System.clearProperty("javax.net.ssl.trustStorePassword");
    }

    /**
     * This method sets the mysql connection properties which includes the IAM Database Authentication token
     * as the password. It also specifies that SSL verification is required.
     * @return
     */

    private Properties setMySqlConnectionProperties() {
        Properties mysqlConnectionProperties = new Properties();
        mysqlConnectionProperties.setProperty("verifyServerCertificate","true");
        mysqlConnectionProperties.setProperty("useSSL", "true");
        mysqlConnectionProperties.setProperty("user", USERNAME);
        mysqlConnectionProperties.setProperty("password", rdsAuthToken.generateToken());
        return mysqlConnectionProperties;
    }

    /**
     * This method sets the SSL properties which specify the key store file, its type and password:
     * @throws Exception
     */
    private void setSslProperties() throws Exception {
        System.setProperty("javax.net.ssl.trustStore", SSLCertificate.
                createKeyStoreFile(SSLCertificate
                                .createCertificate(SSLCertificate.downloadsForRds(REGION)),
                        DEFAULT_KEY_STORE_PASSWORD)
                .getPath());
        System.setProperty("javax.net.ssl.trustStoreType", "JKS");
        System.setProperty("javax.net.ssl.trustStorePassword", DEFAULT_KEY_STORE_PASSWORD);
    }
}

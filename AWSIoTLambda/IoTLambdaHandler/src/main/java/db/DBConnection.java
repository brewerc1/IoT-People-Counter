package db;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;

public class DBConnection implements SQLDb {
    private final DBSecret dbSecret;
    private final StringBuilder jdbc = new StringBuilder();
    private Connection connection;

    public DBConnection(final DBSecret dbSecret) {
        this.dbSecret = dbSecret;
        jdbc.append("jdbc:mysql://");
        jdbc.append(dbSecret.getDbHostName());
        jdbc.append(":");
        jdbc.append(dbSecret.getDbPort());
    }
    @Override
    public Connection getDBConnection() throws SQLException {
        if (connection != null && !connection.isClosed())
            connection = DriverManager.getConnection(jdbc.toString(), dbSecret.getDbUsername(), dbSecret.getDbPassword());
        return connection;
    }
}

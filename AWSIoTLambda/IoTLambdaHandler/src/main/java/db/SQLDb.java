package db;

import java.sql.Connection;
import java.sql.SQLException;

public interface SQLDb {
    Connection getDBConnection() throws SQLException;
}

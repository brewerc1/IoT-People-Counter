package db;

import java.sql.Connection;

public interface SQLDb {
    Connection getDBConnection() throws Exception;
}

package utils.db;

import db.DBConnection;
import db.DBSecret;
import db.SQLDb;

import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.SQLException;

public class DBUtil {
    private static final DBSecret dbSecret = new DBSecret(System.getenv("region"), System.getenv("dbInstanceSecretArn"));
    private static SQLDb db;

    public static SQLDb getDb() {
        return new DBConnection(dbSecret);
    }

    public static String getDefaultDatabaseName() {
        return dbSecret.getDbName();
    }

    public static boolean sqlUseDefaultDatabaseName(Connection connection) throws SQLException {
        String sql = "USE " + getDefaultDatabaseName() + ";";
        PreparedStatement stmt = connection.prepareStatement(sql);
        return stmt.execute(sql);
    }

    public static int sqlCreateTablesIfNotExist(Connection connection) throws SQLException {
        StringBuilder sqlStatement = new StringBuilder();
        int rawCount = 0;

        // peopleCounter table
        sqlStatement.append("CREATE TABLE IF NOT EXISTS peopleCounter ( ");
        sqlStatement.append("id int(11) NOT NULL AUTO_INCREMENT PRIMARY KEY, ");
        sqlStatement.append("device int(11) NOT NULL, ");
        sqlStatement.append("total int(11) NOT NULL, ");
        sqlStatement.append("timestamp timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP");
        sqlStatement.append(")");
        PreparedStatement stmt = connection.prepareStatement(sqlStatement.toString());
        rawCount += stmt.executeUpdate();

        // TODO: create location table

        // TODO: create device table

        return rawCount;
    }
}

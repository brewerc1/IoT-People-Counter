package handler;

import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.LambdaLogger;
import com.amazonaws.services.lambda.runtime.RequestHandler;
import com.amazonaws.services.lambda.runtime.events.SQSBatchResponse;
import com.amazonaws.services.lambda.runtime.events.SQSEvent;
import com.amazonaws.services.lambda.runtime.events.SQSEvent.SQSMessage;
import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonSyntaxException;
import db.DBConnection;
import db.DBSecret;
import db.SQLDb;

import java.sql.*;
import java.util.ArrayList;
import java.util.List;

// Handler value: handler.Handler
public class Handler implements RequestHandler<SQSEvent, SQSBatchResponse>{
  private Connection conn;
  private PreparedStatement stmt;
  @Override
  public SQSBatchResponse handleRequest(SQSEvent sqsEvent, Context context) {
    List<SQSBatchResponse.BatchItemFailure> batchItemFailures = new ArrayList<>();
    LambdaLogger logger = context.getLogger();

    // Creating database object
    String region = System.getenv("region");
    String secretArn = System.getenv("dbInstanceSecretArn");
    DBSecret dbSecret = new DBSecret(region, secretArn);
    SQLDb db = new DBConnection(dbSecret);

    try {
      // Connect to DB
      logger.log("Connecting to db");
      conn = db.getDBConnection();
      logger.log("Connected to db");

      // Select Database
      String dbName = dbSecret.getDbName();
      logger.log("USE " + dbSecret.getDbName() + ";");
      String sql = "USE " + dbName + ";";
      stmt = conn.prepareStatement(sql);
      stmt.execute(sql);
      logger.log("Database " + dbName + " Selected");

      // Create tables if not exist
      StringBuilder sqlStatement = new StringBuilder();
      sqlStatement.append("CREATE TABLE IF NOT EXISTS peopleCounter ( ");
      sqlStatement.append("id int(11) NOT NULL AUTO_INCREMENT PRIMARY KEY, ");
      sqlStatement.append("device int(11) NOT NULL, ");
      sqlStatement.append("total int(11) NOT NULL, ");
      sqlStatement.append("timestamp timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP");
      sqlStatement.append(")");
      stmt = conn.prepareStatement(sqlStatement.toString());
      if (stmt.executeUpdate() > 0) logger.log("peopleCounter table created: " + sqlStatement);

      // Insert into the db
      int deviceId;
      int total;
      String messageId = "";
      for (SQSMessage message: sqsEvent.getRecords()) {
        try {
          logger.log("Processing: " + message.getBody());
          messageId = message.getMessageId();

          JsonObject sqsMessage = new Gson().fromJson(message.getBody(), JsonObject.class);
          if (sqsMessage.has("device") && sqsMessage.has("total")) {

            deviceId = sqsMessage.get("device").getAsInt();
            total = sqsMessage.get("total").getAsInt();

            sql = "INSERT INTO peopleCounter(device, total) VALUES(?,?)";
            stmt = conn.prepareStatement(sql);
            stmt.setInt(1, deviceId);
            stmt.setInt(2, total);

            if (stmt.executeUpdate() < 1) logger.log("Insertion failed.");
            else logger.log("Insertion succeeded");
          }
        } catch (JsonSyntaxException e) {
          // If JSON Syntax Error, message will be ignored and treated as processed
          logger.log(e.getMessage());
        } catch (SQLException e) {
          // If SQL Error, the id of the message is returned, so it can be processed later.
          batchItemFailures.add(new SQSBatchResponse.BatchItemFailure(messageId));
          logger.log(e.getMessage());
        }
      }

      // TODO: Only for testing if data are in table
      String select = "SELECT * FROM peopleCounter;";
      stmt = conn.prepareStatement(select);
      ResultSet res = stmt.executeQuery();
      StringBuilder sb = new StringBuilder();
      while (res.next()) {
        sb.append("{\n");
        sb.append("id: ").append(res.getString(1)).append(",\n");
        sb.append("device: ").append(res.getString(2)).append(",\n");
        sb.append("total: ").append(res.getString(3)).append(",\n");
        sb.append("timestamp: ").append(res.getTimestamp(4));
        sb.append("\n}\n");
      }
      logger.log(sb.toString());
      // TODO: End testing

    } catch (SQLException e) {
      e.printStackTrace();
    }
    finally {
      try {
        conn.close();
      } catch (SQLException e) {
        e.printStackTrace();
      }
    }
    return new SQSBatchResponse(batchItemFailures);
  }
}
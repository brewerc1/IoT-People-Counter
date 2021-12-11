package handler;

import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.LambdaLogger;
import com.amazonaws.services.lambda.runtime.RequestHandler;
import com.amazonaws.services.lambda.runtime.events.SQSEvent;
import com.amazonaws.services.lambda.runtime.events.SQSEvent.SQSMessage;
import db.DBConnection;
import db.DBSecret;
import db.SQLDb;

import java.sql.Connection;
import java.sql.SQLException;
import java.util.TreeSet;

// Handler value: handler.Handler
public class Handler implements RequestHandler<SQSEvent, Void>{
  private final TreeSet<Integer> processedMessagesHashCodes = new TreeSet<>();
  private Connection conn;
  @Override
  public Void handleRequest(SQSEvent sqsEvent, Context context) {
    LambdaLogger logger = context.getLogger();

    String region = System.getenv("region");
    String secretArn = System.getenv("dbInstanceSecretArn");
    DBSecret dbSecret = new DBSecret(region, secretArn);
    SQLDb db = new DBConnection(dbSecret);

    try {
      conn = db.getDBConnection();
      logger.log("Connected to db");
    } catch (SQLException e) {
      logger.log("Failed to connect to db");
      e.printStackTrace();
    }

    // Test SQS
    for (SQSMessage message: sqsEvent.getRecords()) {
      if (processedMessagesHashCodes.contains(message.hashCode()))
        continue;
      processedMessagesHashCodes.add(message.hashCode());
      logger.log(message.getBody());
    }
    return null;
  }
}
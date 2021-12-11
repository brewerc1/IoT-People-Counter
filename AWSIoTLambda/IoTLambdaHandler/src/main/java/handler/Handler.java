package handler;

import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.LambdaLogger;
import com.amazonaws.services.lambda.runtime.RequestHandler;
import com.amazonaws.services.lambda.runtime.events.SQSEvent;
import com.amazonaws.services.lambda.runtime.events.SQSEvent.SQSMessage;
import com.amazonaws.services.secretsmanager.AWSSecretsManager;
import com.amazonaws.services.secretsmanager.AWSSecretsManagerClientBuilder;
import com.amazonaws.services.secretsmanager.model.GetSecretValueRequest;
import com.amazonaws.services.secretsmanager.model.GetSecretValueResult;
import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import db.IAMDatabase;
import db.RDSAuthToken;
import db.RDSAuthTokenImpl;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Base64;
import java.util.TreeSet;

// Handler value: handler.Handler
public class Handler implements RequestHandler<SQSEvent, Void>{
  private final TreeSet<Integer> processedMessagesHashCodes = new TreeSet<>();
  @Override
  public Void handleRequest(SQSEvent sqsEvent, Context context) {
    LambdaLogger logger = context.getLogger();

    String region = System.getenv("region");
    String secretArn = System.getenv("dbInstanceSecretArn");
    logger.log("Region: " + region + "\ndbInstanceSecretArn: " + secretArn);

    AWSSecretsManager secretsManager = AWSSecretsManagerClientBuilder.standard().withRegion(region).build();

    String secret;

    GetSecretValueRequest getSecretValueRequest = new GetSecretValueRequest().withSecretId(secretArn);

    GetSecretValueResult getSecretValueResult = secretsManager.getSecretValue(getSecretValueRequest);

    if (getSecretValueResult.getSecretString() != null)
      secret = getSecretValueResult.getSecretString();
    else
      secret = new String(Base64.getDecoder().decode(getSecretValueResult.getSecretBinary()).array());

    logger.log("Secret: " + secret);
    JsonObject secretJson = new Gson().fromJson(secret, JsonObject.class);


    String dbName = secretJson.get("dbname").getAsString();
    String dbHostName = secretJson.get("host").getAsString();
    String dbPort = secretJson.get("port").getAsString();
    String dbUsername = secretJson.get("username").getAsString();
    String dbPassword = secretJson.get("password").getAsString();

    logger.log("dbName: " + dbName);
    logger.log("dbHostName: " + dbHostName);
    logger.log("dbPort: " + dbPort);
    logger.log("dbUsername: " + dbUsername);
    logger.log("dbPassword: " + dbPassword);

    StringBuilder jdbc = new StringBuilder();

    jdbc.append("jdbc:mysql://");
    jdbc.append(dbHostName);
    jdbc.append(":");
    jdbc.append(dbPort);
//    jdbc.append("?user=");
//    jdbc.append(dbUsername);
//    jdbc.append("&password=");
//    jdbc.append(dbPassword);

    logger.log("jdbc: " + jdbc);

    try {
      Connection conn = DriverManager.getConnection(jdbc.toString(), dbUsername, dbPassword);
      logger.log("Connected to db");
      conn.close();
    } catch (SQLException e) {
      logger.log("Failed to connect to db: " + e.getMessage());
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